#include <iostream>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
using namespace std;

int main()
{
    struct rlimit r;
    r.rlim_max = RLIM_INFINITY;
    r.rlim_cur = 2;
    setrlimit(RLIMIT_CPU, &r);
    int count = 0;
    while (true)
    {
        cout << count++ << endl;
        // sleep(1);
    }
    return 0;
}