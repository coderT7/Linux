#include "Sock.hpp"
#include "Protocol.hpp"
using namespace m_netcal;
using namespace m_protocol;
#include <iostream>
void Usage(char *args[])
{
    std::cout << "Usage: " << args[0] << " ip port" << std::endl;
}
int main(int argc, char *args[])
{
    if (argc != 3)
    {
        Usage(args);
        exit(0);
    }
    std::string server_ip = args[1];
    uint16_t server_port = atoi(args[2]);
    Sock sock;
    int serverSock = sock.Socket();
    if (!sock.Connect(serverSock, server_ip, server_port))
    {
        std::cerr << "Connect error" << std::endl;
        exit(2);
    }
    bool quit = false;
    std::string buffer;

    while (!quit)
    {
        // 获取需求
        Request req;
        std::cout << "Please enter formula#: ";
        std::cin >> req._x >> req._op >> req._y;
        // 序列化
        std::string message = req.serializate();
        // 为报文载荷添加报头
        message = Encode(message);
        // logMessage(DEBUG, "添加报头后的消息: %s", message.c_str());
        // 发送给服务端
        Send(serverSock, message);
        // 读取回复消息
        while (true)
        {
            bool res = Recv(serverSock, &buffer);
            if (!res)
            {
                quit = true;
                break;
            }
            std::string package = Decode(buffer);
            if (package.empty())
                continue;
            Response resp;
            resp.deserializate(package);
            std::string err;
            switch (resp._code)
            {
            case 1:
                err = "除0错误";
                break;
            case 2:
                err = "模0错误";
                break;
            case 3:
                err = "非法操作";
                break;
            default:
                std::cout << resp._x << resp._op << resp._y << " = " << resp._result << " [success]" << std::endl;
                break;
            }
            if (!err.empty())
                std::cerr << err << std::endl;
            // sleep(1);
            break;
        }
    }
    close(serverSock);
    return 0;
}