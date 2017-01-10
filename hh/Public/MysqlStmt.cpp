#pragma once
#include <unordered_map>
#ifdef _WIN64
#include "mysql/mysql64/include/mysql.h"
#include "mysql/mysql64/include/mysql_com.h"
#else
#include "mysql/mysql32/include/mysql.h"
#include "mysql/mysql32/include/mysql_com.h"
#endif 

#include "MysqlStmt.h"
#include <stdarg.h>
#include <iostream>

#include <windows.h>

bool MysqlStmt::Open(const char* host, const char* user, const char* passwd, const char* db,
    unsigned int port /* = 3306 */, const char* charset /* = "utf8" */, unsigned long flag /* = 0 */)
{
    mysql = mysql_init(NULL);
    if (nullptr == mysql)
        return false;

    //支持中文处理
    mysql_options(mysql, MYSQL_SET_CHARSET_NAME, charset);

    ///如果断线则重新在连接
    mysql_options(mysql, MYSQL_OPT_RECONNECT, "1");

    if (mysql_real_connect(mysql, host, user, passwd, db, port, nullptr, flag) == nullptr)
        return false;

    //选择制定的数据库失败
    //0表示成功，非0值表示出现错误。
    if (mysql_select_db(mysql, db) != 0)
    {
        mysql_close(mysql);
        mysql = NULL;
        return false;
    }

    return true;
}

void MysqlStmt::Close()
{
    for (int i = 0; i < (int)stmts.size(); i++)
    {
        stmts[i].m_Result->free();
        delete stmts[i].m_Result;

        auto it = stmts[i].querys.begin();
        for (; it != stmts[i].querys.end(); ++it)
        {
            it->free();
        }
        stmts[i].querys.clear();

        mysql_stmt_close(stmts[i].stmt);
    }
    stmts.clear();

    if (mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
}

bool MysqlStmt::PrepareQuery(const char* pSql)
{
    Stmt temp;

    const char* m_sql = pSql;

    temp.stmt = mysql_stmt_init(mysql);
    //my_bool true_value = 1;
    //mysql_stmt_attr_set(temp.stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void*)&true_value);

    int rt = mysql_stmt_prepare(temp.stmt, pSql, (unsigned long)strlen(pSql));
    if (0 != rt)
    {
        return false;
    }

    temp.m_numIn = mysql_stmt_param_count(temp.stmt);

    MYSQL_RES* ret = mysql_stmt_result_metadata(temp.stmt);
    if (ret)
    {
        int count = mysql_num_fields(ret);

        //输出参数
        if (count > 0)
        {
            temp.m_Result = new StmtBindData(count);

            for (int i = 0; i < (int)temp.stmt->field_count; i++)
            {
                switch (temp.stmt->fields[i].type)
                {
                case MYSQL_TYPE_LONG:
                    temp.m_Result->SetInt32(i, 0);
                    break;
                case MYSQL_TYPE_DATETIME:
                    temp.m_Result->SetDateTime(i,0, 0, 0, 0, 0, 0);
                    break;
                case MYSQL_TYPE_STRING:
                    temp.m_Result->SetString(i, std::string(temp.stmt->fields[i].length, 0));
                    break;
                case MYSQL_TYPE_BLOB:
                {
                    std::string str(temp.stmt->fields[i].length, 0);
                    temp.m_Result->SetBlob(i, str.c_str(), str.length()); 
                }
                    break;
                case MYSQL_TYPE_LONGLONG:
                    temp.m_Result->SetInt64(i, 0);
                    break;
                default:
                    return false;
                }

                temp.m_Result->m_datas[i].name = temp.stmt->fields[i].name;
            }
        }
    }

    stmts.push_back(temp);
    return true;
}

void MysqlStmt::Update()
{
    for (int i = 0; i < (int)stmts.size(); i++)
    {
        stmts[i].update();
    }
}


void Stmt::ExecuteQueryVariable(StmtBindData* param, std::function<void(StmtExData*)> cb)
{
    StmtExData temp;

    if (m_numIn > 0)
    {
        temp.param = param;
    }

    temp.cb = cb;

    querys.push_back(temp);
}

void Stmt::update()
{
    if (!querys.empty())
    {
        StmtExData temp = querys.front();
        querys.pop_front();

        Execute(temp);
    }
}

void Stmt::Execute(StmtExData& row)
{
    int ret = mysql_stmt_bind_param(stmt, row.param->GetBind());
    if (ret != 0)
    {
        return row.free();
    }

    ret = mysql_stmt_execute(stmt);

    if (ret != 0)
    {
        std::cout << mysql_stmt_error(stmt);
        return row.free();
    }

    if (m_Result->m_datas.size() > 0)
    {   
        ret = mysql_stmt_bind_result(stmt, m_Result->GetBind());

        if (ret != 0)
        {
            std::cout << mysql_stmt_error(stmt);
            return row.free();
        }

        ret = mysql_stmt_store_result(stmt);

        if (ret == 0)
        {
            row.call_back(this);
        }

        mysql_stmt_free_result(stmt);
    }

    return row.free();
}

int Stmt::FormatResult(StmtExData& data)
{
    int ret = mysql_stmt_fetch(stmt);
    if (0 != ret)
    {
        return ret;
    }

    StmtResult& temp = data.result;
    temp.clear();

    for (int i = 0; i < (int)m_Result->m_datas.size(); ++i)
    {
        temp.reference_data(m_Result->m_datas[i].name, m_Result->m_datas[i].buffer, m_Result->m_datas[i].length);
    }

    return ret;
}