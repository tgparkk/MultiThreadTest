#include "pch.h"
#include "CSqlConnect.h"

CSqlConnect::CSqlConnect()
	:m_enviroment()
	,m_odbc()
{

}

CSqlConnect::~CSqlConnect()
{
	if (m_odbc != SQL_NULL_HDBC)
		SQLFreeHandle(SQL_HANDLE_DBC, m_odbc);
	if (m_enviroment != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, m_enviroment);
}

void CSqlConnect::Connect()
{
	SQLSetEnvAttr(NULL,
		SQL_ATTR_CONNECTION_POOLING,
		(SQLPOINTER)SQL_CP_ONE_PER_DRIVER,
		SQL_IS_INTEGER);

	if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_ENV,
		SQL_NULL_HANDLE, &m_enviroment))
	{
		SQLSetEnvAttr(m_enviroment, SQL_ATTR_ODBC_VERSION,
			(SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);

		SQLSetEnvAttr(m_enviroment, SQL_ATTR_CP_MATCH,
			(SQLPOINTER)SQL_CP_RELAXED_MATCH, SQL_IS_INTEGER);

		if (SQL_ERROR != SQLAllocHandle(SQL_HANDLE_DBC,
			m_enviroment, &m_odbc))
		{
			RETCODE ret_code = SQLConnect(m_odbc,
				(SQLWCHAR*)L"multiThreadTest", SQL_NTS,
				(SQLWCHAR*)L"tg", SQL_NTS,
				(SQLWCHAR*)L"tg!1", SQL_NTS);

			if (ret_code == SQL_SUCCESS || ret_code == SQL_SUCCESS_WITH_INFO)
			{
				int temp = 1;
			}
			else
			{
				if (m_odbc != SQL_NULL_HDBC)
					SQLFreeHandle(SQL_HANDLE_DBC, m_odbc);
				if (m_enviroment != SQL_NULL_HENV)
					SQLFreeHandle(SQL_HANDLE_ENV, m_enviroment);
			}
		}


	}
}
