#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
int main()
{
    printf("MYVAL=%s\n",getenv("MYVAL"));
    return 0;
}
