#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test01()
{
    vector<int> nums = {1, 7, 2, 0, 9};
    int ret = Solution().find_max(nums);
    if (ret == 9)
    {
        cout << "test1 passed" << endl;
    }
    else
    {
        cout << "test1 failed" << endl;
    }
}
void Test02()
{
    vector<int> nums = {-2, -1, -9, -4};
    int ret = Solution().find_max(nums);
    if (ret == -1)
    {
        cout << "test2 passed" << endl;
    }
    else
    {
        cout << "test2 failed" << endl;
    }
}

int main()
{
    Test01();
    Test02();
    return 0;
}