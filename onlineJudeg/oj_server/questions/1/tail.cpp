#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    bool ret = Solution().isPalindrome(121);
    if (ret)
    {
        std::cout << "Test1 ok!" << "\n";
    }
    else
    {
        std::cout << "Test1 failed! input: 121, output expected true, actual false"
                  << "\n";
    }
}
void Test2()
{
    bool ret = Solution().isPalindrome(-10);
    if (!ret)
    {
        std::cout << "Test2 ok!"
                  << "\n";
    }
    else
    {
        std::cout << "Test2 failed! input: -10, output expected false, actual true"
                  << "\n";
    }
}
int main()
{
    Test1();
    Test2();
    return 0;
}