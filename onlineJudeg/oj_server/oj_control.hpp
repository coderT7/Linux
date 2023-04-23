#pragma once
#include <mutex>
#include <vector>
#include <cassert>
#include <fstream>
#include "../comment/log.hpp"
#include "../comment/util.hpp"
#include "oj_model.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"
namespace ns_control
{
    using namespace ns_model;
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_view;

    class Machine
    {
    public:
        std::string ip;
        int port;
        uint64_t load;
        std::mutex *mtx; // c++的mutex默认禁止拷贝，故使用指针
    public:
        Machine()
            : ip(""), port(0), load(0), mtx(nullptr)
        {
        }
        ~Machine() {}

    public:
        void loadIncr()
        {
            // 被调用时，当前机器一定已经进行了初始化（绑定了ip，端口号和互斥锁）
            if (mtx)
                mtx->lock();
            load++;
            if (mtx)
                mtx->unlock();
        }
        void loadDecr()
        {
            if (mtx)
                mtx->lock();
            load--;
            if (mtx)
                mtx->unlock();
        }
        // 统一load的操作
        uint64_t getLoad()
        {
            uint64_t _load;
            if (mtx)
                mtx->lock();
            _load = load;
            if (mtx)
                mtx->unlock();
            return _load;
        }
    };
    const std::string machine_conf = "./config/server_machine.conf";
    class LoadBlance
    {
    public:
        std::vector<Machine> machines;
        // 在线机器与离线机器，以机器下标作为标识
        std::vector<int> online;
        std::vector<int> offline;
        // 保证主机选择的安全
        std::mutex *mtx;

    public:
        LoadBlance()
            : mtx(new std::mutex())
        {
            assert(LoadMachine());
        }
        ~LoadBlance()
        {
            if (mtx)
            {
                delete mtx;
            }
        }

    public:
        bool LoadMachine()
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG("FATAL") << "服务机器配置文件打开失败，请检查配置文件"
                             << "\n";
                return false;
            }
            // 打开成功
            std::vector<std::string> data;
            std::string line;
            while (std::getline(in, line))
            {
                StringUtil::splitString(line, data, ":");
                if (data.size() != 2)
                {
                    LOG(WARNING) << "读取机器配置信息出错，请检查配置文件该行：" << line << "\n";
                    continue;
                }
                Machine machine;
                machine.ip = data[0];
                machine.port = atoi(data[1].c_str());
                machine.load = 0;
                machine.mtx = new mutex();
                // 将对应机器的下标作为在线机器的标识插入
                online.push_back(machines.size());
                machines.push_back(machine);
            }
            LOG(INFO) << "加载机器配置成功"
                      << "\n";
            return true;
        }
        bool smartChoice(int &id, Machine *&machine)
        {
            mtx->lock();
            // 没有在线机器则返回false
            if (online.size() == 0)
            {
                machine = nullptr;
                return false;
            }
            id = online[0];
            // 有在在线机器，遍历找到负载最低的那台机器
            uint64_t min_load = machines[online[0]].getLoad();
            for (int i = 0; i < online.size(); i++)
            {
                uint64_t cur_load = machines[online[i]].getLoad();
                if (cur_load < min_load)
                {
                    min_load = cur_load;
                    id = online[i];
                    machine = &machines[online[i]];
                }
            }
            mtx->unlock();
            return true;
        }
        void onlineMachine()
        {
        }
        void offlineMachine()
        {
        }
    };

    class Control
    {
    private:
        // 题目数据信息
        Model _model;
        // 将数据进行可视化
        View _view;
        // 负载均衡模块
        LoadBlance _load_blance;

    public:
        Control() {}
        ~Control() {}
        bool getAllQuestions(std::string &html)
        {
            // 获取到所有题目
            vector<Question> questions;
            _model.getAllQuestions(questions);
            if (questions.empty())
            {
                // 出错
                LOG(FATAL) << "获取题目列表失败，请检查题目列表文件"
                           << "\n";
                return false;
            }
            else
            {
                // 构建html网页返回
                _view.expandAllQuestions(questions, &html);
                return true;
            }
        }
        bool getOneOfQuestions(const std::string &number, std::string &html)
        {
            Question q;
            if (!_model.getOneQuestion(number, q))
                return false;
            // 获取到题目后构建html返回
            _view.expandOneQuestion(q, &html);
            return true;
        }
    };
}