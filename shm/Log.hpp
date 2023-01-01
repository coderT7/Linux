//输出对应的日志信息

#include<iostream>
#include<ctime>

#define DEBUG 0
#define NOTICE 1
#define WARNING 2
#define ERROR 3

const std::string LEVEL[]{
    "DEBUG",
    "NOTICE",
    "WARNING",
    "ERROR"};

std::ostream& log(std::string message, int level){
    std::cout << "time:" << (unsigned int)time(nullptr) << "时触发" << message << LEVEL[level];
    return std::cout;
}