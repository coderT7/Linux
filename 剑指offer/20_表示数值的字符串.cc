#include <bits/stdc++.h>
using namespace std;
class Solution
{
public:
    bool isNumber(string s)
    {
        vector<unordered_map<char, int>> states = {
            unordered_map<char, int>({{' ', 0}, {'s', 1}, {'d', 2}, {'.', 4}}),
            unordered_map<char, int>({{'d', 2}, {'.', 4}}),
            unordered_map<char, int>({{'d', 2}, {'.', 3}, {'e', 5}, {' ', 8}}),
            unordered_map<char, int>({{'d', 3}, {'e', 5}, {' ', 8}}),
            unordered_map<char, int>({{'d', 3}}),
            unordered_map<char, int>({{'s', 6}, {'d', 7}}),
            unordered_map<char, int>({{'d', 7}}),
            unordered_map<char, int>({{'d', 7}, {' ', 8}}),
            unordered_map<char, int>({{' ', 8}}),
        };
        int p = 0;
        char t;
        for (char c : s)
        {
            if (c >= '0' && c <= '9')
                t = 'd';
            else if (c == '+' || c == '-')
                t = 's';
            else if (c == 'e' || c == 'E')
                t = 'e';
            else if (c == '.' || c == ' ')
                t = c;
            else
                t = '?';
            if (!states[p].count(t))
                return false;
            p = (int)states[p][t];
        }
        return p == 2 || p == 3 || p == 7 || p == 8;
    }
};
/*
### 有限状态自动机
1. 首先需要确定当前问题共有几种状态
2. 再将每种状态下能有几种转换对应画出
3. 转换成代码，记录下每种状态下所能转化成其它的状态，即经过（碰到哪个类型的字符能转化成什么状态）
4. 遍历字符串，从初始状态开始进行变化
5. 人为观察结束状态为什么状态是符合条件，将其作为结果判断值
*/