#include "../comment/httplib.h"

using namespace httplib;

int main()
{
    Server svr;
    svr.set_base_dir("./wwwroot");
    // 访问所有的题目资源
    svr.Get("/questions_list", [](const Request &req, Response &resp)
            { resp.set_content("获取题目列表", "text/plain; charset=utf-8"); });
    // 访问某一题的题目资源
    svr.Get(R"(/questions/(\d+))", [](const Request &req, Response &resp)
            { 
        std::string number = req.matches[1];
        resp.set_content("获取题目：" + number, "text/plain; charset=utf-8"); });
    // 访问代码编译资源
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp)
            {
                std::string number = req.matches[1];
                resp.set_content("判断题目：" + number, "text/plain; charset=utf-8"); });
    svr.listen("0.0.0.0", 8080);
}