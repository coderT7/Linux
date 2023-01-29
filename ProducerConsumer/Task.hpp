#include<iostream>
#include<functional>

typedef std::function<int(int, int)> func;

class Task{
public:
    Task(){};

    int operator()(){
        return _func(_x, _y);
    }

    Task(int x, int y, func fun)
        :_x(x), _y(y), _func(fun)
    {}

    int getX(){
        return _x;
    }
    int getY(){
        return _y;
    }

private:
    int _x;
    int _y;
    func _func;
};
