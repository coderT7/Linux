#include <ctime>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#include "task.hpp"
#define PROC_NUM 5

using namespace std;

int waitCommand(pid_t pid, int fd, bool &quit)
{
    uint32_t command = 0;
    ssize_t s = read(fd, &command, sizeof(command));
    if (s == 0)
    {
        quit = true;
        cout << "写端进程退出, 读端进程紧随退出" << endl;
        return -1;
    }
    assert(s == sizeof(uint32_t));
    return command;
}
void sendTask(pid_t pid, int fd, int command)
{
    write(fd, &command, sizeof(command));
    cout << "主进程指派子进程: [" << pid << "]执行" << command << "号" << explanation[command] << "任务" << endl;
}
int main()
{
    vector<pair<pid_t, int>> slots;
    load(); //加载任务

    for (int i = 0; i < PROC_NUM; i++)
    {
        int fds[2] = {0};
        //创建管道
        int n = pipe(fds);
        assert(n == 0);
        //创建子进程
        pid_t pid = fork();
        assert(pid >= 0);

        if (pid == 0)
        {
            // child
            close(fds[1]);
            //持续等待任务
            while (true)
            {
                bool quit = false;

                int command = waitCommand(getpid(), fds[0], quit);
                if (quit)
                {
                    break;
                }
                if (command >= 0 && command < taskSize())
                {
                    tasks[command](); //调用任务
                    cout << "子进程: " << getpid() << "执行任务: [" << explanation[command] << "]" << endl;
                }
                else
                {
                    cout << "非法command: " << command << endl;
                }
            }
            exit(0);
        }
        //父进程
        close(fds[0]);
        slots.push_back(make_pair(pid, fds[1]));
        //随机数种子, 以随机方式控制负载均衡
        srand((unsigned long)time(nullptr) ^ pid ^ 123456789L);
    }

    //子进程已创建完毕, 父进程开始指派任务
    while (true)
    {
        // int choice = rand() % taskSize();

        // sendTask(slots[choice].first, slots[choice].second, choice);
        int select;
        int command;
        cout << "############################################" << endl;
        cout << "#   1. show funcitons      2.send command  #" << endl;
        cout << "############################################" << endl;
        cout << "Please Select> ";
        cin >> select;
        if (select == 1)
            showTask();
        else if (select == 2)
        {
            cout << "Enter Your Command> ";
            // 选择任务
            cin >> command;
            // 选择进程
            int choice = rand() % slots.size();
            // 把任务给指定的进程
            sendTask(slots[choice].first, slots[choice].second, command);
        }
        else
        {
            //...
        }
        sleep(1);
    }
    //关闭写端文件描述符
    for (const auto &val : slots)
    {
        close(val.second);
    }
    //等待回收子进程资源
    for (const auto &val : slots)
    {
        waitpid(val.first, nullptr, 0);
    }
    return 0;
}