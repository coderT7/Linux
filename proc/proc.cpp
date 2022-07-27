#include<stdio.h>
#include<string.h>
#include<unistd.h>
#define NUM 101
int main()
{
    const char* label = "/|-\\";
    char proc[NUM];
    memset(proc,0,sizeof(proc));
    int cnt = 0;
    while(cnt <= 100){
        printf("[%-100s][%d%%] %c )\r",proc,cnt,label[cnt%4]);
        proc[cnt++] = '#';
        fflush(stdout);
        usleep(30000);
    }
    printf("\n");
    return 0;
}
