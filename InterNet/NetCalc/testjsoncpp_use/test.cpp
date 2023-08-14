#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>

int main()
{

    int a = 10;
    int b = 20;
    char c = '+';

    // 使用示例
    // 序列化用法
    // 1. 创建万能对象：json 数据格式就是键值对模型
    Json::Value root;
    root["aa"] = a;
    root["bb"] = b;
    root["op"] = c;

    // json 提供的序列化方法
    // Json::StyledWriter writer;       // 该格式常用于调试时的查看，分行显示清晰！
    /* 返回的序列化格式：
        {
        "aa" : 10,
        "bb" : 20,
        "op" : 43
        }
    */
    Json::FastWriter writer; // 常用
    /* 返回的序列化格式：
        {"aa":10,"bb":20,"op":43}
    */
    std::string s = writer.write(root); // write 方法就是直接把指定的对象进行序列化并返回 string【结构化的键值对数据体】
    std::cout << s << std::endl;

    ///////////////////////////////////////////////////////////////////////////
    // 反序列化
    Json::Value root;                   // 定义存储对象
    Json::Reader reader;                // 使用 Reader 方法
    reader.parse(s, root);              // 将 序列化的串 s 解析（反序列化）到 root 对象中
    // 后续（通过键值对的方式）提取内容数据

    
    // _x = root["_x"].asInt();
    // _y = root["_y"].asInt();
    // _op = root["_op"].asInt();

    // 扩展：json 支持 子json 结构
    // 如：
    // Json::Value root;
    // root["aa"] = a;
    // root["bb"] = b;
    // root["op"] = c;

    // Json::Value sub;
    // sub["sub1"] = a;
    // sub["sub2"] = b;

    // root["sub"] = sub;
    // Json::StyledWriter writer1;       // 该格式常用于调试时的查看，分行显示清晰！
    // std::string str = writer1.write(root);
    // std::cout << str << std::endl;

    /*
        输出：
            {
                "aa" : 10,
                "bb" : 20,
                "op" : 43,
                "sub" : {
                    "sub1" : 10,
                    "sub2" : 20
                }
            }
    */

    return 0;
}