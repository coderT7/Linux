#include <jsoncpp/json/json.h>
#include <iostream>
#include <string>
using namespace std;
int main()
{
    // json字符串的中间对象，可以向内部填充kv值
    Json::Value root;
    root["code"] = "编码";
    root["user"] = "root";
    root["age"] = "19";
    // 类似哈希表
    // 定义一个写对象，可以将上面的中间对象序列化成字符串
    Json::StyledWriter writer;
    string str = writer.write(root);
    cout << str << endl;
    // 编译时要带上 -ljsoncpp 引入json库
    return 0;
}