#include <stack>
using namespace std;
class MinStack
{
private:
    stack<int> _min;
    stack<int> _vals;

public:
    /** initialize your data structure here. */
    MinStack() {}

    void push(int x)
    {
        _vals.push(x);
        if (_min.empty())
            _min.push(x);
        else if (x <= _min.top())
            _min.push(x);
    }

    void pop()
    {
        int tmp = _vals.top();
        _vals.pop();
        if (tmp == _min.top())
            _min.pop();
    }

    int top()
    {
        return _vals.top();
    }

    int min()
    {
        if (!_min.empty())
            return _min.top();
        else
            return -1;
    }
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(x);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->min();
 */