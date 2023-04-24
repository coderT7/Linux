#pragma once
#include <mutex>
#include <vector>
#include <cassert>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "../comment/log.hpp"
#include "../comment/util.hpp"
#include "../comment/httplib.h"
#include "oj_model.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"
namespace ns_control
{
    using namespace ns_model;
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_view;
    using namespace httplib;
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
            LOG(DEBUG) << "当前在线主机个数：" << online.size() << "\n"; 
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
                }
            }
            machine = &machines[id];
            if (machine)
                LOG(INFO) << "完成智能选择" << machine->ip << ":" << machine->port
                          << "\n";
            mtx->unlock();
            return true;
        }
        void onlineMachine()
        {
        }
        void offlineMachine(int id)
        {
            // 可能同时很多台主机进行离线，需进行枷锁
            mtx->lock();
            // 遍历找到该主机，对应转移下标即可
            for (auto iter = online.begin(); iter != online.end(); iter++)
            {
                if (*iter == id)
                {
                    online.erase(iter);
                    offline.push_back(id);
                    // 直接break，故不需要考虑迭代器失效
                    LOG(DEBUG) << "下线主机：" << id << "\n";
                    break;
                }
            }
            mtx->unlock();
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
        void judge(const std::string &number, const std::string &in_json, std::string &out_json)
        {
            LOG(DEBUG) << "准备进行判题：" << in_json << "\n";
            // 获取对应的题目
            Question question;
            _model.getOneQuestion(number, question);
            // 将请求反序列化获取用户上传的代码等数据
            Json::Value root;
            Json::Reader reader;
            reader.parse(in_json, root);
            std::string code = root["code"].asString();
            std::string input = root["input"].asString();
            int cpu_limit = question._cpu_limit;
            int mem_limit = question._mem_limit;
            Json::Value compile;
            Json::FastWriter writer;
            // 将用户代码拼接测试用例后发给后端编译服务
            compile["code"] = code + question._tail;
            compile["input"] = input;
            compile["cpu_limit"] = cpu_limit;
            compile["mem_limit"] = mem_limit;
            std::string compile_json = writer.write(compile);
            LOG(DEBUG) << "成功获取compile_json" << compile_json << "\n";
            // 负载均衡选择主机进行服务
            // 采取轮询式获取负载均衡最小的主机
            while (true)
            {
                LOG(DEBUG) << "开始进行负载均衡选择"
                           << "\n";
                int id = 0;
                Machine *machine = nullptr;
                // 负载均衡选择负载最小的主机
                if (!_load_blance.smartChoice(id, machine))
                {
                    LOG(DEBUG) << "智能选择失败"
                               << "\n";
                    break;
                }
                LOG(INFO) << "智能选择服务主机成功，主机IP：" << machine->ip << "；主机端口号" << machine->port << "\n";
                // 新建客户端
                Client client(machine->ip, machine->port);
                // 增加负载
                machine->loadIncr();
                // 成功获取响应
                if (auto res = client.Post("/compile_and_run", compile_json, "application/json;charset=utf-8"))
                {
                    // 状态码为200才是我们想要获取的响应
                    if (res->status == 200)
                    {
                        LOG(INFO) << "成功获取响应"
                                  << res->body << "\n";
                        out_json = res->body;
                        break;
                    }
                }
                else
                {
                    LOG(ERROR) << "请求主机id：" << id << "失败"
                               << " ，主机ip：" << machine->ip << "端口号：" << machine->port << "\n";
                    _load_blance.offlineMachine(id);
                }
                machine->loadDecr();
            }
        }
    };
}