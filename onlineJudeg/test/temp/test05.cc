#include <iostream>
#include <string>
#include <ctemplate/template.h>

int main()
{
    const std::string in_html = "./test.html";
    const std::string value = "helloworld";
    //  建立数据字典结构
    ctemplate::TemplateDictionary tempDic("test");
    // 添加数据（kv形式）
    tempDic.SetValue("key", value);
    // 获取被渲染的网页对象
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(in_html, ctemplate::DO_NOT_STRIP);
    // 进行渲染，并获取渲染的结果
    std::string out_htmp;
    tpl->Expand(&out_htmp, &tempDic);

    std::cout << "渲染后：" << std::endl << out_htmp << std::endl;

    return 0;
}