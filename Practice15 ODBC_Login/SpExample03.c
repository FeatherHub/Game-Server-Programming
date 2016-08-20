/*
#include <windows.h>
#include <sqltypes.h>
#include <sqlext.h>

SQLHDESC hIpd = NULL;
SQLHENV henv = NULL;
SQLHDBC hdbc = NULL;
SQLRETURN retcode;
SQLHSTMT hstmt = NULL;
SQLCHAR szQuote[50] = "100084";
SQLINTEGER cbValue = SQL_NTS;
 
int main() 
{
	retcode = SQLPrepare(hstmt, (SQLCHAR*)"{call SQLBindParameter(?)}", SQL_NTS);
	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0, szQuote, 0, &cbValue);
	retcode = SQLGetStmtAttr(hstmt, SQL_ATTR_IMP_PARAM_DESC, &hIpd, 0, 0);
	retcode = SQLSetDescField(hIpd, 1, SQL_DESC_NAME, "@quote", SQL_NTS);

	retcode = SQLExecute(hstmt);
}
*/