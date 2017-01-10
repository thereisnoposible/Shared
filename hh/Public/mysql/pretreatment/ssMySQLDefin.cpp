#include "fcMySQLDefin.h"
#include "fcMySQL.h"
#include "../fcAssert.h"


namespace FoldCat
{
namespace fcMySQL
{

fcMySQLQuery::fcMySQLQuery(void)
{
	m_pConnect = 0;
	m_hStmt = 0;
	m_pMain = 0;
	m_resFlag = 0;
	m_numIn = 0;
	m_numOut = 0;
	m_in = 0;
	m_out = 0;
	m_strInParam = 0;
	m_strOutParam = 0;
	m_inParam = 0;
	m_outParam = 0;
	m_insertID = 0;
}

fcMySQLQuery::~fcMySQLQuery(void)
{
	Exit();
}

bool fcMySQLQuery::Init(fcMySQLClient * pMain, MYSQL * pConn)
{
	if (m_hStmt || m_pConnect) return false;

	m_pMain = pMain;
	m_pConnect = pConn;
	m_hStmt = mysql_stmt_init(pConn);
	if (!m_hStmt)
	{
		m_pMain->SaveLog("mysql_stmt_init faild!\n");
		return false;
	}
	return true;
}

void fcMySQLQuery::Exit()
{
	if (m_hStmt)
	{
		ClearResult();
		mysql_stmt_close(m_hStmt);
		m_hStmt = 0;
	}
	for (u8 i=0; i<m_numIn && m_inParam; ++i)
	{
		FC_SAFE_DEL_ARRAY(m_strInParam)
		FC_SAFE_DEL_ARRAY(m_in)
		FC_SAFE_DELETE(m_inParam[i])
	}
	FC_SAFE_DEL_ARRAY(m_inParam)
	for (u8 i=0; i<m_numOut && m_outParam; ++i)
	{
		FC_SAFE_DEL_ARRAY(m_strOutParam)
		FC_SAFE_DEL_ARRAY(m_out)
		FC_SAFE_DELETE(m_outParam[i])
	}
	FC_SAFE_DEL_ARRAY(m_outParam)
}

bool fcMySQLQuery::PrepareQuery(const char* pIn, const char* pOut, const char* pSql, const u8 resultFlag)
{
	if (m_in || m_out)
		return false;
	m_sql = pSql;
	m_numIn = pIn?(u8)strlen(pIn):0;
	m_numOut = pOut?(u8)strlen(pOut):0;
	if (m_numIn>DB_PARAM_FLAG_NUM || m_numOut>DB_PARAM_FLAG_NUM)
	{
		m_pMain->SaveLog("mysql param num error!\n");
		return false;
	}

	m_resFlag = resultFlag;
	//输入参数
	if (m_numIn>0)
	{
		m_in = new MYSQL_BIND[m_numIn];
		memset(m_in, 0, sizeof(MYSQL_BIND)*m_numIn);
		m_strInParam = new char[m_numIn];
		memcpy(m_strInParam, pIn, m_numIn);
		m_inParam = new QueryParam_s*[m_numIn];
		for (u8 i=0; i<m_numIn; ++i)
		{
			switch (m_strInParam[i])
			{
			case '1':
				m_inParam[i] = new QueryInt_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '2':
				m_inParam[i] = new QueryUInt_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '3':
				m_inParam[i] = new QueryChar_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '4':
				m_inParam[i] = new QueryUChar_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '5':
				m_inParam[i] = new QueryTime_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '6':
				m_inParam[i] = new QueryName_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '7':
				m_inParam[i] = new QuerySBuffer_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '8':
				m_inParam[i] = new QueryString_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case '9':
				m_inParam[i] = new QueryBuffer_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case 'a':
				m_inParam[i] = new QueryText_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case 'b':
				m_inParam[i] = new QueryLBuffer_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case 'c':
				m_inParam[i] = new QueryInt64_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case 'd':
				m_inParam[i] = new QueryUInt64_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case 'e':
				m_inParam[i] = new QueryShort_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			case 'f':
				m_inParam[i] = new QueryUShort_s;
				m_inParam[i]->Bind(m_in[i], 0);
				break;
			default:
				m_pMain->SaveLog("PrepareQuery:mysql in-param flag error!\n");
				return false;
			}
		}
	}
	//输出参数
	if (m_numOut>0)
	{
		if (resultFlag & DB_RESULT_MULTI) m_resultSet.Init(128, true);
		m_result.Set(64, true);
		m_out = new MYSQL_BIND[m_numOut];
		memset(m_out, 0, sizeof(MYSQL_BIND)*m_numOut);
		m_strOutParam = new char[m_numOut];
		memcpy(m_strOutParam, pOut, m_numOut);
		m_outParam = new QueryParam_s*[m_numOut];
		for (u8 i=0; i<m_numOut; ++i)
		{
			switch (m_strOutParam[i])
			{
			case '1':
				m_outParam[i] = new QueryInt_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '2':
				m_outParam[i] = new QueryUInt_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '3':
				m_outParam[i] = new QueryChar_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '4':
				m_outParam[i] = new QueryUChar_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '5':
				m_outParam[i] = new QueryTime_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '6':
				m_outParam[i] = new QueryName_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '7':
				m_outParam[i] = new QuerySBuffer_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '8':
				m_outParam[i] = new QueryString_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case '9':
				m_outParam[i] = new QueryBuffer_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case 'a':
				m_outParam[i] = new QueryText_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case 'b':
				m_outParam[i] = new QueryLBuffer_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case 'c':
				m_outParam[i] = new QueryInt64_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case 'd':
				m_outParam[i] = new QueryUInt64_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case 'e':
				m_outParam[i] = new QueryShort_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			case 'f':
				m_outParam[i] = new QueryUShort_s;
				m_outParam[i]->Bind(m_out[i], 1);
				break;
			default:
				m_pMain->SaveLog("PrepareQuery:mysql out-param flag error!\n");
				return false;
			}
		}
	}
	int rt = mysql_stmt_prepare(m_hStmt, pSql, (unsigned long)strlen(pSql));
	if (0!=rt)
	{
		CheckError(1, rt);
		return false;
	}
	return true;
}

int fcMySQLQuery::ExecuteQueryVariable(i8 num, ...)
{
	if ((u8)num<m_numIn) return DB_QUERY_PARAM_ERROR;
	//参数赋值
	ClearParam();
	if (m_numIn>0)
	{
		va_list arglist;
		va_start(arglist, num);
		for (u8 i=0; i<m_numIn; ++i)
		{
			switch (m_strInParam[i])
			{
			case '1':
				{
					int x = va_arg(arglist, int);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case '2':
				{
					unsigned int x = va_arg(arglist, unsigned int);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num < 0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case '3':
				{
					char x = va_arg(arglist, int);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case '4':
				{
					unsigned char x = va_arg(arglist, unsigned int);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case '5':
				{
					void* x = va_arg(arglist, void*);
					if (!m_inParam[i]->SetValue(x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'b':
				{
					unsigned int xlen = va_arg(arglist, unsigned int);
					char* px = va_arg(arglist, char*);
					if (!m_inParam[i]->SetValue(px, xlen))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					num -= 2;
					if (num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case 'c':
				{
					long long x = va_arg(arglist, long long);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case 'd':
				{
					unsigned long long x = va_arg(arglist, unsigned long long);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
			case 'e':
				{
					short x = va_arg(arglist, int);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			case 'f':
				{
					unsigned long long x = va_arg(arglist, unsigned long long);
					if (!m_inParam[i]->SetValue(&x, 0))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					if (--num<0)
					{
						m_pMain->SaveLog("mysql in-param num error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
				}
				break;
			default:
				m_pMain->SaveLog("mysql in-param flag error!\n");
				FC_ASSERT_NORMAL(false);
				return DB_QUERY_PARAM_ERROR;
			}
		}
		va_end(arglist);
	}
	int ret = 0;
	if (m_numIn>0)
	{
		ret = ExecuteSQL(m_in);
	}
	else
	{
		ret = ExecuteSQL(0);
	}
	if (ret!=0)
	{
		ret = DB_QUERY_ERROR;
		goto EXECUTE_RETURN;
	}
	if (m_numOut>0)
	{
		ret = GetSQLResult(m_out);
		if (ret!=0)
		{
			ret = DB_QUERY_RESULE_ERROR;
			goto EXECUTE_RETURN;
		}
	}
	FormatResult(ret);
EXECUTE_RETURN:
	ClearResult();
	return ret;
}

int fcMySQLQuery::ExecuteQueryStream(const u32 len, const char* data)
{
	char* pData = FC_CONST_CAST(char*, data);
	u32 dataLen = FC_STATIC_CAST(u32, len);
	//参数赋值
	ClearParam();
	if (m_numIn>0)
	{
		for (u8 i=0; i<m_numIn; ++i)
		{
			switch (m_strInParam[i])
			{
			case '1':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_i32sz;
				if (dataLen<cg_i32sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_i32sz;
				break;
			case '2':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_u32sz;
				if (dataLen<cg_u32sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_u32sz;
				break;
			case '3':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_i8sz;
				if (dataLen<cg_i8sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_i8sz;
				break;
			case '4':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_u8sz;
				if (dataLen<cg_u8sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_u8sz;
				break;
			case '5':
				m_inParam[i]->SetValue(pData, 0);
				pData += sizeof(MYSQL_TIME);
				if (dataLen<sizeof(MYSQL_TIME))
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= sizeof(MYSQL_TIME);
				break;
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'b':
				{
					u32 xlen = *FC_REINTERPRET_CAST(u32*, pData);
					pData += cg_u32sz;
					if (!m_inParam[i]->SetValue(pData, xlen))
					{
						m_pMain->SaveLog("mysql in-param error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					pData += xlen;
					if (dataLen<xlen+cg_u32sz)
					{
						m_pMain->SaveLog("mysql in-param len error!\n");
						FC_ASSERT_NORMAL(false);
						return DB_QUERY_PARAM_ERROR;
					}
					dataLen -= xlen+cg_u32sz;
				}
				break;
			case 'c':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_i64sz;
				if (dataLen<cg_i64sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_i64sz;
				break;
			case 'd':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_u64sz;
				if (dataLen<cg_u64sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_u64sz;
				break;
			case 'e':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_i16sz;
				if (dataLen<cg_i16sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_i16sz;
				break;
			case 'f':
				m_inParam[i]->SetValue(pData, 0);
				pData += cg_u16sz;
				if (dataLen<cg_u16sz)
				{
					m_pMain->SaveLog("mysql in-param len error!\n");
					FC_ASSERT_NORMAL(false);
					return DB_QUERY_PARAM_ERROR;
				}
				dataLen -= cg_u16sz;
				break;
			default:
				m_pMain->SaveLog("mysql in-param error!\n");
				FC_ASSERT_NORMAL(false);
				return DB_QUERY_PARAM_ERROR;
			}
		}
	}
	int ret = 0;
	if (m_numIn>0)
	{
		ret = ExecuteSQL(m_in);
	}
	else
	{
		ret = ExecuteSQL(0);
	}
	if (ret!=0)
	{
		ret = DB_QUERY_ERROR;
		goto EXECUTE_RETURN;
	}
	if (m_numOut>0)
	{
		ret = GetSQLResult(m_out);
		if (ret!=0)
		{
			ret = DB_QUERY_RESULE_ERROR;
			goto EXECUTE_RETURN;
		}
	}
	FormatResult(ret);
EXECUTE_RETURN:
	ClearResult();
	return ret;
}

void fcMySQLQuery::ClearResult()
{
	mysql_stmt_free_result(m_hStmt);
	while (0 == mysql_next_result(m_pConnect))
	{
		MYSQL_RES* res = mysql_store_result(m_pConnect);
		mysql_free_result(res);
	}
}

void fcMySQLQuery::CheckError(int fun, int ret)
{
	if (fun==6 && ret!=1)
	{
		if (ret==MYSQL_NO_DATA)
		{
			m_pMain->SaveLog("mysql error! fun:%d, MYSQL_NO_DATA\n");
		}
		else if (ret==MYSQL_DATA_TRUNCATED)
		{
			m_pMain->SaveLog("mysql error! fun:%d, MYSQL_DATA_TRUNCATED\n");
		}
		return;
	}

	int err = mysql_stmt_errno(m_hStmt);
	m_pMain->SaveLog("mysql error! fun:%d, err1:%d, err2:%d, prompt:%s\n", fun, ret, err, mysql_stmt_error(m_hStmt));
	if (err==0 || err==2013 || err==2006)
	{
		m_pMain->Reconnect();
	}
	else if (err==1062)
	{
	}
	else
	{
		FC_ASSERT_NORMAL(false);
	}
}

int fcMySQLQuery::ExecuteSQL(MYSQL_BIND *pin)
{
	int rt = 0;
	if (pin)
	{
		rt = mysql_stmt_bind_param(m_hStmt, pin);
		if (0!=rt)
		{
			CheckError(2, rt);
			return rt;
		}
	}
	rt = mysql_stmt_execute(m_hStmt);
	if (0!=rt)
	{
		CheckError(3, rt);
	}
	return rt;
}

int fcMySQLQuery::GetSQLResult( MYSQL_BIND *pout )
{
	int rt = mysql_stmt_bind_result(m_hStmt, pout);
	if (0!=rt)
	{
		CheckError(4, rt);
		return rt;
	}
	rt = mysql_stmt_store_result(m_hStmt);
	if (0!=rt)
	{
		CheckError(5, rt);
	}
	return rt;
}

void fcMySQLQuery::ClearParam()
{
	for (u8 i=0; i<m_numOut; ++i)
		m_outParam[i]->Reset();
	for (u8 i=0; i<m_numIn; ++i)
		m_inParam[i]->Reset();
}

void fcMySQLQuery::FormatResult(int& row)
{
	m_result.Reset();
	m_resultSet.Empty();
	if (m_resFlag & DB_RESULT_NULL)
	{
		row = (int)mysql_stmt_affected_rows(m_hStmt);
		if (row!=1)
		{
			row = 0;
		}
	}
	else if (m_resFlag & DB_RESULT_ONE)
	{
		row = (int)mysql_stmt_num_rows(m_hStmt);
		if (row==1)
		{
			int err = mysql_stmt_fetch(m_hStmt);
			if (0!=err)
			{
				row = 0;
				CheckError(6, err);
				return;
			}
			for (u8 i=0; i<m_numOut; ++i)
			{
				switch (m_strOutParam[i])
				{
				case '1':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i32sz);
					break;
				case '2':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u32sz);
					break;
				case '3':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i8sz);
					break;
				case '4':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u8sz);
					break;
				case '5':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), sizeof(MYSQL_TIME));
					break;
				case '6':
				case '7':
				case '8':
				case '9':
				case 'a':
				case 'b':
					m_result.WriteStruct(m_outParam[i]->GetValueMemory(), m_outParam[i]->len);
					break;
				case 'c':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i64sz);
					break;
				case 'd':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u64sz);
					break;
				case 'e':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i16sz);
					break;
				case 'f':
					m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u16sz);
					break;
				default:
					m_pMain->SaveLog("FormatResult:mysql out-param flag error!\n");
					row = 0;
					return;
				}
			}
		}
		else
		{
			row = 0;
		}
	}
	else if (m_resFlag & DB_RESULT_MULTI)
	{
		row = (int)mysql_stmt_num_rows(m_hStmt);
		if (row>0)
		{
			int err = 0;
			for (int i=0; i<row; ++i)
			{
				err = mysql_stmt_fetch(m_hStmt);
				if (0!=err)
				{
					row = 0;
					CheckError(6, err);
					break;
				}
				for (u8 i=0; i<m_numOut; ++i)
				{
					switch (m_strOutParam[i])
					{
					case '1':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i32sz);
						break;
					case '2':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u32sz);
						break;
					case '3':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i8sz);
						break;
					case '4':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u8sz);
						break;
					case '5':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), sizeof(MYSQL_TIME));
						break;
					case '6':
					case '7':
					case '8':
					case '9':
					case 'a':
					case 'b':
						m_result.WriteStruct(m_outParam[i]->GetValueMemory(), m_outParam[i]->len);
						break;
					case 'c':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i64sz);
						break;
					case 'd':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u64sz);
						break;
					case 'e':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_i16sz);
						break;
					case 'f':
						m_result.WriteData(m_outParam[i]->GetValueMemory(), cg_u16sz);
						break;
					default:
						m_pMain->SaveLog("FormatResult:mysql out-param flag error!\n");
						row = 0;
						return;
					}
				}
				m_resultSet.Push(m_result.Buffer(), m_result.Size());
				m_result.Reset();
			}
		}
	}
	if (m_resFlag & DB_RESULT_INSERT_ID)
	{
		m_insertID = (u32)mysql_stmt_insert_id(m_hStmt);
	}
}

}
}
