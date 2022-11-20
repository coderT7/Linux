/*
    创建任务, 指派给子进程执行
*/
#include <iostream>
#include<functional>
#include<vector>
#include<map>

typedef std::function<void()> func;

std::map<int, std::string> explanation;
std::vector<func> tasks;

void receiveRequest()
{
    std::cout << "任务执行逻辑: receive request\n" << std::endl;
}
void parseRequest()
{
    std::cout << "任务执行逻辑: parse request\n" << std::endl;
}
void processRequest()
{
    std::cout << "任务执行逻辑: process request\n" << std::endl;
}
void responseRequest(){
    std::cout << "任务执行逻辑: response request\n" << std::endl;
}

void load(){
    explanation[tasks.size()] = "receive request";
    tasks.emplace_back(receiveRequest);

    explanation[tasks.size()] = "parse request";
    tasks.emplace_back(parseRequest);

    explanation[tasks.size()] = "process request";
    tasks.emplace_back(processRequest);

    explanation[tasks.size()] = "response request";
    tasks.emplace_back(responseRequest);
}
void showTask(){
    for(const auto& val : explanation){
        std::cout << "任务" << val.first << ": " << val.second << std::endl;
    }
}
size_t taskSize(){
    return tasks.size();
}
