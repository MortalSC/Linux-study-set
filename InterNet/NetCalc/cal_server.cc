#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "Deamon.hpp"
#include <memory>
#include <signal.h>

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
    Response resp(0, 0); // 默认设定计算结果为：0，计算结果状态码为：正常
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
        if (0 == req._y)
            resp._code = 1; // 计算结果错误码【1】：除 0 错误！
        else
            resp._result = req._x / req._y;
        break;
    case '%':
        if (0 == req._y)
            resp._code = 2; // 计算结果错误码【2】：模 0 错误！
        else
            resp._result = req._x % req._y;
        break;
    default:
        resp._code = 3; // 计算结果错误码【3】：非法运算符！
        break;
    }
    return resp;
}

// 网络计算器实际功能函数：服务端的业务！
void Calaculator(int sock)
{
    // 优化协议的新增内容”缓冲区“
    std::string inbuffer;
    while (true)
    {
        // std::cout << "here ?" << std::endl;

        // 1. 数据读取【不在乎读取的内容的多少】
        // std::string str = Recv(sock, &inbuffer); // 在此处读取到（计算）请求
        bool res = Recv(sock, &inbuffer);
        if (!res)
            break; // 检查数据是否读取成功！

        // 调试参考输出:
        // std::cout << "Calaculator[func] read inbuffer > " << inbuffer << std::endl;

        // bug 修复点：【读取到的数据可能为空！】
        // 优化协议的新增要求：一定要保证读取到一个完整的报文！
        std::string package = Decode(inbuffer); // 获取一个完成的遵循协议的报文！

        // 调试参考输出:
        // std::cout << "Calaculator[func] Decode inbuffer > " << inbuffer << std::endl;
        // std::cout << "Calaculator[func] Decode package > " << package << std::endl;

        // 旧版：协议处理方式
#if 0
        if (!str.empty())
        {

            // 2. 创建计算数据管理的结构体
            Request req;
            // 3. 由于读取的数据是 string，故需要反序列化并把对应数据 req 的成员中
            req.Deserialized(str);
            // 4. 获取计算结果及状态
            Response resp = calculatorHelp(req); // 调用计算方法函数进行结果计算
            // 5. 对计算结果及状态序列化反馈：形成可网络”规范传输“的字符串
            std::string respTOstring = resp.Serialize();
            // 6. 发送
            // bug 修复点：发送失败！【正在发送时，把连接关闭了！】解决方式：使用信号忽略
            Send(sock, respTOstring);
        }
        else
        {
            break;
        }
#endif
        // 新版协议处理方式
        if (package.empty())
            continue; // 不是一个完整报文，继续读取内容！

        // 生成日志文
        logMessage(NORMAL, "%s", package.c_str());

        // 到此处，就一定是一个完整的报文！
        // 2. 创建计算数据管理的结构体
        Request req;
        // 3. 反序列化：字节流 => 结构化；
        req.Deserialized(package);
        // 4. 业务逻辑【本案例中的数据运算】
        Response resp = calculatorHelp(req); // 调用计算方法函数进行结果计算
        // 5. 序列化：回馈结果
        std::string respTOstring = resp.Serialize();

        // 调试参考输出:
        // std::cout << "Calaculator[func] Serialize respTOstring > " << respTOstring << std::endl;

        // 6. 报文加工【注意需要添加长度信息，形成一个完整报文！】
        respTOstring = EnCode(respTOstring);

        // 调试参考输出:
        // std::cout << "Calaculator[func] EnCode respTOstring > " << respTOstring << std::endl;

        // 7. send 在多路转接时再优化！
        Send(sock, respTOstring);
    }
}

void handler(int sig)
{
    std::cout << "get a sig：" << sig << std::endl;
    exit(0);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    // signal(SIGPIPE, handler);（有了守护进程，就可以不在此处使用信号忽略了）

    // 捕获信号，查看客户端退出后，服务端跟着退出的bug！
    // 结果为：13 号信号！信号几乎是网络通信中最常见的问题！
    // 对端关闭：OS 直接杀掉了当前端的服务！
    // 处理方式（两种！）
    // 方式一：signal(SIGPIPE, SIG_IGN);    // 设置信号忽略（13号）
    // 方式二：在服务端读取时需要设置判断！是否读取到空内容！（空内容不操作！）
    // 一般两种方式都用上，方式一：用于解决对端关闭，本端被os强制关闭！方式二：防止发送中，被关闭！
    // 一般经验: server在编写的时候，要有较为严谨性的判断逻辑
    // 一般服务器，都是要忽略SIGPIPE信号的，防止在运行中出现非法写入的问题!│

    // 服务启动前，启用进程守护
    // 说明：在此函数调用后，即启用进程守护，后续不能出现任何的向显示器输出/读入内容（会是程序暂停/终止）
    // 日志信息的输出，使用自定义的logMessage函数向指定文件写入
    MyDaemon();
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