#pragma once
#include "Sock.hpp"
#include <iostream>
#include <jsoncpp/json/json.h>
namespace m_protocol
{
// 报文的分隔符及分隔符长度
#define SEP " "
#define SEP_LENGTH strlen(SEP)
// 整段报文的标志符
#define IDENTIFIER "\r\n"
#define IDEN_LENGTH strlen(IDENTIFIER)
    // 条件编译
    // #define MYSELF 1
    class Request
    {
    public:
        Request() {}
        Request(int x, int y, char op)
            : _x(x), _y(y), _op(op)
        {
        }
        ~Request()
        {
        }
        std::string serializate()
        {
#ifdef MYSELF
            // 将结构化数据序列化
            std::string res = std::to_string(_x);
            res += SEP;
            res += _op;
            res += SEP;
            res += std::to_string(_y);
            return res;
#else
            // 万能对象
            Json::Value root;
            root["x"] = _x;
            root["y"] = _y;
            root["op"] = _op;
            Json::FastWriter writer;
            return writer.write(root);
#endif
        }
        // 进行反序列化, 将序列化数据转为结构化数据
        bool deserializate(const std::string &buffer)
        {
#ifdef MYSELF
            int left = buffer.find(SEP);
            if (left == std::string::npos)
                return false;
            int right = buffer.rfind(SEP);
            if (right == std::string::npos)
                return false;
            if (left + SEP_LENGTH > buffer.size())
                return false;
            _x = atoi(buffer.substr(0, left).c_str());
            _y = atoi(buffer.substr(right + SEP_LENGTH).c_str());
            _op = buffer[left + SEP_LENGTH];
            return true;
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(buffer, root);
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _op = root["op"].asInt();
            return true;

#endif
        }

    public:
        int _x;
        int _y;
        char _op;
    };
    class Response
    {
    public:
        Response() {}
        Response(int code, int result, int x, int y, char op)
            : _code(code), _result(result), _x(x), _y(y), _op(op)
        {
        }
        ~Response()
        {
        }
        std::string serializate()
        {
#ifdef MYSELF
            std::string res = std::to_string(_code);
            res += SEP;
            res += std::to_string(_result);
            return res;
#else
            Json::Value root;
            root["code"] = _code;
            root["result"] = _result;
            root["x"] = _x;
            root["y"] = _y;
            root["op"] = _op;
            Json::FastWriter writer;
            return writer.write(root);
#endif
        }
        bool deserializate(const std::string &buffer)
        {
#ifdef MYSELF
            size_t pos = buffer.find(SEP);
            if (pos == std::string::npos)
                return false;
            _code = atoi(buffer.substr(0, pos).c_str());
            _result = atoi(buffer.substr(pos + SEP_LENGTH).c_str());
            return true;
#else
            Json::Value root;
            Json::Reader reader;
            reader.parse(buffer, root);
            _code = root["code"].asInt();
            _result = root["result"].asInt();
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _op = root["op"].asInt();
            return true;
#endif
        }

    public:
        int _code;
        int _result;
        int _x;
        int _y;
        char _op;
    };

    bool Recv(int sock, std::string *buffer)
    {
        char temp[1024];
        ssize_t s = recv(sock, temp, sizeof(temp) - 1, 0);
        if (s > 0)
        {
            temp[s] = 0;
            *buffer += temp;
        }
        else if (s == 0)
        {
            return false;
        }
        else
        {
            logMessage(ERROR, "recv failed in portocol");
            return false;
        }
        return true;
    }
    size_t Send(int sock, std::string buffer)
    {
        size_t s = send(sock, buffer.c_str(), buffer.size(), 0);
        if (s < 0)
        {
            logMessage(ERROR, "send message error");
        }
    }
    std::string Decode(std::string &buffer)
    {
        // // 将报文解析成一个完整的请求 "length\r\n有效载荷\r\n"
        // size_t left = str.find(IDENTIFIER);
        // if (left == std::string::npos)
        //     return "";
        // // 报文载荷的长度
        // int size = atoi(str.substr(0, left).c_str());
        // // 查看str除去报头和报尾的长度之后剩余是否够一个完整的报文
        // int surplus = str.size() - left - 2 * IDEN_LENGTH;
        // if (surplus >= size)
        // {
        //     // 提取报文载荷
        //     str.erase(0, left + IDEN_LENGTH);
        //     std::string res = str.substr(0, size);
        //     str.erase(0, size + IDEN_LENGTH);
        //     return res;
        // }
        // // 不够则不进行提取,直接返回空串
        // else
        //     return "";
        std::size_t pos = buffer.find(IDENTIFIER);
        if (pos == std::string::npos)
            return "";
        int size = atoi(buffer.substr(0, pos).c_str());
        int surplus = buffer.size() - pos - 2 * IDEN_LENGTH;
        if (surplus >= size)
        {
            // 至少具有一个合法完整的报文, 可以动手提取了
            buffer.erase(0, pos + IDEN_LENGTH);
            // std::cout << "first: " << buffer << "first end." << std::endl;
            std::string s = buffer.substr(0, size);
            buffer.erase(0, size + IDEN_LENGTH);
            // std::cout << "second:" << buffer << "second end." << std::endl;
            return s;
        }
        else
        {
            return "";
        }
    }
    std::string Encode(const std::string &str)
    {
        // 为报文添加报头报尾
        std::string res = std::to_string(str.size());
        res += IDENTIFIER;
        res += str;
        res += IDENTIFIER;
        return res;
    }
}