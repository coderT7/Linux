#include <bits/stdc++.h>
using namespace std;
class Solution
{
public:
    int strToInt(string str)
    {
        if (str.empty())
            return 0;
        int pos = 0;
        // 跳过空格
        while (true)
        {
            if (str[pos] == ' ')
                pos++;
            else
                break;
        }
        // 如果跳过空格后，这个字符既不是正负号，也不是数字则直接返回0
        if (str[pos] != '-' && str[pos] != '+' && !(str[pos] >= '0' && str[pos] <= '9'))
            return 0;
        int sign = 1; // 默认是正数
        if (str[pos] == '-')
        {
            pos++;
            // 负号
            sign = -1;
        }
        else if (str[pos] == '+')
        {
            pos++; // 正号无需处理
        }
        int res = 0;
        for (; pos < str.size(); pos++)
        {
            if (!(str[pos] >= '0' && str[pos] <= '9'))
                break; // 不是数字则直接返回0
            // 走到这说明肯定是数字
            // 注意进行越界判断
            if (res > INT_MAX / 10 || (res == INT_MAX / 10 && str[pos] > '7'))
                return sign == -1 ? INT_MIN : INT_MAX;
            res = res * 10 + (str[pos] - '0');
        }
        return sign * res;
    }
};