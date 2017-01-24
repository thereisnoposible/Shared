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
#include "Helper.h"
#include <stdarg.h>
#include <iostream>

#include <windows.h>

#include <boost/regex.hpp>

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
    auto it = stmts.begin();
    for (; it!=stmts.end(); ++it)
    {
        if (it->second.m_Result)
        {
            it->second.m_Result->free();
            delete it->second.m_Result;
            it->second.m_Result = nullptr;
        }

        auto itt = it->second.querys.begin();
        for (; itt != it->second.querys.end(); ++it)
        {
            itt->free();
        }
        it->second.querys.clear();

        mysql_stmt_close(it->second.stmt);
    }
    stmts.clear();

    if (mysql)
    {
        mysql_close(mysql);
        mysql = NULL;
    }
}

StmtBindData* MysqlStmt::PrepareQuery(const char* pSql)
{
    Stmt temp(100);

    std::string m_sql(pSql);

    auto sqit = stmts.find(m_sql);
    if (sqit != stmts.end())
    {
        StmtBindData* pItem = sqit->second.GetNewStmtParam();
        return pItem;
    }

    std::string temp_sql = AnalyzeSql(&temp, m_sql);

    temp.stmt = mysql_stmt_init(mysql);
    //my_bool true_value = 1;
    //mysql_stmt_attr_set(temp.stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void*)&true_value);

    int rt = mysql_stmt_prepare(temp.stmt, temp_sql.c_str(), temp_sql.size());
    if (0 != rt)
    {
        mysql_stmt_close(temp.stmt);
        return NULL;
    }

    temp.m_numIn = mysql_stmt_param_count(temp.stmt);

    MYSQL_RES* ret = mysql_stmt_result_metadata(temp.stmt);
    if (ret)
    {
        int count = mysql_num_fields(ret);
        temp.m_Result = nullptr;
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
                case MYSQL_TYPE_VAR_STRING:
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
                    std::cout << "unknown type" << temp.stmt->fields[i].type << "\n";
                    mysql_stmt_close(temp.stmt);
                    return NULL;
                }

                temp.m_Result->m_datas[i].name = temp.stmt->fields[i].name;
            }
        }
    }

    stmts.insert(std::make_pair(m_sql, temp));
    std::unordered_map<std::string, Stmt>::iterator iooo = stmts.find(m_sql);
    return iooo->second.GetNewStmtParam();
}

void MysqlStmt::AsynExecuteQueryVariable(std::string& sql, StmtBindData* param, std::function<void(StmtExData*)> cb)
{
    auto sqit = stmts.find(sql);
    if (sqit == stmts.end())
    {
        PrepareQuery(sql.c_str());
        sqit = stmts.find(sql);
    }

    sqit->second.AsynExecuteQueryVariable(param, cb);

}
void MysqlStmt::SyncExecuteQueryVariable(std::string& sql, StmtBindData* param, StmtSyncResult& res)
{
    auto sqit = stmts.find(sql);
    if (sqit == stmts.end())
    {
        PrepareQuery(sql.c_str());
        sqit = stmts.find(sql);
    }

    sqit->second.SyncExecuteQueryVariable(param, res);
}

std::string MysqlStmt::AnalyzeSql(Stmt* stmt, std::string& sql)
{
    std::string temp_sql = sql;

    const char* str = temp_sql.c_str();
    int pos = Helper::FindString(str, temp_sql.size(), "?");

    std::vector<std::string> finder;
    finder.push_back(",");
    std::string tttt;
    tttt.assign(1, '\0');
    finder.push_back(tttt);
    finder.push_back(" ");
    finder.push_back(")");

    int index = -1;
    while (pos != -1)
    {
        index++;
        int s_pos = Helper::FindString(str + pos + 1, strlen(str + pos + 1) + 1, finder);

        if (s_pos != -1)
        {
            std::string ss;
            ss.assign(str + pos + 1, s_pos);

            if (ss.empty())
                abort();

            temp_sql.erase(pos + 1, s_pos);
            stmt->m_Index[ss].push_back(index);
        }

        int t_pos = Helper::FindString(str + pos + 1, strlen(str + pos + 1), "?");
        if (t_pos == -1)
            break;
        pos += t_pos + 1;
    }

    return temp_sql;
}

