#include <bits/stdc++.h>
using namespace std;

class Solution
{
public:
    string replaceSpace(string s)
    {
        string res;
        for (const auto &val : s)
        {
            if (!(val == ' '))
            {
                res += val;
            }
            else
            {
                res += "%20";
            }
        }
        return res;
    }
};