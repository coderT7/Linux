#include "ThreadPool.hpp"
#include "Task.hpp"
#include "Log.hpp"
#include <time.h>
#include <unistd.h>

int main()
{
    // logMessage(DEBUG, "%s", "这是一条测试");
    srand((uint64_t)time(nullptr));
    ThreadPool<Task>* tp = ThreadPool<Task>::getIntance();
    tp->run();
    while (true)
    {
        int x = rand() % 100 + 1;
        int y = rand() % 100 + 1;
        Task task(x, y, [](int x, int y)
                  { return x + y; });
        tp->pushTask(task);
        logMessage(DEBUG, "%d + %d = ? push成功", x, y);
        sleep(1);
    }

    return 0;
}