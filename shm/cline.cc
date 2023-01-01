#include"info.hpp"

int main()
{
    log("child pid is: ", DEBUG) << getpid() << endl;
    // 获取共享内存的唯一key值
    key_t key = ftok(PATH_NAME, PROJ_ID);
    assert(key >= 0);
    log("key值获取成功: ", DEBUG) << endl;
    // 获取共享内存
    int shmid = shmget(key, MAX_SIZE, 0);
    assert(shmid >= 0);
    log("shmid获取成功: ", DEBUG) << "shmid: " <<shmid << endl;
    // 将进程挂接到共享内存上
    char *shmaddr = (char *)shmat(shmid, nullptr, 0);
    log("共享内存挂接成功, 可以开始通信: ", DEBUG) << endl;
    // 使用共享内存进行通信
    int fd = open(PIPE_NAME, WRITE);
    while (true)
    {
        ssize_t s = read(0, shmaddr, MAX_SIZE - 1);
        if(s > 0){
            // 进行一定的访问控制
            shmaddr[s - 1] = '\0'; // 将'\n'去掉
            signal(fd);
            if (strcmp("quit", shmaddr) == 0)
            {
                break;
            }
        }
    }
    // 使用完毕去掉进程与共享内存的挂接关系
    shmdt(shmaddr);
    log("共享内存去挂接成功: ", DEBUG) << endl;
    //关闭管道文件
    closeFd(fd);
    return 0;
}