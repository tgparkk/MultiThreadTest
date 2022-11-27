#pragma once

#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>

#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000

class CSqlConnect
{
public:
	CSqlConnect();
	~CSqlConnect();

public:
	void Connect();;

private:
	SQLHANDLE m_enviroment;
	SQLHDBC m_odbc;

};

