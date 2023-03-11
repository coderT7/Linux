#pragma once

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
void Deamon()
{
    // 忽略一些特殊信号, 避免异常退出
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    // 避免成为进程组组长
    if (fork() > 0)
        exit(0);
    // 通过接口成为会话
    setsid();
    // 将标准输入,输出,错误进行重定向,防止向显示器输出内容
    int devnull = open("/dev/null", O_RDONLY | O_WRONLY);
    if (devnull > 0)
    {
        dup2(0, devnull);
        dup2(1, devnull);
        dup2(2, devnull);
        close(devnull);
    }
}