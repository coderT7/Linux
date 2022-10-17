#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>

#define NUM 1024
#define SIZE 32
#define SEP " "
char cmd_line[NUM];
char* g_argv[SIZE];
char g_val[64];//保存环境变量
int main()
{
    while(1){
        //打印提示信息
        printf("[root@localhost myshell]# ");
        fflush(stdout);
        memset(cmd_line, '\0', sizeof cmd_line);
        //读取命令行字符串
        if(fgets(cmd_line, sizeof cmd_line, stdin) == NULL){
            continue;
        }
        //解析命令行字符串
        cmd_line[strlen(cmd_line)-1] = '\0';
        g_argv[0] = strtok(cmd_line, SEP);
        int index = 1;

        //让ls命令带配色
        if(strcmp(g_argv[0], "ls") == 0){
            g_argv[index++] = (char*)"--color=auto";
        }
        //分割命令字符串
        while(g_argv[index++] = strtok(NULL, SEP));
        if(strcmp(g_argv[0],"export") == 0 && g_argv[1] != NULL){
            strcpy(g_val,g_argv[1]);
            putenv(g_val);
            continue;
        } 
        //int i = 0;
        //for(i = 0; g_argv[i]; i++){
        //    printf("%d->%s\n",i, g_argv[i]);
        //}
        //处理内置命令,chdir为shell内部自己实现的方法，由自己（父进程）来执行
        if(strcmp(g_argv[0], "cd") == 0){
            if(g_argv[1] != NULL){
                chdir(g_argv[1]);
            }
            continue;
        }
        //执行命令
        pid_t id = fork();
        if(id == 0){
            printf("以下功能为子进程执行\n");
            execvp(g_argv[0],g_argv);
            exit(1);
        }
        int status = 0;
        //等待子进程完成并返回其pid
        pid_t ret = waitpid(id, &status, 0);
        if(ret > 0){
            printf("exit code: %d\n",WEXITSTATUS(status));
        }
    }
    return 0;
}
