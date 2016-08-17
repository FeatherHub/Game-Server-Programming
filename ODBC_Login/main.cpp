#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

//ODBC MySql
#include "sql.h"
#include "sqlext.h"

//ODBC vars
SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;

//Connect datas
SQLCHAR* ODBC_Name = (SQLCHAR*)"MySource";
SQLCHAR* ODBC_ID = (SQLCHAR*)"";
SQLCHAR* ODBC_PW = (SQLCHAR*)"";

bool DBConnect();
bool DBExcuteSQL();
void DBDisconnect();

int main()
{
	if (DBConnect() == false)
	{
		cout << "DBConnect() fail" << endl;
		return 0;
	}

	if (DBExcuteSQL() == false)
	{
		cout << "DBExecute() fail" << endl;
		return 0;
	}

	DBDisconnect();

	return 0;
}

//Allocate ODBC handle and Connect to SQL
bool DBConnect()
{
	int res;

	//Envrionment handle
	res = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	if (res != SQL_SUCCESS) return false;

	res = SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (res != SQL_SUCCESS) return false;

	//DBC handle
	res = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	if (res != SQL_SUCCESS) return false;

	res = SQLConnectA(hDbc, ODBC_Name, SQL_NTS, ODBC_ID, SQL_NTS, ODBC_PW, SQL_NTS);
	if (res != SQL_SUCCESS) return false;

	res = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	if (res != SQL_SUCCESS) return false;

	return true;
}

bool AddUser(SQLWCHAR* userId, SQLWCHAR* userPw);
bool Login(SQLWCHAR* userId, SQLWCHAR* userPw);
bool DeleteUser(SQLWCHAR* userId);

bool DBExcuteSQL()
{
	//Korean
	AddUser(L"°£Àå", L"¾ûµ¢ÀÌ");

	//English
	AddUser(L"Car", L"Whoa");
	
	//Id overlapped
//	AddUser(L"Car", L"Whoa");

	//nonexisting Id
//	Login(L"Ear", L"Whoa");

	//wrong Password
//	Login(L"Car", L"Whoa");
	
	//valid user
//	Login(L"Car", L"Whoa");

	//invalid user
//	DeleteUser(L"Ear");

	//valid user
//	DeleteUser(L"Car");

	return true;
}

void DBDisconnect()
{
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}

//TO-DO 
//- Id/Pw empty check
//- Id overlapped check 
bool AddUser(SQLWCHAR* userId, SQLWCHAR* userPw)
{
	SQLExecDirect(hStmt, L"SET NAMES euckr", SQL_NTS);

	SQLWCHAR szId[32];
	int idLen = wcslen(userId);
	wcsncpy_s(szId, 32, userId, idLen);
	szId[idLen] = L'\0';

	SQLWCHAR szPw[32];
	int pwLen = wcslen(userPw);
	wcsncpy_s(szPw, 32, userPw, pwLen);
	szPw[pwLen] = L'\0';

	int res;

	SQLWCHAR* query = L"{CALL AddUser(?, ?)}";

	SQLINTEGER cbValue;

	cbValue = SQL_NTS;
	res = SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, idLen, 0, szId, 0, &cbValue);
	if (res != SQL_SUCCESS) return false;

	cbValue = SQL_NTS;
	res = SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, pwLen, 0, szPw, 0, &cbValue);
	if (res != SQL_SUCCESS) return false;

	//SQLSMALLINT paramNum = 2;
	//SQLNumParams(hStmt, &paramNum);

	//SQLHDESC hIpd = NULL;
	//res = SQLGetStmtAttr(hStmt, SQL_ATTR_IMP_PARAM_DESC, &hIpd, 0, 0);
	//if (res != SQL_SUCCESS) return false;

	//res = SQLSetDescField(hIpd, 1, SQL_DESC_NAME, L"@UserID", SQL_NTS);
	//if (res != SQL_SUCCESS) return false;
	//res = SQLSetDescField(hIpd, 2, SQL_DESC_NAME, L"@UserPW", SQL_NTS);
	//if (res != SQL_SUCCESS) return false;

	res = SQLExecDirect(hStmt, query, SQL_NTS);
	if (res != SQL_SUCCESS) return false;

	return true;
}

bool Login(SQLWCHAR* userId, SQLWCHAR* userPw)
{
	return true;
}

bool DeleteUser(SQLWCHAR* userId)
{
	return true;
}