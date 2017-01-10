#pragma once
#include <functional>
#include <unordered_map>
#include <assert.h>

struct StmtData
{
    StmtData()
    {
        buffer = nullptr;
        length = 0;
    }

    StmtData(void*d, int l)
    {
        buffer = d;
        length = l;
    }

    std::string name;
    void* buffer;
    unsigned long length;
};

struct StmtExData;

struct StmtBindData
{
    StmtBindData(int count)
    {
        m_datas.resize(count);
        m_bd = new MYSQL_BIND[count];

        memset(m_bd, 0, sizeof(MYSQL_BIND)*count);
    }

    void SetInt32(int index, int value)
    {
#ifdef _DEBUG
        assert(index >= 0 && index < (int)m_datas.size());
        assert(m_datas[index].buffer == nullptr);
#endif

        int*p = new int;
        *p = value;

        StmtData temp(p, sizeof(int));
        m_datas[index] = temp;

        m_bd[index].buffer_type = MYSQL_TYPE_LONG;
        m_bd[index].buffer = m_datas[index].buffer;
        m_bd[index].length = &m_datas[index].length;
        m_bd[index].buffer_length = sizeof(int);
        m_bd[index].is_null = (my_bool*)0;
        m_bd[index].is_unsigned = 0;
    }

    void SetInt64(int index, long long value)
    {
#ifdef _DEBUG
        assert(index >= 0 && index < (int)m_datas.size());
        assert(m_datas[index].buffer == nullptr);
#endif
        long long*p = new long long;
        *p = value;

        StmtData temp(p, sizeof(long long));
        m_datas[index] = temp;

        m_bd[index].buffer_type = MYSQL_TYPE_LONGLONG;
        m_bd[index].buffer = m_datas[index].buffer;
        m_bd[index].length = &m_datas[index].length;
        m_bd[index].buffer_length = sizeof(long long);
        m_bd[index].is_null = (my_bool*)0;
        m_bd[index].is_unsigned = 0;
    }

    void SetString(int index, const char* str, int len)
    {
#ifdef _DEBUG
        assert(index >= 0 && index < (int)m_datas.size());
        assert(m_datas[index].buffer == nullptr);
#endif
        char* p = new char[len];

        memcpy(p, str, len);

        StmtData temp(p, len);
        m_datas[index] = temp;

        m_bd[index].buffer_type = MYSQL_TYPE_STRING;
        m_bd[index].buffer = m_datas[index].buffer;
        m_bd[index].length = &m_datas[index].length;
        m_bd[index].buffer_length = m_datas[index].length;
        m_bd[index].is_null = (my_bool*)0;
        m_bd[index].is_unsigned = 0;
    }

    void SetString(int index, const std::string& str)
    {
#ifdef _DEBUG
        assert(index >= 0 && index < (int)m_datas.size());
        assert(m_datas[index].buffer == nullptr);
#endif
        char* p = new char[str.length()];
        memcpy(p, str.c_str(), str.length());

        StmtData temp(p, str.length());
        m_datas[index] = temp;

        m_bd[index].buffer_type = MYSQL_TYPE_STRING;
        m_bd[index].buffer = m_datas[index].buffer;
        m_bd[index].length = &m_datas[index].length;
        m_bd[index].buffer_length = m_datas[index].length;
        m_bd[index].is_null = (my_bool*)0;
        m_bd[index].is_unsigned = 0;
    }

    void SetBlob(int index, const void* str, int len)
    {
#ifdef _DEBUG
        assert(index >= 0 && index < (int)m_datas.size());
        assert(m_datas[index].buffer == nullptr);
#endif
        char* p = new char[len];

        memcpy(p, str, len);

        StmtData temp(p, len);
        m_datas[index] = temp;

        m_bd[index].buffer_type = MYSQL_TYPE_BLOB;
        m_bd[index].buffer = m_datas[index].buffer;
        m_bd[index].length = &m_datas[index].length;
        m_bd[index].buffer_length = m_datas[index].length;
        m_bd[index].is_null = (my_bool*)0;
        m_bd[index].is_unsigned = 0;
    }

    void SetDateTime(int index, int year,int month,int day,int hour,int minute,int second)
    {
        MYSQL_TIME* p = new MYSQL_TIME;
        p->year = year; 
        p->month = month;
        p->day = day;
        p->hour = hour;
        p->minute = minute;
        p->second = second;
        p->second_part = 0;
        p->neg = false;
        p->time_type = MYSQL_TIMESTAMP_DATETIME;
    }

    void free()
    {
        if (m_bd)
        {
            delete[] m_bd;
            m_bd = nullptr;
        }

        for (int i = 0; i < (int)m_datas.size(); i++)
        {
            delete m_datas[i].buffer;
        }

        m_datas.clear();
    }

    MYSQL_BIND* GetBind()
    {
        return m_bd;
    }

    std::vector<StmtData> m_datas;
private:  
    MYSQL_BIND* m_bd;
};

struct StmtResult
{
    void reference_data(std::string& name, void* data, int length)
    {
        StmtData temp(data, length);
        map_datas[name] = temp;
        vec_datas.push_back(temp);
    }

    void clear()
    {
        vec_datas.clear();
        map_datas.clear();
    }

    StmtData* GetData(int index)
    {
        return &vec_datas[index];
    }

    StmtData* GetData(const std::string& name)
    {
        StmtData* pItem = nullptr;
        auto it = map_datas.find(name);
        if (it != map_datas.end())
        {
            pItem = &it->second;
        }
        return pItem;
    }

private:
    std::vector<StmtData> vec_datas;

    std::unordered_map<std::string, StmtData> map_datas;
};

struct Stmt
{
    Stmt()
    {
        stmt = nullptr;
    }

    MYSQL_STMT* stmt;
    StmtBindData* m_Result;

    int m_numIn;
    StmtBindData* GetNewStmtParam()
    {
        StmtBindData* pItem = new StmtBindData(m_numIn);
        
        return pItem;
    }

    void ExecuteQueryVariable(StmtBindData* param, std::function<void(StmtExData*)> cb);

    void update();
    void Execute(StmtExData& row);

    int FormatResult(StmtExData& row);

    std::list<StmtExData> querys;
};

struct StmtExData
{
    void free()
    {
        param->free();
        delete param;

        result.clear();
    }

    bool eof()
    {
        return ret != 0;
    }

    void nextRow()
    {
        ret = stmt->FormatResult(*this);
    }

    void call_back(Stmt* st)
    {
        stmt = st;

        ret = stmt->FormatResult(*this);

        if (cb != nullptr)
        {
            cb(this);
        }
    }

    StmtBindData* param;
    StmtResult result;

    Stmt* stmt;
    int ret;
    std::function<void(StmtExData*)> cb;
};

class MysqlStmt
{
public:
    bool Open(const char* host, const char* user, const char* passwd, const char* db,
        unsigned int port = 3306, const char* charset = "utf8", unsigned long flag = 0);

    void Close();

    bool PrepareQuery(const char* pSql);

    void Update();

    std::vector<Stmt> stmts;
private:

    MYSQL* mysql;
};