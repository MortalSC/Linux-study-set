#include "TcpServer.hpp"
#include "Protocol.hpp"
#include <memory>

using namespace Mortal_TcpServer;
using namespace ns_protocol;

// 参数用例提示
static void Usage(const std::string &process)
{
    std::cout << "\nUsage：" << process << " port\n"
              << std::endl;
}

// 测试绑定服务代码
void debug(int sock)
{
    std::cout << "我是测试服务方法绑定的！读到的 sock ：【" << sock << "】" << std::endl;
}

// 计算：拿到结构化的数据包，读取其中数据，并完成运算，而后在打包反馈
static Response calculatorHelp(const Request &req)
{
    Response resp(0, 0);    // 默认设定计算结果为：0，计算结果状态码为：正常
    switch (req._op)
    {
    case '+':
        resp._result = req._x + req._y;
        break;
    case '-':
        resp._result = req._x - req._y;

        break;
    case '*':
        resp._result = req._x * req._y;

        break;
    case '/':
        if(0 == req._y) resp._code = 1;         // 计算结果错误码【1】：除 0 错误！
        else resp._result = req._x / req._y;
        break;
    case '%':
        if(0 == req._y) resp._code = 2;         // 计算结果错误码【2】：模 0 错误！
        else resp._result = req._x % req._y;
        break;
    default:
        resp._code = 3;                        // 计算结果错误码【3】：非法运算符！
        break;
    }
    return resp;
}

// 网络计算器实际功能函数：服务端的业务！
void Calaculator(int sock)
{
    while (true)
    {
        // 1. 数据读取
        std::string str = Recv(sock); // 在此处读取到（计算）请求
        // 2. 创建计算数据管理的结构体
        Request req;
        // 3. 由于读取的数据是 string，故需要反序列化并把对应数据 req 的成员中
        req.Deserialized(str);
        // 4. 获取计算结果及状态
        Response resp = calculatorHelp(req); // 调用计算方法函数进行结果计算
        // 5. 对计算结果及状态序列化反馈：形成可网络”规范传输“的字符串
        std::string respTOstring = resp.Serialize();
        // 6. 发送
        Send(sock, respTOstring);
    }
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    // 使用智能指针！
    std::unique_ptr<TcpServer> server(new TcpServer(atoi(argv[1])));

    // 绑定服务方法
    // server->BandHandler(debug);
    server->BandHandler(Calaculator);
    // 启动服务器
    server->Start();

    // // 本地测试
    // // 序列化
    // Request req(123,456,'+');           // 数据体
    // std::string s = req.Serialize();    // 序列化
    // std::cout << s << std::endl;        // 序列化的结果

    // Request resp;
    // resp.Deserialized(s);
    // std::cout << "_x: " << resp._x << std::endl; 
    // std::cout << "_op: " << resp._op << std::endl; 
    // std::cout << "_y: " << resp._y << std::endl; 

    // Response resp(0,1);
    // std::string s = resp.Serialize();    // 序列化
    // std::cout << s << std::endl;        // 序列化的结果

    // Response temp;
    // temp.Deserialized(s);
    // std::cout << "_code: " << temp._code << std::endl; 
    // std::cout << "_result: " << temp._result << std::endl; 

    return 0;
}