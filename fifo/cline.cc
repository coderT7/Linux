#include"info.hpp"

int main()
{
    //打开管道文件
    int fd = open(pathName.c_str(), O_WRONLY);
    if(fd == -1){
        perror("cline open");
        exit(1);
    }
    //正常进行通信
    string buf;
    while (true)
    {
        cout << "please enter your message: ";
        getline(cin, buf);
        write(fd, buf.c_str(), buf.size());
    }
    //关闭管道文件描述符
    close(fd);
    return 0;
}