#include<iostream>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
using namespace std;

int main()
{
    while(1){
       // malloc(sizeof(int)*1024*1024);
       cout << "pid: "<< getpid() << " hello world" << endl;
//    pid_t id = fork();
//    if(id < 0){
//        cerr << "fork";
//    }else if(id == 0){
//        while(1){
//            cout << "fork return : " << id << endl;
//            cout << "this is child process，pid：" << getpid() << endl;
//            sleep(2);
//        }
//    }else{
//        while(1){
//            cout << "fork return : " << id << endl;
//            cout << "this is parent process, pid：" << getpid() << endl;
//            sleep(2);
//        }
    }
    return 0;
}
