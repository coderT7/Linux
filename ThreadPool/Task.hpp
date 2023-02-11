#pragma once

#include <iostream>
#include <functional>
#include <string>
typedef std::function<int(int, int)> func;

class Task
{
public:
    Task(){};

    void operator()(const std::string &name)
    {
        printf("%s - 任务处理完毕", name.c_str());
    }

    Task(int x, int y, func fun)
        : _x(x), _y(y), _func(fun)
    {
    }

    int getX()
    {
        return _x;
    }
    int getY()
    {
        return _y;
    }

private:
    int _x;
    int _y;
    func _func;
};
