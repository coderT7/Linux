#include <bits/stdc++.h>
using namespace std;

/**
 * Definition for singly-linked list.

 */
struct ListNode
{
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};
class Solution
{
public:
    vector<int> reversePrint(ListNode *head)
    {
        if (head == nullptr)
            return vector<int>();
        ListNode *cur = head;
        vector<int> res;
        while (cur)
        {
            res.push_back(cur->val);
            cur = cur->next;
        }
        reverse(res.begin(), res.end());
        return res;
    }
};