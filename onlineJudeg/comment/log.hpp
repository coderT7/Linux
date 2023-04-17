#pragma once

#include <iostream>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <cstring>
#include "util.hpp"
namespace ns_log
{
    using namespace ns_util;
    enum LEVEL
    {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    inline std::ostream &log(const std::string& level, const std::string &file_name, int line)
    {
        // 获取当前时间
        time_t t;
        time(&t);
        char *curTime = ctime(&t);
        size_t len = strlen(curTime);
        curTime[len - 1] = '\0';
        std::string res = "[";
        res += level;
        res += "]";

        res += "[";
        res += file_name;
        res += "]";

        res += "[";
        res += std::to_string(line);
        res += "]";

        res += "[";
        res += curTime;
        res += "]";

        std::cout << res;
        return std::cout;
    }
#define LOG(LEVEL) log(#LEVEL, __FILE__, __LINE__)

}