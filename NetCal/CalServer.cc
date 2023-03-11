#include "TcpServer.hpp"
#include "Log.hpp"
#include "Protocol.hpp"
#include <iostream>
#include <signal.h>
#include "Deamon.hpp"
using namespace m_netcal;
using namespace m_protocol;
// class CalServer
// {
// public:
//     CalServer(uint16_t port, std::string ip = "0.0.0.0")
//         :_server(new TcpServer(port, ip))
//     {}
//     ~CalServer(){}
// private:
//     std::unique_ptr <TcpServer>_server;
// };
void Usage(char *args[])
{
    std::cout << "Usage: " << args[0] << "  port" << std::endl;
}
static Response calculateHelper(const Request &req)
{
    Response res(0, 0, req._x, req._y, req._op);
    res._x = req._x;
    res._y = req._y;
    res._op = req._op;
    switch (req._op)
    {
    case '+':
        res._result = req._x + req._y;
        break;
    case '-':
        res._result = req._x - req._y;
        break;
    case '*':
        res._result = req._x * req._y;
        break;
    case '/':
        if (req._y == 0)
        {
            res._code = 1;
            // std::cout << "除0错误" << std::endl;
            
        }
        else
            res._result = req._x / req._y;
        break;
    case '%':
        if (req._y == 0)
        {
            res._code = 2;
            // std::cout << "模0错误" << std::endl;
        }
        else
            res._result = req._x % req._y;
        break;
    default:
        // std::cout << "非法错误" << std::endl;
        res._code = 3;
        break;
    }
    return res;
}
void calculate(int sock)
{
    // 输入缓冲区
    std::string inBuffer;
    while (true)
    {
        // 接收请求 (序列化的数据)
        bool ret = Recv(sock, &inBuffer);
        if (!ret)
            break;
        logMessage(DEBUG, "begin inBuffer: %s", inBuffer.c_str());

        // 解析报头, 确定获取到了一个完整的请求
        std::string package = Decode(inBuffer);
        // 如果解析到了一个完整的报文则进行反序列化, 否则继续接收
        if (package.empty())
        {
            continue;
        }
        logMessage(DEBUG, "end inBuffer: %s", inBuffer.c_str());
        logMessage(NORMAL, "package: %s", package.c_str());
        Request request;
        request.deserializate(package);

        // 将解析后的请求进行处理, 形成回复报文结构体
        Response resp = calculateHelper(request);
        // 将处理后的结果进行序列化
        std::string message = resp.serializate();
        // 将序列化后的结果添加报头报尾 "length\r\n有效载荷\r\n"
        message = Encode(message);
        // logMessage(DEBUG, "添加报头报尾后的回复序列: %s", message.c_str());
        // 添加报头报尾后进行发送
        Send(sock, message);
    }
    logMessage(NORMAL, "client quit");
    close(sock);
}
int main(int argc, char *args[])
{
    // 忽略客户端退出发出的信号
    signal(SIGPIPE, SIG_IGN);
    if (argc != 2)
    {
        Usage(args);
        exit(0);
    }
    int port = atoi(args[1]);
    Deamon();
    std::unique_ptr<TcpServer> server(new TcpServer(atoi(args[1])));
    server->setServer(calculate);
    server->start();

    return 0;
}