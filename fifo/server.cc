#include "info.hpp"

void getMessage(int fd)
{
    char buf[SIZE];
    while (true)
    {
        memset(buf, '\0', SIZE);
        ssize_t ret = read(fd, buf, SIZE);
        if (ret == 0)
        {
            cerr << "写端关闭, 读端即将关闭" << endl;
            break;
        }
        else
        {
            cout << getpid() << "server get a message:" << buf << endl;
        }
    }
}

int main()
{
    // 1. 创建管道文件
    int ret = mkfifo(pathName.c_str(), MODE);
    // 创建管道文件失败则退出程序并抛出错误
    if (ret == -1)
    {
        perror("mkfifo");
        exit(1);
    }
    log("创建管道文件", DEBUG);
    // 2. 打开管道文件
    int fd = open(pathName.c_str(), O_RDONLY);
    log("打开管道文件", DEBUG);
    if (fd == -1)
    {
        perror("server open");
        exit(2);
    }
    // 创建子进程接收消息
    for (int i = 0; i < NUM; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(3);
        }
        // 3. (子进程)正常进行通信
        if (pid == 0)
        {
            getMessage(fd);
        }
    }
    // 父进程等待子进程
    for (int i = 0; i < NUM; i++)
    {
        waitpid(-1, nullptr, 0);
    }
    // 4. 关闭管道文件
    close(fd);
    unlink(pathName.c_str());
    log("关闭管道文件", DEBUG);
    return 0;
}