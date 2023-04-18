#include <bits/stdc++.h>
using namespace std;

class Solution
{
public:
    string reverseLeftWords(string s, int n)
    {
        if (s.empty() || s.size() == 1)
        {
            return s;
        }
        n %= s.size();
        reverse(s.begin(), s.begin() + n);
        reverse(s.begin() + n, s.end());
        reverse(s.begin(), s.end());
        return s;
    }
};