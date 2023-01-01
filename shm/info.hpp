#pragma once

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<assert.h>
#include"Log.hpp"

using namespace std;

#define PATH_NAME "./"
#define PROJ_ID 066
#define MAX_SIZE 4096

#define PIPE_NAME "./pipe"
#define READ O_RDONLY
#define WRITE O_WRONLY

class Init
{
public:
    Init(){
        //创建命名管道
        int ret = mkfifo(PIPE_NAME, 0666);
        assert(ret != -1);
        log("创建命名管道成功: ", NOTICE) << endl;
    }
    ~Init(){
        //删除命名管道
        int ret = unlink(PIPE_NAME);
        assert(ret != -1);
        log("删除命名管道成功: ", NOTICE) << endl;
    }
};
//等待cline写入数据
void waiting(int fd){
    log("正在等待中...: ", NOTICE) << endl;
    uint32_t temp = 0;
    //等待"信号发送"后读取解除阻塞
    ssize_t ret = read(fd, &temp, sizeof(uint32_t));
    assert(ret == sizeof(uint32_t));
}
//cline写入数据完毕发送信号
void signal(int fd){
    uint32_t temp = 1;
    ssize_t ret = write(fd, &temp, sizeof(uint32_t));
    assert(ret == sizeof(uint32_t));
    log("唤醒服务端...: ", NOTICE) << endl;
}
void closeFd(int fd){
    close(fd);
    log("关闭管道文件: ", NOTICE) << endl;
}
