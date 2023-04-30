#pragma once
// 文件版本数据模块
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <cassert>
#include <fstream>
#include "../comment/log.hpp"
#include "../comment/util.hpp"

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
    const string question_list_path = "./questions/question.list";
    const string question_path = "./questions/";
    // 根据数据文件向外提供资源访问接口
    class Model
    {
    private:
        unordered_map<string, Question> questions_map;

    public:
        Model()
        {
            // 必须加载所有题目成功，否则整个系统无法运行
            assert(loadAllQuestions());
        }
        // 加载所有题目列表至内存中
        bool loadAllQuestions()
        {
            ifstream in(question_list_path);
            if (!in.is_open())
            {
                LOG(FATAL) << "题目列表文件打开失败，请检查题目列表文件"
                           << "\n";
                return false;
            }
            string line;
            while (getline(in, line))
            {
                vector<string> questionV;
                StringUtil::splitString(line, questionV, " ");
                // 我们的题目列表默认有五列（五个项），不足五项则说明该行有问题，我们直接先跳过
                if (questionV.size() != 5)
                {
                    LOG(WARNING) << "出现单个题目获取失败，请检查题目文件"
                                 << "\n";
                    continue;
                }
                Question question;
                // 1 回文数 简单 1 50000
                // 题目的基本信息的填充
                question._number = questionV[0];
                question._name = questionV[1];
                question._level = questionV[2];
                question._cpu_limit = atoi(questionV[3].c_str());
                question._mem_limit = atoi(questionV[4].c_str());
                // 获取题目的具体信息
                string curQuestionPath = question_path + question._number + "/"; // 具体题目文件的路径
                ifstream questionIn(curQuestionPath);
                if (!questionIn.is_open())
                    return false;
                FileUtil::readFile(curQuestionPath + "desc.txt", question._desc, true);
                FileUtil::readFile(curQuestionPath + "header.cpp", question._header, true);
                FileUtil::readFile(curQuestionPath + "tail.cpp", question._tail, true);
                questions_map.insert({question._number, question});
                LOG(DEBUG) << "成功加载题目文件：" << question._number << "\n";
            }
            LOG(DEBUG) << "加载题目列表文件完毕"
                       << "\n";
            in.close();
            return true;
        }
        class Compare
        {
        public:
            bool operator()(Question q1, Question q2)
            {
                return atoi(q1._number.c_str()) < atoi(q2._number.c_str());
            }
        };
        // 获取所有的题目列表
        bool getAllQuestions(vector<Question> &questions)
        {
            if (questions_map.empty())
            {
                LOG(ERROR) << "题目列表为空，请检查"
                           << "\n";
                return false;
            }
            for (auto &val : questions_map)
            {
                questions.push_back(val.second);
            }
            std::sort(questions.begin(), questions.end(), Compare());
            LOG(INFO) << "成功获取所有题目文件"
                      << "\n";
            return true;
        }
        // 获取某一道题
        bool getOneQuestion(const string &number, Question &question)
        {
            auto it = questions_map.find(number);
            if (it == questions_map.end())
            {
                return false;
            }
            question = it->second;
            LOG(INFO) << "成功获取当前题目，题目编号：" << number << "\n";
            return true;
        }
        ~Model() {}
    };
}