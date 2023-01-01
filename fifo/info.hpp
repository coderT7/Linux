#pragma once

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/fcntl.h>
#include<sys/wait.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include"Log.hpp"

using namespace std;

#define MODE 0666
#define SIZE 128
#define NUM 3

const string pathName = "./pipe";