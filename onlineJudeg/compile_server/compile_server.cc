#include "compile_run.hpp"
#include "../comment/httplib.h"
#include "Daemon.hpp"
using namespace ns_compile_run;
using namespace httplib;
void Usage(char argv[])
{
        std::cout << argv << " port" << std::endl;
}

// 编译服务随时可能被多个人请求，必须保证传递上来的code，形成源文件名称的时候，要具有
// 唯一性，要不然多个用户之间会互相影响
int main(int argc, char *argv[])
{
        Daemon();
        if (argc != 2)
        {
                Usage(argv[0]);
                exit(1);
        }
        Server svr;
        // svr.Get("/hello", [](const Request &req, Response &res)
        //         { res.set_content("hello httplib, 这是个服务器响应", "text/plain;charset=utf-8"); });
        // svr.set_base_dir("./wwwroot"); // 默认根目录（但我们的服务并非网站而仅是服务，无需设置这个）
        // 提供的编译服务，打包形成一个网络服务
        // cpp-httplib
        svr.Post("/compile_and_run", [](const Request &req, Response &res)
                 {
            std::string in_json = req.body;
            std::string out_json;
            CompileAndRun::start(in_json, &out_json);
            res.set_content(out_json, "application/json, charset=utf-8"); });
        svr.listen("0.0.0.0", atoi(argv[1]));
        //  in_json: {"code": "#include...", "input": "","cpu_limit":1, "mem_limit":10240}
        //  out_json: {"status":"0", "reason":"","stdout":"","stderr":"",}
        //  通过http 让client 给我们 上传一个json string
        //  下面的工作，充当客户端请求的json串
        // std::string in_json;
        // Json::Value in_value;
        // // R"()", raw string
        // in_value["code"] = R"(#include<iostream>
        // int main(){
        // std::cout << "你可以看见我了" << std::endl;
        // while(1);
        // return 0;
        // })";
        // in_value["input"] = "";
        // in_value["cpu_limit"] = 1;
        // in_value["mem_limit"] = 10240 * 3;
        // Json::FastWriter writer;
        // in_json = writer.write(in_value);
        // std::cout << in_json << std::endl;
        // // 这个是将来给客户端返回的json串
        // std::string out_json;
        // CompileAndRun::start(in_json, &out_json);
        // std::cout << out_json << std::endl;
        return 0;
}
