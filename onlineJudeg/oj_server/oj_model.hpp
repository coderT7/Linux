#pragma once

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <cassert>
#include <fstream>
#include "../comment/log.hpp"
#include "../comment/util.hpp"
#include "include/mysql.h"

namespace ns_model
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace std;
    class Question
    {
    public:
        // 1 回文数 简单 1 50000
        string _number;
        string _name;
        string _level;
        int _cpu_limit;
        int _mem_limit;
        // 题目描述
        string _desc;
        // 题目预设置代码
        string _header;
        // 测试用例
        string _tail;
    };
    // 数据表名
    const std::string question_table = "oj_question";
    // 连接服务端
    const std::string host = "127.0.0.1";
    // 连接用户
    const std::string user = "oj_client";
    // 登录密码
    const std::string password = "C123456.xt";
    // 访问的数据库
    const std::string db = "oj";
    // 访问的端口
    const int port = 3306;
    // 根据数据文件向外提供资源访问接口
    class Model
    {
    public:
        Model()
        {
        }
        bool queryMySQL(const std::string &sql, vector<Question> &out)
        {
            // 获取MySQL句柄
            MYSQL *mysql = mysql_init(nullptr);
            // 尝试连接数据库
            if (nullptr == mysql_real_connect(mysql, host.c_str(), user.c_str(), password.c_str(), db.c_str(), port, nullptr, 0))
            {
                LOG(FATAL) << "连接数据库失败"
                           << "\n";
                return false;
            }
            // 设置该连接句柄的编码格式（默认连接使用的编码是拉丁）
            mysql_set_character_set(mysql, "utf8");
            // LOG(INFO) << "连接数据库成功"
            //           << "\n";
            // 向数据库发起SQL请求
            if (0 != mysql_query(mysql, sql.c_str()))
            {
                LOG(WARNING) << sql << "执行失败"
                             << "\n";
                return false;
            }
            // 获取查询返回的结果
            MYSQL_RES *res = mysql_store_result(mysql);
            // 解析查询返回的结果
            int rows = mysql_num_rows(res);
            int cols = mysql_num_fields(res);
            // 提取每一行，每一列的数据
            for (int i = 0; i < rows; i++)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                Question question;
                question._number = row[0];
                question._name = row[1];
                question._level = row[2];
                question._desc = row[3];
                question._header = row[4];
                question._tail = row[5];
                question._cpu_limit = atoi(row[6]);
                question._mem_limit = atoi(row[7]);
                // 插入返回列表中
                out.push_back(question);
            }
            // 由于查询结果是用动态申请的空间保存，所以我们需要手动释放
            free(res);
            // 关闭SQL句柄
            mysql_close(mysql);
            return true;
        }
        // 获取所有的题目列表
        bool getAllQuestions(vector<Question> &questions)
        {
            std::string sql = "select * from ";
            sql += question_table;
            return queryMySQL(sql, questions);
        }
        // 获取某一道题
        bool getOneQuestion(const string &number, Question &question)
        {
            std::string sql = "select * from " + question_table + " where ";
            sql += "id=" + number;
            vector<Question> result;
            if (queryMySQL(sql, result))
            {
                // 获取成功
                if (result.size() == 1)
                {
                    question = result[0];
                    return true;
                }
            }
            return false;
        }
        ~Model() {}
    };
}