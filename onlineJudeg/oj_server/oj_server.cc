#include "../comment/httplib.h"
#include "oj_control.hpp"
using namespace httplib;
using namespace ns_control;
int main()
{
    Server svr;
    Control ctrl;
    svr.set_base_dir("./wwwroot");
    // 访问所有的题目资源
    svr.Get("/question_list", [&ctrl](const Request &req, Response &resp)
            {
                std::string html;
                ctrl.getAllQuestions(html);
                resp.set_content(html, "text/html; charset=utf-8"); });
    // 访问某一题的题目资源
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
            { 
        std::string number = req.matches[1];
        std::string html;
        ctrl.getOneOfQuestions(number, html);
        resp.set_content(html, "text/html; charset=utf-8"); });
    // 访问代码编译资源
    svr.Get(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp)
            {
                std::string number = req.matches[1];
                std::string out_json; 
                ctrl.judge(number, req.body, out_json);
                resp.set_content(out_json, "application/json; charset=utf-8"); });
    svr.listen("0.0.0.0", 8080);
}