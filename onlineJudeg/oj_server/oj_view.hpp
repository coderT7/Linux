#pragma once
#include "../comment/log.hpp"
#include "oj_model.hpp"
#include <ctemplate/template.h>

namespace ns_view
{
    using namespace ns_log;
    using namespace ns_model;

    const std::string html_path = "./questions_html/";
    class View
    {
    public:
        void expandAllQuestions(const vector<Question>& questions, std::string* html)
        {
            // 使用ctemplate库进行数据渲染（将数据添加到网页中）
            // 获取要被渲染的网页路径
            std::string path = html_path + "all_questions.html";
            // 构建数据字典
            ctemplate::TemplateDictionary root("questions");
            for(const auto& val : questions)
            {
                // 获取子字典，用于循环插入数据
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("question_list");
                sub->SetValue("number", val._number);
                sub->SetValue("name", val._name);
                sub->SetValue("level", val._level);
            }
            // 获取要被渲染的网页
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(path, ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &root);
        }
        void expandOneQuestion(const Question& q, std::string* html)
        {
            // 获取路径
            std::string path = html_path + "one_question.html";
            // 获取数据字典
            ctemplate::TemplateDictionary root("question");
            root.SetValue("number", q._number);
            root.SetValue("name", q._name);
            root.SetValue("level", q._level);
            root.SetValue("desc", q._desc);
            root.SetValue("header", q._header);
            // 获取要渲染的网页对象
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(path, ctemplate::DO_NOT_STRIP);
            // 进行渲染并输出
            tpl->Expand(html, &root);
        }
    };
}