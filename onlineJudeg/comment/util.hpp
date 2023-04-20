#pragma once
#include <string>
#include <atomic>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
namespace ns_util
{
    const static std::string temp_path = "./temp/";
    class TimeUtil
    {
    public:
        static std::string getCurTime()
        {
            // 获取当前时间
            time_t t;
            time(&t);
            char *curTime = ctime(&t);
            size_t len = strlen(curTime);
            curTime[len - 1] = '\0';
            return curTime;
        }
        static std::string getTimeofMS()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);
            // 毫秒数
            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    };
    // 用于路径的拼接的工具
    class PathUtil
    {
    public:
        static std::string add_suffix(const std::string &file_name, const std::string &suffix)
        {
            std::string res = temp_path;
            res += file_name;
            res += suffix;
            return res;
        }
        // 编译时需要的文件
        // 添加源文件的后缀
        static std::string src(const std::string &file_name)
        {
            return add_suffix(file_name, ".cc");
        }
        // 添加可执行文件的后缀
        static std::string exe(const std::string &file_name)
        {
            return add_suffix(file_name, ".exe");
        }
        // 添加编译时错误文件的后缀
        static std::string compile_error_file(const std::string &file_name)
        {
            return add_suffix(file_name, ".compile_error");
        }
        // 运行时需要的文件
        // 添加标准输入文件的后缀
        static std::string stdin_file(const std::string &file_name)
        {
            return add_suffix(file_name, ".stdin");
        } // 添加标准输出文件的后缀
        static std::string stdout_file(const std::string &file_name)
        {
            return add_suffix(file_name, ".stdout");
        }
        // 添加运行时错误文件的后缀
        static std::string stderr_file(const std::string &file_name)
        {
            return add_suffix(file_name, ".stderr");
        }
    };
    class FileUtil
    {
    public:
        static bool fileIsExist(const std::string path_name)
        {
            struct stat buf;
            int ret = stat(path_name.c_str(), &buf);
            if (ret == 0)
                return true;
            else
                return false;
        }
        static std::string getUniqueFileName()
        {
            // 获取一个具备原子性的整数
            static std::atomic_uint id(0);
            id++;
            std::string ms = TimeUtil::getTimeofMS();
            std::string unique_id = to_string(id);
            return ms + "_" + unique_id;
        }
        static bool writeCodeToFile(const std::string path_name, const std::string &content)
        {
            std::ofstream out(path_name);
            if(!out.is_open())
                return false;
            out.write(content.c_str(), content.size());
            out.close();
            return true;
        }

        static bool readFile(const std::string path_name, std::string& content, bool keep = false)
        {
            content.clear();
            std::ifstream in(path_name);
            if(!in.is_open())
                return false;
            std::string line;
            // getline默认会去掉\n，所以我们要看情况选择是否要保留\n
            while(std::getline(in, line))
            {
                content += line;
                content += (keep ? "\n" : "");
            }
            in.close();
            return true;
        }
    };
}