bool MysqlStmt::Update()
{
    bool bhang = true;

    auto it = stmts.begin();
    for (; it != stmts.end(); ++it)
    {
        if (it->second.update())
        {
            bhang = false;
        }
    }

    return bhang;
}


void Stmt::AsynExecuteQueryVariable(StmtBindData* param, std::function<void(StmtExData*)> cb)
{
    StmtExData temp;

    temp.param = param;

    temp.cb = cb;

    CircularQuerys[in_pos].push_back(temp);

    int next = in_pos + 1;
    if (next == CircularQuerys.size())
        next = 0;

    if (CircularQuerys[next].size() == 0)
        in_pos = next;

    //querys.push_back(temp);
}

void Stmt::SyncExecuteQueryVariable(StmtBindData* param, StmtSyncResult& res)
{
    if (m_numIn > 0)
    {
        res.pResult.param = param;
    }
    
    if (m_Result != nullptr)
    {
        res.res_param = new StmtBindData(0);
        res.res_param->CopyFrom(*m_Result);
    }  

    int ret = 0;
    if (res.pResult.param)
        ret = mysql_stmt_bind_param(stmt, res.pResult.param->GetBind());
    if (ret != 0)
    {
        res.pResult.result.bResult = false;
        return;
    }

    ret = mysql_stmt_execute(stmt);

    if (ret != 0)
    {
        std::cout << mysql_stmt_error(stmt);
        res.pResult.result.bResult = false;
        return;
    }

    if (res.res_param->m_datas.size() > 0)
    {
        ret = mysql_stmt_bind_result(stmt, res.res_param->GetBind());

        if (ret != 0)
        {
            std::cout << mysql_stmt_error(stmt);
            res.pResult.result.bResult = false;
            return;
        }

        ret = mysql_stmt_store_result(stmt);

        if (ret != 0)
        {
            return;
            
        }

        res.pResult.call_back(this, true, res.res_param);
    }

    res.pResult.result.bResult = true;
}

bool Stmt::update()
{
    if (CircularQuerys[out_pos].size() == 0)
    {
        return false;
    }

    std::vector<StmtExData>& temp = CircularQuerys[out_pos];
    for (int i = 0; i < (int)temp.size(); i++)
    {
        Execute(temp[i], m_Result);
        temp[i].free();
    }
    temp.clear();
    out_pos++;
    if (out_pos == CircularQuerys.size())
        out_pos = 0;  

    return true;
}

void Stmt::Execute(StmtExData& row, StmtBindData* res)
{
    int ret = 0;
    if (row.param)
        ret = mysql_stmt_bind_param(stmt, row.param->GetBind());
    if (ret != 0)
    {
        row.call_back(this, false, nullptr);
        return;
    }

    ret = mysql_stmt_execute(stmt);

    if (ret != 0)
    {
        row.call_back(this, false, nullptr);

        std::cout << mysql_stmt_error(stmt);
        return;
    }

    if (res && res->m_datas.size() > 0)
    {   
        ret = mysql_stmt_bind_result(stmt, res->GetBind());

        if (ret != 0)
        {
            row.call_back(this, false, nullptr);
            std::cout << mysql_stmt_error(stmt);
            return;
        }

        ret = mysql_stmt_store_result(stmt);

        if (ret == 0)
        {
            row.call_back(this, true, res);
        }

        mysql_stmt_free_result(stmt);
    }

    return;
}

int Stmt::FormatResult(StmtExData& data, StmtBindData* result)
{
    int ret = mysql_stmt_fetch(stmt);
    if (0 != ret)
    {
        return ret;
    }

    StmtResult& temp = data.result;
    temp.clear();

    for (int i = 0; i < (int)result->m_datas.size(); ++i)
    {
        temp.reference_data(result->m_datas[i].name, result->m_datas[i].buffer, result->m_datas[i].length);
    }

    return ret;
}