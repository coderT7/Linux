#pragma once
#include <iostream>
#include <unistd.h>
#include <jsoncpp/json/json.h>
#include "../comment/log.hpp"
#include "../comment/util.hpp"
#include "compiler.hpp"
#include "runner.hpp"

namespace ns_compile_run
{
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_compile;
    using namespace ns_run;

    class CompileAndRun
    {
    public:
        static void RemoveTempFile(const std::string &file_name)
        {
            // 清理文件的个数是不确定的，但是有哪些我们是知道的
            std::string _src = PathUtil::src(file_name);
            if (FileUtil::fileIsExist(_src))
                unlink(_src.c_str());

            std::string _compiler_error = PathUtil::compile_error_file(file_name);
            if (FileUtil::fileIsExist(_compiler_error))
                unlink(_compiler_error.c_str());

            std::string _execute = PathUtil::exe(file_name);
            if (FileUtil::fileIsExist(_execute))
                unlink(_execute.c_str());

            std::string _stdin = PathUtil::stdin_file(file_name);
            if (FileUtil::fileIsExist(_stdin))
                unlink(_stdin.c_str());

            std::string _stdout = PathUtil::stdout_file(file_name);
            if (FileUtil::fileIsExist(_stdout))
                unlink(_stdout.c_str());

            std::string _stderr = PathUtil::stderr_file(file_name);
            if (FileUtil::fileIsExist(_stderr))
                unlink(_stderr.c_str());
        }
        static std::string codeToDesc(int code, const std::string &file_name)
        {
            std::string desc;
            switch (code)
            {
            case -1:
                desc = "代码为空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                FileUtil::readFile(PathUtil::compile_error_file(file_name), desc, true);
                break;
            case 0:
                desc = "编译运行完毕!";
                break;
            case SIGABRT:
                desc = "内存超过使用限制";
                break;
            case SIGXCPU:
                desc = "运行时间超出限制";
                break;
            case SIGFPE:
                desc = "浮点数溢出";
                break;
            default:
                desc = "unknow error";
                break;
            }
            return desc;
        }
        // 要从网络上获取到我们的代码，并对应要获取到资源的限制情况
        static void start(const std::string &in_json, std::string *out_json)
        {
            // 先从网络来的代码数据in_json进行反序列化
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value);
            // 将获取到的数据取出来
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();
            // 本次请求的状态码
            int status_code = 0;
            std::string file_name;
            int result_code;
            if (code.empty())
            {
                status_code = -1; // 获取到的code为空
                goto END;
            }
            // 取出来的代码数据需要写入一个具有唯一性的文件名中
            // 获取一个唯一性的文件名 (毫秒级时间戳+原子性递增唯一值: 来保证唯一性)
            file_name = FileUtil::getUniqueFileName();
            // LOG(DEBUG) << "唯一文件形成成功" << file_name << "\n";
            // 将代码写入该文件中
            if (!FileUtil::writeCodeToFile(PathUtil::src(file_name), code))
            {
                LOG(DEBUG) << "代码写入文件失败"
                           << "\n";
                status_code = -2; // 未知错误（不暴露错误细节给用户）
                goto END;
            }
            // 存在了源文件之后我们则可以进行编译和运行
            if (!Compiler::compile(file_name))
            {
                LOG(DEBUG) << "编译错误"
                           << "\n";

                status_code = -3; // 编译错误
                goto END;
            }
            result_code = Runner::run(file_name, cpu_limit, mem_limit);
            if (result_code < 0)
            {
                status_code = -2; // 运行返回值小于0即意味着是内部错误，不暴露错误细节给外部故为未知错误
            }
            else if (result_code > 0)
            {
                status_code = result_code; // 返回值大于0时即意为着程序崩溃，返回值为进程信号
            }
            else
            {
                status_code = 0; // 返回值为0即意味着运行成功
            }
        END:
            // 构建响应字符串
            Json::Value out_value;
            out_value["status"] = status_code;
            // 根据状态码获取原因描述
            out_value["reason"] = codeToDesc(status_code, file_name);
            if (status_code == 0)
            {
                LOG(INFO) << "编译运行成功"
                          << "\n";
                // 状态码为0，则意味着我们的程序运行完毕，我们将输出的内容构建于响应返回
                // 我们编译运行时，我们已经将对应的内容写到了对应的文件中，我们读取文件中的内容即可
                std::string _stdout;
                FileUtil::readFile(PathUtil::stdout_file(file_name), _stdout, true);
                out_value["stdout"] = _stdout;

                std::string _stderr;
                FileUtil::readFile(PathUtil::stderr_file(file_name), _stderr, true);
                out_value["stderr"] = _stderr;
            }
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);
            RemoveTempFile(file_name); // 清空临时文件
        }
    };
}