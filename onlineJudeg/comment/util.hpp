#pragma once
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
namespace ns_util
{
    // 用于路径的拼接的工具
    class PathUtil
    {
    public:
        static std::string add_suffix(const std::string &file_name, const std::string &suffix)
        {
            std::string res = "./temp/";
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
    };
}