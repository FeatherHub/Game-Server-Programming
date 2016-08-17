/**********************************************************************
* FILENAME :    CallSPUtil.c
*
* DESCRIPTION :
*               Utility functions for Stored Procedure exmples:
*               	DropProcedure () 	- Drop strored procedure
*               	CreateProcedure ()	- Create stored proecdure
*               	ListProcedure()		- Lookup and list stored procudure
*
*               Procedure referenced are:
*
*                   {? = CALL Insert_Record (?, ?, ?, ?, ?, ?)}
*                   {CALL SelectRecords()}
*                   {CALL Find_Record(?)}
*                   {CALL Multi_Result_Sets()}
*                   {? = CALL InOutRet_Params (?, ?)}
*                   {? = CALL Find_ByID (?, ?)}
*                   {? = CALL Find_ByName (?, ?)}
*                   {? = CALL Edit_Record (?, ?, ?, ?, ?, ?)}
*
*               See below for individual details.
*
* ODBC USAGE :
*           	DropProcedure()   - Calls SQLExecDirect() with statement that
*           			    checks procedure exist before dropping it.
*           	CreateProcedure() - Calls SQLExecDirect() to create a stored
*           			    procedure.
*           			    Insert_Record for use in these tests.
*           	List Procedure()  - Used to show one of the above procedures
*           			    is defined by lookup using SQLProcedures()
*           			    and SQLBindCol() for the columns returned.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <string.h>

#define BUFF_SIZE 255
#define FIRSTNAME_LEN 256
#define LASTNAME_LEN 256
#define ADDRESS_LEN 256
#define CITY_LEN 256

//
// Create Stored procedure
// Procedure is called either Insert_Record or Select_Records
//
SQLRETURN CreateProcedure(SQLHDBC hdbc, char *ProcName) {

	SQLHSTMT hstmt = SQL_NULL_HSTMT;  	// Statement handle
	SQLRETURN retcode = SQL_SUCCESS;	// Return status

	SQLCHAR strCreateSP[1024];

	if (strcmp(ProcName, "Insert_Record") == 0) {
		// Stored Procedure: Insert_Record
		//
		// Called by: CallSPINsertRec.c
		//
		// Usage {? = CALL Insert_Record (?, ?, ?, ?, ?, ?)}
		//
		// Has 7 Parameters - 1   - Proc return value (Prov RETURN value)
		//		      2-5 - Table columns (INPUT)
		//                    6   - Matching records count (OUTPUT)
		//		      7   - Total records Count (OUTPUT)
		//
		// Creates Stored Proc 'Insert_Record'
		// Inserts record into TestTBL1 using params 2-5
		// Generates result set of entire table
		// Generates result set of records matching FirstName just inserted
		// Returns number of records in table
		// Returns number of records matching FirstName just inserted
		// Function returns 100
		//
		strcpy(strCreateSP, "CREATE PROCEDURE Insert_Record ("
			"@pFirstName nvarchar(256), @pLastName varchar(256), "
			"@pAddress varchar(256), @pCity varchar(256), "
			"@MatchingRecs int OUTPUT, @TotalRecs int OUTPUT) AS "
			"INSERT INTO TestTBL1 (FirstName, LastName, Address, City) "
			"VALUES (@pFirstName, @pLastName, @pAddress, @pCity); "
			"SELECT * FROM TestTBL1 ORDER BY PersonID; "
			"SELECT * FROM TestTBL1 WHERE FirstName=@pFirstName; "
			"SELECT @MatchingRecs=count(*) FROM TestTBL1 WHERE "
			"FirstName=@pFirstName; "
			"SELECT @TotalRecs=count(*) FROM TestTBL1; "
			"RETURN 100;");

	}
	else if (strcmp(ProcName, "Select_Records") == 0) {
		// Stored Procedure : 'Select_Records' (no param example)
		// Called By : CallSP.c
		//
		// Usage {CALL SelectRecords()}
		//
		// Has no parameters
		//
		// Creates Stored Procedure 'Select_Records'
		// Generates result set of entire table
		//
		strcpy(strCreateSP, "CREATE PROCEDURE Select_Records AS "
			"SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1");

	}
	else if (strcmp(ProcName, "Find_Record") == 0) {
		// Stored Procedure : 'Find_Record'
		// Called By : CallSPWithParam.c
		//
		// Usage {CALL Find_Record(?)}
		//
		// Has 1 Input Parameter - pPersonID (INPUT)
		//
		// Creates stored procedure 'Find_Record'
		// Generates result set of records matching pPersonID
		//
		strcpy(strCreateSP, "CREATE PROCEDURE Find_Record (@pPersonID INT) "
			"AS SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1 WHERE PersonID=@pPersonID");

	}
	else if (strcmp(ProcName, "Multi_Result_Sets") == 0) {
		// Stored Procedure : 'Multi_Result_Sets'
		// Called By : CallSPMultiResultSet.c
		//
		// Usage {CALL Multi_Result_Sets()}
		//
		// Has no parameters
		//
		// Creates stored procedure Multi_Result_Sets
		// Generates results set for all records in TestTBL1
		// Generates results set for all records in TestTBLCopy
		//
		strcpy(strCreateSP, "CREATE PROCEDURE Multi_Result_Sets AS "
			"SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1; "
			"SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1Copy");

	}
	else if (strcmp(ProcName, "InOutRet_Params") == 0) {
		// Stored Procedure : 'InOutRet_Params'
		// Called By : CallSPWithInOutParams.c
		//
		// Usage {? = CALL InOutRet_Params (?, ?)}
		//
		//  Has 3 Parameters - 1 - Proc return value (Proc RETURN value)
		//		       2 - table field PersonID (INPUT)
		//                     3 - matching record count (OUTPUT)
		//
		// Creates stored procedure 'InOutRet_Params'
		// Generates result set for records matching the input parameter
		// Returns number of records in result set
		// Function returns 100
		//
		strcpy(strCreateSP, "CREATE PROCEDURE InOutRet_Params ("
			"@pPersonID int, @MatchingRecs int OUTPUT) AS "
			"SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1 WHERE PersonID=@pPersonID; "
			"SELECT @MatchingRecs=count(*) FROM TestTBL1 WHERE "
			"PersonID=@pPersonID; "
			"RETURN 100;");

	}
	else if (strcmp(ProcName, "Find_ByID") == 0) {
		// Stored Procedure : Find_ByID
		// Called By : CallSPFindID.c
		//
		// Usage {? = CALL Find_ByID (?, ?)}
		//
		// Uses 3 Parameters - 1 - return value
		//		       2 - pPersonIDs parameter array element (INPUT)
		//		       3 - pPersonIXs parameter array element (INPUT)
		//
		// Creates stored procedure Find_ByID
		// Generates a result set that matches the pPersonID parameter
		// Returns the pPersonIX. This can be used to tie the result set
		// to the pPersonID that generated it
		strcpy(strCreateSP, "CREATE PROCEDURE Find_ByID "
			"(@pPersonID int, @pPersonIX int) AS SELECT PersonID, FirstName, "
			"LastName, Address, City FROM TestTBL1 WHERE PersonID=@pPersonID; "
			"RETURN @pPersonIX");

	}
	else if (strcmp(ProcName, "Find_ByName") == 0) {
		// Stored Procedure : Find_ByName
		// Called By : CalLSPFindName.c
		//
		// Usage {? = CALL Find_ByName (?, ?)}
		//
		// Uses 3 Parameters - 1 - return value
		//		       2 - pFirstName parameter array element (INPUT)
		//		       3 - pPersonIXs parameter array element (INPUT)
		//
		// Creates stored procedure Find_ByName
		// Generates a result set that matches the pFirstName parameter
		// Returns the pPersonIX. This can be used to tie the result set
		// to the pFirstName that generated it
		strcpy(strCreateSP, "CREATE PROCEDURE Find_ByName ("
			"@pFirstName varchar(2), @pPersonIX int) "
			"AS SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1 WHERE FirstName LIKE @pFirstName+'%'; "
			"RETURN @pPersonIX");

	}
	else if (strcmp(ProcName, "Edit_Record") == 0) {
		// Stored Procedure : Edit_Record
		// Called By : CallSPEditRecord.c
		//
		// Usage {? = CALL Edit_Record (?, ?, ?, ?, ?, ?)}
		//
		// Uses 7 Parameters - 1   - Proc return value   - RETURNED
		//                     2   - Action - See below  - INPUT
		//                     3   - RecordID            - INPUT
		//                     4-7 - if action is SELECT
		//                         - old column values   - OUTPUT
		//                     4-7 - if action is UPDATE or INSERT
		//                         - new column values   - INPUT
		//		       4-7 - if action is DELETE
		//			   - unused, delete is via RecordID
		//
		// Action values are SELECT RECORD 0
		// 					 UPDATE 	   1
		//					 INSERT        2
		//					 DELETE        3
		//					 SELECT ALL    4
		//

		strcpy(strCreateSP, "CREATE PROCEDURE Edit_Record ("
			"@action int, @pPersonID int OUTPUT, "
			"@pFirstName nvarchar(256), @pLastName nvarchar(256), "
			"@pAddress nvarchar(256), @pCity nvarchar(256)) "
			"AS "
			"IF (@action=0) BEGIN "
			"SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1 WHERE PersonID=@pPersonID; "
			"END "
			"ELSE "
			"IF (@action=1) BEGIN "
			"UPDATE TestTBL1 "
			"SET FirstName=@pFirstName, LastName=@pLastName, "
			"Address=@pAddress, City=@pCity "
			"WHERE PersonID=@pPersonID; "
			"END "
			"ELSE "
			"IF (@action=2) BEGIN "
			"INSERT INTO TestTBL1 "
			"(FirstName, LastName, Address, City) "
			"VALUES (@pFirstName, @pLastName, @pAddress, @pCity);"
			"SET @pPersonID=@@IDENTITY; "
			"END "
			"ELSE "
			"IF (@action=3) BEGIN "
			"DELETE FROM TestTBL1 WHERE PersonID=@pPersonID; "
			"END "
			"ELSE "
			"IF (@action=4) BEGIN "
			"SELECT PersonID, FirstName, LastName, Address, City "
			"FROM TestTBL1 ORDER BY PersonID; "
			"END "
			"RETURN @action");
	}

	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	CHECK_ERROR(retcode, "SQLAllocHandle(SQL_HANDLE_STMT)",
		hstmt, SQL_HANDLE_STMT);

	// Execute Create Procedure
	retcode = SQLExecDirect(hstmt, strCreateSP, SQL_NTS);
	CHECK_ERROR(retcode, "SQLExecDirect(CREATESP)",
		hstmt, SQL_HANDLE_STMT);

exit:

	// Free statement handle
	if (hstmt != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	return retcode;
}

//
// Function to drop existing stored proc. Either Insert_Record or
// Select_Records, which are the two we are using in these examples.
//
SQLRETURN DropProcedure(SQLHDBC  hdbc, char *ProcName) {

	SQLHSTMT hstmt = SQL_NULL_HSTMT;  	// Statement handle
	SQLRETURN retcode = SQL_SUCCESS;	// Return status

	SQLCHAR   strDropSP[1024];

	sprintf(strDropSP, "IF EXISTS (SELECT * FROM sys.objects WHERE "
		"type='P' AND name='%s') DROP PROCEDURE %s", ProcName, ProcName);

	// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	CHECK_ERROR(retcode, "SQLAllocHandle(SQL_HANDLE_STMT)", hstmt,
		SQL_HANDLE_STMT);

	// Execute Drop Procedure
	retcode = SQLExecDirect(hstmt, strDropSP, SQL_NTS);
	CHECK_ERROR(retcode, "SQLExecDirect(SQL_HANDLE_STMT)", hstmt,
		SQL_HANDLE_STMT);

exit:

	// Free statement handle
	if (hstmt != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	return retcode;
}

//
// Function to list the stored proc we are using in this example.
// Uses SQLProcedures () to find stored proc Insert_Record or Select_Records.
//
SQLRETURN ListProcedure(SQLHDBC hdbc, char *ProcName) {

	// Columns for binding to SQLProcedures() results set
	SQLCHAR strProcedureCat[BUFF_SIZE];
	SQLCHAR strProcedureSchema[BUFF_SIZE];
	SQLCHAR strProcedureName[BUFF_SIZE];
	SQLSMALLINT ProcedureType;

	SQLLEN  lenProcedureCat, lenProcedureSchema;
	SQLLEN  lenProcedureName, lenProcedureType;

	int header = 0;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;  	// Statement handle
	SQLRETURN retcode = SQL_SUCCESS;	// Return status

										// Allocate a statement handle
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	CHECK_ERROR(retcode, "SQLAllocHandle(SQL_HANDLE_STMT)",
		hstmt, SQL_HANDLE_STMT);

	retcode = SQLProcedures(hstmt,
		NULL, 0,
		NULL, 0,
		ProcName, strlen(ProcName));

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		// Bind columns in result set to buffers
		// column 1 is the catalogue
		SQLBindCol(hstmt, 1, SQL_C_CHAR, strProcedureCat,
			sizeof(strProcedureCat), &lenProcedureCat);

		// column 2 is the schema
		SQLBindCol(hstmt, 2, SQL_C_CHAR, strProcedureSchema,
			sizeof(strProcedureSchema), &lenProcedureSchema);

		// column 3 is the procedure name
		SQLBindCol(hstmt, 3, SQL_C_CHAR, strProcedureName,
			sizeof(strProcedureName), &lenProcedureName);

		// columns 4 to 7 are skipped
		// column 8 is the procedure type
		SQLBindCol(hstmt, 8, SQL_C_SHORT, &ProcedureType,
			sizeof(ProcedureType), &lenProcedureType);

		// fetch results (only expecting one row)
		while (SQL_SUCCESS == retcode) {
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO) {
				extract_error("SQLFetch Error : ", hstmt, SQL_HANDLE_STMT);
			}

			if (header++ == 0) {
				printf("\nDSN : SQLSRV");
			}

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				printf("\nProcedure Cat    : %s\n", strProcedureCat);
				printf("Procedure Schema : %s\n", strProcedureSchema);
				printf("Procedure Name   : %s\n", strProcedureName);
				printf("Procedure Type   : ");
				switch (ProcedureType) {
				case SQL_PT_PROCEDURE:
					printf("%s\n", "SQL_PT_PROCEDURE");
					break;
				case SQL_PT_FUNCTION:
					printf("%s\n", "SQL_PT_FUNCTION");
					break;
				case SQL_PT_UNKNOWN:
				default:
					printf("%s\n", "SQL_PT_UNKNOWN");
					break;
				}
			}

			if (retcode == SQL_NO_DATA && header == 1) {
				printf("(NO DATA)\n");
			}
		}
	}

exit:

	// Free statement handle
	if (hstmt != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	// if last status is no data it is ok
	if (retcode == SQL_NO_DATA) {
		retcode = SQL_SUCCESS;
	}

	return retcode;
}
