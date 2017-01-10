#ifndef __MYSQL_PARAM_H__
#define __MYSQL_PARAM_H__

#ifdef FC_USE_WINDOWS
#include <winsock.h>
#else
#include <sys/socket.h>
#include <memory.h>
#endif
#include "./include/mysql.h"
#include "../byteBuffer/fcWriteByteBuffer.h"
#include "../container/fcOrderContainer.h"

#define DB_PARAM_FLAG_NUM		16
#define DB_BUFFER_MAX			512
#define DB_S_BUFFER_LEN			64
#define DB_M_BUFFER_LEN			256
#define DB_L_BUFFER_LEN			510
#define DB_QUERY_RESULE_ERROR	-5		//获取查询结果错误
#define DB_QUERY_LOGIC_ERROR	-4		//逻辑错误
#define DB_QUERY_PARAM_ERROR	-3		//参数错误
#define DB_QUERY_NAME_EXISTS	-2		//名字已存在
#define DB_QUERY_ERROR			-1		//查询出错
#define DB_QUERY_NULL			0		//没有数据

#define DB_RESULT_INSERT_ID		1		//返回插入的新ID
#define DB_RESULT_NULL			2		//不返回记录集
#define DB_RESULT_ONE			4		//返回1条记录集
#define DB_RESULT_MULTI			8		//返回多条记录集

namespace FoldCat
{
namespace fcMySQL
{
//结构体编号：
//QueryInt_s：1
//QueryUInt_s：2
//QueryChar_s：3
//QueryUChar_s：4
//QueryTime_s：5
//QueryName_s：6			64字节
//QuerySBuffer_s：7		64字节
//QueryString_s：8		256字节
//QueryBuffer_s：9		256字节
//QueryText_s：a			510字节
//QueryLBuffer_s：b		510字节
//QueryInt64_s：c
//QueryUInt64_s：d
//QueryShort_s：e
//QueryUShort_s：f
struct QueryParam_s
{
	unsigned long len;	//数据长度
	my_bool isOut;		//是否是输出参数
	//与mysql参数结构体绑定
	virtual void Bind(MYSQL_BIND &bd, char outParam)=0;
	//重置数据
	virtual void Reset()=0;
	//返回数据地址
	virtual void* GetValueMemory()=0;
	//设置参数
	virtual bool SetValue(void* pData, unsigned int nLen)=0;
};
//参数结构定义
//1
struct QueryInt_s : public QueryParam_s
{
	int	val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_LONG;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(int);
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(int*)pData;
		return true;
	}
};
//2
struct QueryUInt_s : public QueryParam_s
{
	unsigned int val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_LONG;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(unsigned int);
		bd.is_null = &isOut;
		bd.is_unsigned = 1;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(unsigned int*)pData;
		return true;
	}
};
//3
struct QueryChar_s : public QueryParam_s
{
	char val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_TINY;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(char);
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(char*)pData;
		return true;
	}
};
//4
struct QueryUChar_s : public QueryParam_s
{
	unsigned char val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_TINY;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(char);
		bd.is_null = &isOut;
		bd.is_unsigned = 1;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(unsigned char*)pData;
		return true;
	}
};
//5
struct QueryTime_s : public QueryParam_s
{
	MYSQL_TIME time;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_DATETIME;
		bd.buffer = &time;
		bd.length = &len;
		bd.buffer_length = sizeof(MYSQL_TIME);
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		memset(&time, 0, sizeof(MYSQL_TIME));
	}
	void* GetValueMemory()
	{
		return &time;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		memcpy(&time, pData, sizeof(MYSQL_TIME));
		return true;
	}
};
//6
struct QueryName_s : public QueryParam_s
{
	char name[DB_S_BUFFER_LEN];

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		len = 0;
		bd.buffer_type = MYSQL_TYPE_STRING;
		bd.buffer = name;
		bd.length = &len;
		bd.buffer_length = DB_S_BUFFER_LEN;
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		len = 0;
		memset(name, 0, DB_S_BUFFER_LEN);
	}
	void* GetValueMemory()
	{
		return name;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		if (nLen==0)
		{
			len = 0;
			return true;
		}
		else if (nLen<DB_S_BUFFER_LEN)
		{
			len = nLen;
			memcpy(name, pData, nLen);
			return true;
		}
		return false;
	}
};
//7
struct QuerySBuffer_s : public QueryParam_s
{
	char sbuf[DB_S_BUFFER_LEN];

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		len = 0;
		bd.buffer_type = MYSQL_TYPE_BLOB;
		bd.buffer = sbuf;
		bd.length = &len;
		bd.buffer_length = DB_S_BUFFER_LEN;
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		len = 0;
	}
	void* GetValueMemory()
	{
		return sbuf;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		if (nLen==0)
		{
			len = 0;
			return true;
		}
		else if (nLen<=DB_S_BUFFER_LEN)
		{
			len = nLen;
			memcpy(sbuf, pData, nLen);
			return true;
		}
		return false;
	}
};
//a
struct QueryText_s : public QueryParam_s
{
	char str[DB_L_BUFFER_LEN];

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		len = 0;
		bd.buffer_type = MYSQL_TYPE_STRING;
		bd.buffer = str;
		bd.length = &len;
		bd.buffer_length = DB_L_BUFFER_LEN;
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		memset(str, 0, DB_L_BUFFER_LEN);
		len = 0;
	}
	void* GetValueMemory()
	{
		return str;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		if (nLen==0)
		{
			len = 0;
			return true;
		}
		else if (nLen<DB_L_BUFFER_LEN)
		{
			len = nLen;
			memcpy(str, pData, nLen);
			return true;
		}
		return false;
	}
};
//b
struct QueryLBuffer_s : public QueryParam_s
{
	char lbuf[DB_L_BUFFER_LEN];

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		len = 0;
		bd.buffer_type = MYSQL_TYPE_BLOB;
		bd.buffer = lbuf;
		bd.length = &len;
		bd.buffer_length = DB_L_BUFFER_LEN;
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		len = 0;
	}
	void* GetValueMemory()
	{
		return lbuf;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		if (nLen==0)
		{
			len = 0;
			return true;
		}
		else if (nLen<=DB_L_BUFFER_LEN)
		{
			len = nLen;
			memcpy(lbuf, pData, nLen);
			return true;
		}
		return false;
	}
};
//8
struct QueryString_s : public QueryParam_s
{
	char str[DB_M_BUFFER_LEN];

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		len = 0;
		bd.buffer_type = MYSQL_TYPE_STRING;
		bd.buffer = str;
		bd.length = &len;
		bd.buffer_length = DB_M_BUFFER_LEN;
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		memset(str, 0, DB_M_BUFFER_LEN);
		len = 0;
	}
	void* GetValueMemory()
	{
		return str;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		if (nLen==0)
		{
			len = 0;
			return true;
		}
		else if (nLen<DB_M_BUFFER_LEN)
		{
			len = nLen;
			memcpy(str, pData, nLen);
			return true;
		}
		return false;
	}
};
//9
struct QueryBuffer_s : public QueryParam_s
{
	char buf[DB_M_BUFFER_LEN];

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		len = 0;
		bd.buffer_type = MYSQL_TYPE_BLOB;
		bd.buffer = buf;
		bd.length = &len;
		bd.buffer_length = DB_M_BUFFER_LEN;
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		len = 0;
	}
	void* GetValueMemory()
	{
		return buf;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		if (nLen==0)
		{
			len = 0;
			return true;
		}
		else if (nLen<=DB_M_BUFFER_LEN)
		{
			len = nLen;
			memcpy(buf, pData, nLen);
			return true;
		}
		return false;
	}
};
//c
struct QueryInt64_s : public QueryParam_s
{
	long long	val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_LONGLONG;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(long long);
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(int*)pData;
		return true;
	}
};
//d
struct QueryUInt64_s : public QueryParam_s
{
	unsigned long long val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_LONGLONG;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(unsigned long long);
		bd.is_null = &isOut;
		bd.is_unsigned = 1;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(unsigned long long*)pData;
		return true;
	}
};
//e
struct QueryShort_s : public QueryParam_s
{
	short val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_SHORT;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(short);
		bd.is_null = &isOut;
		bd.is_unsigned = 0;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(short*)pData;
		return true;
	}
};
//f
struct QueryUShort_s : public QueryParam_s
{
	unsigned short val;

