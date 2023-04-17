#pragma once
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../comment/util.hpp"
#include "../comment/log.hpp"
namespace ns_compile
{
    using namespace ns_util;
    using namespace ns_log;
    class Compiler
    {
    public:
        Compiler() {}
        ~Compiler() {}
        // 根据文件名（无后缀，仅文件名）编译文件
        static bool compile(const std::string &file_name)
        {
            umask(0);
            // 打开对应的错误文件
            int _compile_fd = open(PathUtil::compile_error_file(file_name).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (_compile_fd < 0)
            {
                LOG(WARNING) << "没有成功形成compile_error文件"
                             << "\n";
                exit(2);
            }
            pid_t pid = fork();
            // 因为只有文件名，所以我们要给其添加对应的后缀以供编译
            // 为了规整方便，我们将编译的临时文件放到一个文件夹中
            if (pid < 0)
            {
                // pid < 0则fork失败，对应记录日志信息
                LOG(ERROR) << "fork创建子进程失败，当前编译退出"
                           << "\n";
                return false;
            }
            else if (pid == 0)
            {
                // 将标准错误重定向到错误文件上
                dup2(_compile_fd, 2);
                // 子进程
                int res = execlp("g++", "g++", "-o", PathUtil::exe(file_name).c_str(), PathUtil::src(file_name).c_str(), nullptr);
                if (res < 0)
                {
                    LOG(ERROR) << "启动编译器失败，请检查参数"
                               << "\n";
                    exit(1);
                }
                return false;
            }
            else
            {
                // 父进程
                waitpid(pid, nullptr, 0);
                // 等待到了则判断可执行程序是否生成成功
                bool isExists = FileUtil::fileIsExist(PathUtil::exe(file_name).c_str());
                if (isExists)
                {
                    LOG(INFO) << "生成可执行程序成功"
                              << "\n";
                    return true;
                }
            }
            LOG(ERROR) << "编译失败，未生成可执行程序"
                       << "\n";
            return false;
        }
    };
}