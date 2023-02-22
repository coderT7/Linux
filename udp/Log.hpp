#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define DEBUG   0
#define NORMAL  1
#define WARNING 2
#define ERROR   3
#define FATAL   4

const char* levalToStr(int level){
    const char *levels[] = {
        "DEBUG",
        "NORMAL",
        "WARNING",
        "ERROR",
        "FATAL"};
    return levels[level];
}

void logMessage(int level, const char* format, ...){
    // 日志文件
    // const char *filename = "threadPoolLog.log";
    // FILE *fp = fopen(filename, "a");
    // 标准日志缓冲区
    char stdBuffer[1024];
    // 日志等级, 日期等信息
    const time_t timeStamp = time(nullptr);
    struct tm *timeStruct = localtime(&timeStamp);
    snprintf(stdBuffer, sizeof(stdBuffer), "[%s] [%d-%d-%d] ", levalToStr(level), 
    timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);
    // 用户自定义信息缓冲区
    char userBuffer[1024];
    // 本质就是一个 char*
    va_list va;
    // 将指针指向参数列表的参数
    va_start(va, format);
    // 讲可变参数列表对应输出
    vsnprintf(userBuffer, sizeof(userBuffer), format, va);
    // 将va置空
    va_end(va);
    // fprintf(fp, "%s - %s\n", stdBuffer, userBuffer);
    printf("%s %s\n", stdBuffer, userBuffer);
}