	void Bind(MYSQL_BIND &bd, char outParam)
	{
		isOut = outParam;
		bd.buffer_type = MYSQL_TYPE_SHORT;
		bd.buffer = &val;
		bd.length = &len;
		bd.buffer_length = sizeof(unsigned short);
		bd.is_null = &isOut;
		bd.is_unsigned = 1;
	}
	void Reset()
	{
		val = 0;
	}
	void* GetValueMemory()
	{
		return &val;
	}
	bool SetValue(void* pData, unsigned int nLen)
	{
		val = *(unsigned short*)pData;
		return true;
	}
};

//查询基类
class fcMySQLClient;
class fcMySQLQuery
{
public:
	fcMySQLQuery(void);
	~fcMySQLQuery(void);
	//初始化
	bool Init(fcMySQLClient * pMain, MYSQL * pConn);
	//释放对象
	void Exit();
	//准备查询
	bool PrepareQuery(const char* pIn, const char* pOut, const char* pSql, const u8 resultFlag);
	//执行查询
	int ExecuteQueryVariable(i8 num, ...);
	int ExecuteQueryStream(const u32 len, const char* data);
	//获取错误字符串
	const char* GetLastError() {return mysql_stmt_error(m_hStmt);}
	//获取错误代码
	unsigned int GetLastErrno() {return mysql_stmt_errno(m_hStmt);}
	//获取结果
	fcWriteByteBuffer& GetResult() {return m_result;}
	//获取结果集
	fcOrderContainer& GetResultSet() {return m_resultSet;}
	//获取新插入的ID
	u32 GetInsertID() {return m_insertID;}
	//获取查询类型
	u8 GetResultFlag() {return m_resFlag;}
private:
	//清空结果缓冲区
	void ClearResult();
	//错误检测
	void CheckError(int ret, int fun);
	//清空参数数组
	void ClearParam();
	//获取查询结果并写入到输出缓冲区
	void FormatResult(int& row);
	//执行查询
	int ExecuteSQL(MYSQL_BIND *pin);
	//获取查询结果集
	int GetSQLResult(MYSQL_BIND *pout);


	MYSQL		*		m_pConnect;		//连接对象
	MYSQL_STMT	*		m_hStmt;		//查询对象
	fcMySQLClient*		m_pMain;		//fcMySQLClient类对象
	u8					m_resFlag;		//查询类型
	u8					m_numIn;		//输入参数个数
	u8					m_numOut;		//输出参数个数
	MYSQL_BIND	*		m_in;			//mysql输入参数数组
	MYSQL_BIND	*		m_out;			//mysql输出参数数组
	char		*		m_strInParam;	//输入参数字符串
	char		*		m_strOutParam;	//输出参数字符串
	QueryParam_s**		m_inParam;		//输入参数数组
	QueryParam_s**		m_outParam;		//输出参数数组
	fcWriteByteBuffer	m_result;		//一个结果集
	fcOrderContainer	m_resultSet;	//多个结果集
	u32					m_insertID;		//新插入的ID
	std::string			m_sql;			//sql语句
};

}
}
#endif
