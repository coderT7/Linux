#include "info.hpp"

// 创建命名管道
Init init;

string transToHex(key_t k)
{
    char buf[128];
    snprintf(buf, sizeof buf, "0x%x", k);
    return buf;
}

int main()
{
    // 获取共享内存的唯一key值
    key_t key = ftok(PATH_NAME, PROJ_ID);
    assert(key >= 0);
    log("key值生成成功: ", DEBUG) << "key: " << transToHex(key) << endl;
    // 创建全新的共享内存
    int shmid = shmget(key, MAX_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    assert(shmid >= 0);
    log("shmid生成成功: ", DEBUG) <<"shmid: " << shmid << endl;
    // 将进程挂接到共享内存上
    char *shmaddr = (char *)shmat(shmid, nullptr, 0);
    log("共享内存挂接成功: ", DEBUG) << endl;
    // 使用共享内存进行通信
    int fd = open(PIPE_NAME, READ);

    while (true)
    {
        // 进行一定的访问控制
        waiting(fd);
        //输出消息
        printf("接收到消息: %s\n", shmaddr);
        //接收到quit则退出循环
        if (strcmp("quit", shmaddr) == 0)
        {
            break;
        }
    }

    // 使用完毕去掉进程与共享内存的挂接关系
    shmdt(shmaddr);
    log("共享内存去挂接成功", DEBUG) << endl;
    // 删除共享内存
    shmctl(shmid, IPC_RMID, nullptr);
    log("删除共享内存成功", DEBUG) << endl;
    closeFd(fd);
    return 0;
}