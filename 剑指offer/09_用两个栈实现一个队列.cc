#include <stack>
using namespace std;
class CQueue
{
private:
    stack<int> _in;  // 队列尾部
    stack<int> _out; // 队列首部
public:
    CQueue() {}

    void appendTail(int value)
    {
        // 直接往队列尾部插入即可
        _in.push(value);
    }

    int deleteHead()
    {
        // 当队列首部没元素时，则将队列尾部的给给它
        if (_out.empty())
        {
            // 队列尾部也没元素时则队列为空返回-1
            if (_in.empty())
            {
                return -1;
            }
            else
            {
                // 否则将对尾元素入队首中
                while (!_in.empty())
                {
                    int tmp = _in.top();
                    _in.pop();
                    _out.push(tmp);
                }
            }
        }
        // 队首有元素，直接返回即可
        int res = _out.top();
        _out.pop();
        return res;
    }
};

/**
 * Your CQueue object will be instantiated and called as such:
 * CQueue* obj = new CQueue();
 * obj->appendTail(value);
 * int param_2 = obj->deleteHead();
 */