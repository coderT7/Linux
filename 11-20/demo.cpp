#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <iostream>
#include <assert.h>
#include <cstring>

using namespace std;

int main()
{
    int fds[2] = {0};
    int count = 0; //构建变化的字符串
    //创建管道
    int n = pipe(fds);
    assert(n == 0);
#ifdef DEBUG
    cout << "fds[0] : " << fds[0] << endl;
    cout << "fds[1] : " << fds[1] << endl;
#endif
    // fds[0] 为读端, fds[1]为写端
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0)
    {
        // child
        close(fds[1]); //关闭写端
        while (true)
        {
            char readBuf[1024] = {0};
            ssize_t s = read(fds[0], readBuf, sizeof(readBuf));
            if (s == 0)
            {
                cout << "写端关闭, 读端退出" << endl;
                break;
            }
            cout << "子进程: " << getpid() << " 接收消息: " << readBuf << endl;
        }
        exit(0);
    }
    // father
    close(fds[0]); //关闭读端
    while (true)
    {
        char sendBuf[1024] = {0};
        //构建变化字符串
        snprintf(sendBuf, sizeof(sendBuf), "%s : %d", "this is father process sending message. ", count++);
        ssize_t s = write(fds[1], sendBuf, strlen(sendBuf));
        sleep(1);
    }
    //等待子进程
    pid_t ret = waitpid(pid, nullptr, 0);
    assert(ret > 0);

    //关闭写端
    close(fds[1]);
    return 0;
}