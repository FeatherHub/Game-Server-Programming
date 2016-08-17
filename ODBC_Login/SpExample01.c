/**********************************************************************
* FILENAME :    CallSPFindID
*
* DESCRIPTION :
*               Simple ODBC example to SELECT data from a table via a stored
*               procedure where two arrays of parameters are used, one
*               containing FirstNames and one contining offsets into the
*               FirstNames array. As each FirstName is processed the matching
*               offset value is passed back as the stored procedure return
*               value. This can then be used by the calling program to identify
*               which element in the FirstNames array was used to generate the
*               results set.
*
*               The stored procedure has 3 parameters,
*               in the form :
*
*               {? = CALL Find_ByID(?, ?)}
*
*		If the record IDs array contains 111,222,333,444 and the
*		offsets array contains 0,1,2,3, we get, for each record ID, the
*		result set with the return value equal to the offset into the
*		record IDs array that produced the result set. For 111, we
*		0, 222 we get 1, etc. In that way we can tie a results set back
*		to the data that produced it.
*
* ODBC USAGE :
*               Calls DropProcedure (), CreateProcedure () and ListProcedure ()
*               to recreate and display the stored procedure 'Find_ByID'.
*               SQLSetStmtAttr - to set : SQL_ATTR_PARAMSET_SIZE
*                                         SQL_ATTR_PARAM_STATUS_PTR
*                                         SQL_ATTR_PARAMS_PROCESSED_PTR
*               SQLBindParameter - to bind 3 paramaters for the stored proc :
*                                  Param 1 - RetParam - proc return variable
*                                  Param 2 - pPersonIDs array
*                                  Param 3 - pPersonIXs array
*               SQLExecDirect    - to execute the stored procedure
*               Loops using SQLNumResultCols(), SQLFetch(), SQLRowCount()
*               and SQLMoreResults() to retrieve results sets generated by
*               stored procedure.
*/
/*
#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>
*/
//#include "util.c"
//#include "CallSPUtil.c"

