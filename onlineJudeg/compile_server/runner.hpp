#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../comment/log.hpp"
#include "../comment/util.hpp"

namespace ns_run
{
    using namespace ns_log;
    using namespace ns_util;
    class Runner
    {
    public:
        // 返回值大于0即为程序运行异常
        // 返回值等于0即正常运行完毕
        // 返回值小于0即为内部错误
        static int run(const std::string &file_name)
        {
            // 打开一个程序运行时默认会打开的三个文件（标准输入，标准输出，标准错误）
            // 我们将原先的0,1,2对应进行重定向，便于我们管理这个程序的运行
            std::string _stdin = PathUtil::stdin_file(file_name);
            std::string _stdout = PathUtil::stdout_file(file_name);
            std::string _stderr = PathUtil::stderr_file(file_name);

            umask(0);
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                LOG(ERROR) << "打开对应的标准输入，输出，错误文件失败"
                           << "\n";
                return -2;
            }
            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "创建子进程失败"
                           << "\n";
                exit(1);
            }
            else if (pid == 0)
            {
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);
                // 进行程序替换，运行我们想运行的程序
                execl(PathUtil::exe(file_name).c_str(), PathUtil::exe(file_name).c_str(), nullptr);
                LOG(ERROR) << "程序替换失败"
                           << "\n";
                // 程序替换如果未成功，直接返回负值表示内部错误即可
                return -1;
            }
            else
            {
                // 父进程等待子进程退出，获取其是否异常退出
                int status = 0;
                waitpid(pid, &status, 0);
                LOG(INFO) << "程序执行完毕，status：" << (status & 0x7F) << "\n";
                return status & 0x7F;
            }
        }
    };
}