#define PARAM_ARRAY_SIZE 20 // Number of params in param array
/*
int main() 
{

	SQLHENV  henv = SQL_NULL_HENV;   	// Environment
	SQLHDBC  hdbc = SQL_NULL_HDBC;   	// Connection handle
	SQLHSTMT hstmt = SQL_NULL_HSTMT;  	// Statement handle
	SQLRETURN retcode;			// Return status

								// Return index pPersonIXs[param no] from stored Proc
	SQLCHAR * strCallSP = "{? = CALL Find_ByID(?, ?)}";
	char    * strProcName = "Find_ByID";
	SQLSMALLINT columns; 		// Number of columns in result-set
	int i, count;

	SQLINTEGER  pPersonIDs[PARAM_ARRAY_SIZE] =
	{ 1,2,3,4,5,6,7,8,9,10,
		11,12,13,14,15,16,17,18,19,20 };
	SQLINTEGER  pPersonIXs[PARAM_ARRAY_SIZE] =
	{ 0,1,2,3,4,5,6,7,8,9,
		10,11,12,13,14,15,16,17,18,19 };

	SWORD  RetParam;
	SQLLEN cbRetParam;

	SQLUSMALLINT ParamStatusArray[PARAM_ARRAY_SIZE];
	SQLINTEGER   ParamsProcessed = 0;

	// Allocate an environment handle
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	CHECK_ERROR(retcode, "SQLAllocHandle(ENV)", henv, SQL_HANDLE_ENV);

	// Set ODBC Version
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,
		(void *)SQL_OV_ODBC3, 0);
	CHECK_ERROR(retcode, "SQLSetEnvAttr(SQL_ATTR_ODBC_VERSION)",
		henv, SQL_HANDLE_ENV);

	// Allocate a connection handle
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	CHECK_ERROR(retcode, "SQLAllocHandle(SQL_HANDLE_DBC)",
		hdbc, SQL_HANDLE_DBC);

	// DSN
	retcode = SQLDriverConnect(hdbc, NULL, "DSN=DATASOURCE;", SQL_NTS,
		NULL, 0, NULL, SQL_DRIVER_COMPLETE);
	CHECK_ERROR(retcode, "SQLDriverConnect(DATASOURCE)",
		hdbc, SQL_HANDLE_DBC);

	retcode = DropProcedure(hdbc, strProcName);
	retcode = CreateProcedure(hdbc, strProcName);
	retcode = ListProcedure(hdbc, strProcName);

	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	CHECK_ERROR(retcode, "SQLAllocHandle(SQL_HANDLE_STMT)",
		hstmt, SQL_HANDLE_STMT);

	// Setup for parameter array processing
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMSET_SIZE,
		(SQLPOINTER)PARAM_ARRAY_SIZE, 0);
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAM_STATUS_PTR,
		ParamStatusArray, PARAM_ARRAY_SIZE);
	retcode = SQLSetStmtAttr(hstmt, SQL_ATTR_PARAMS_PROCESSED_PTR,
		&ParamsProcessed, 0);

	// Bind the output parameter to variable RetParam.
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT,
		SQL_INTEGER, 0, 0, &RetParam, 0, &cbRetParam);
	CHECK_ERROR(retcode, "SQLBindParameter(1)",
		hstmt, SQL_HANDLE_STMT);

	// Bind array values of parameter 2 data in
	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_LONG,
		SQL_INTEGER, 0, 0, pPersonIDs, 0, NULL);
	CHECK_ERROR(retcode, "SQLBindParameter(2)",
		hstmt, SQL_HANDLE_STMT);

	// Bind array values of parameter 3 data in
	retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG,
		SQL_INTEGER, 0, 0, pPersonIXs, 0, NULL);
	CHECK_ERROR(retcode, "SQLBindParameter(3)",
		hstmt, SQL_HANDLE_STMT);


	retcode = SQLExecDirect(hstmt, strCallSP, SQL_NTS);
	CHECK_ERROR(retcode, "SQLExecDirect(SQL_HANDLE_STMT)",
		hstmt, SQL_HANDLE_STMT);

	printf("\nPersonID   Firstname     Surname    Address       City");
	printf("\n--------   ---------     -------    -------       ----\n");

	SQLLEN  indicator, RowCount;
	char *buf = malloc(255);
	do {
		// SQLNumResultCols() returns number of columns in result set.
		// If non zero use SQLFetch until SQL_NO_DATA returned
		retcode = SQLNumResultCols(hstmt, &columns);
		CHECK_ERROR(retcode, "SQLNumResultCols()", hstmt, SQL_HANDLE_STMT);
		if (columns > 0) {
			printf("\nRows ...");
			while (SQLFetch(hstmt) != SQL_NO_DATA) {
				// Loop through the columns
				memset(buf, ' ', 255);
				printf("\n");
				for (i = 1; i <= columns; i++) {
					// retrieve column data as a string
					retcode = SQLGetData(hstmt, i, SQL_C_CHAR,
						buf, 255, &indicator);
					if (SQL_SUCCEEDED(retcode)) {
						// Handle null columns
						if (indicator == SQL_NULL_DATA)
							strcpy(buf, "NULL");
						buf = rtrim(buf, ' ');
						printf("%10s ", buf);
					}
				}
			}
			printf("\nEnd ...");
		}
		else {
			// SQLRowCount returns number of rows affected by INSERT, UPDATE,
			// DELETE or (if supported by the driver) number of rows returned
			// by a SELECT
			retcode = SQLRowCount(hstmt, &RowCount);
			CHECK_ERROR(retcode, "SQLRowCount()", hstmt, SQL_HANDLE_STMT);
			printf("\nRecord ID offset was : %i\nRecord ID was : %i",
				RetParam, pPersonIDs[RetParam]);
		}

		if (columns == 0) {
			printf("\n-----------");
		}
	} while ((retcode = SQLMoreResults(hstmt)) == SQL_SUCCESS);

	// After SQLMoreResults() returns SQL_NO_DATA,
	if (retcode == SQL_NO_DATA && columns > 0) {
		printf("\nRecord ID offset was : %i\nRecord ID was : %i",
			RetParam, pPersonIDs[RetParam]);
		printf("\n-----------");
	}

exit:

	free(buf);

	printf("\nComplete.\n");

	// Free handles
	// Statement
	if (hstmt != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	// Connection
	if (hdbc != SQL_NULL_HDBC) {
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	}

	// Environment
	if (henv != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return 0;
}
*/