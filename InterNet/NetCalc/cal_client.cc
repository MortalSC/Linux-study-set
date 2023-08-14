#include <iostream>
#include "Sock.hpp"
#include "Protocol.hpp"

using namespace ns_protocol;

// 参数用例提示
static void Usage(const std::string &process)
{
    std::cout << "\nUsage：" << process << "ip port\n"
              << std::endl;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string client_ip = argv[1];
    uint16_t client_port = atoi(argv[2]);
    Sock sock;
    int sockfd = sock.Socket();
    if (!sock.Connect(sockfd, client_ip, client_port))
    {
        std::cerr << "Connect Error！" << std::endl;
        exit(2);
    }

    bool quit = false;
    std::string buffer;

    while (!quit)
    {
        // 含长度的报文处理方式
        // 1. 获取需求
        Request req;
        // std::cout << "Please Enter x > ";
        // std::cin >> req._x;
        // std::cout << "Please Enter y > ";
        // std::cin >> req._y;
        // std::cout << "Please Enter op [+、-、*、/、%] > ";
        // std::cin >> req._op;
        std::cout << "Please Enter > ";
        std::cin >> req._x >> req._op >> req._y;

        // 2. 序列化！
        std::string str = req.Serialize();

        // 3. 添加长度报头加工！
        str = EnCode(str);
        // 4. 发送给服务端
        Send(sockfd, str);

        // 5. 正常读取反馈结果
        while (true)
        {

            bool res = Recv(sockfd, &buffer);
            if (!res)
            {
                quit = true;
                break; // 如果出现读取失败！跳出循环！并关闭文件描述符！
            }

            // 获取正确报文格式
            std::string package = Decode(buffer);
            if (package.empty())
                continue;

            Response temp;
            temp.Deserialized(package);

            // std::cout << "_code: " << temp._code << std::endl;
            // std::cout << "_result: " << temp._result << std::endl;

            std::string err;
            switch (temp._code)
            {
            case 1:
                err = "除0错误";
                break;
            case 2:
                err = "模0错误";
                break;
            case 3:
                err = "非法操作";
                break;
            default:
                std::cout << req._x << req._op << req._y << " = " << temp._result << " [success]" << std::endl;
                break;
            }
            if (!err.empty())
                std::cerr << err << std::endl;
            // sleep(1);
            break; // 一次处理一个报文
        }

        // 旧式报文处理方式
        // // Request req(10, 20, '+');

        // // 引入简单交互
        // Request req;
        // std::cout << "Please Enter x > ";
        // std::cin >> req._x;
        // std::cout << "Please Enter y > ";
        // std::cin >> req._y;

        // std::cout << "Please Enter op [+、-、*、/、%] > ";
        // std::cin >> req._op;

        // std::string str = req.Serialize();
        // Send(sockfd, str);

        // std::string rstr = Recv(sockfd);
        // Response temp;
        // temp.Deserialized(rstr);
        // std::cout << "_code: " << temp._code << std::endl;
        // std::cout << "_result: " << temp._result << std::endl;
        // sleep(1);
    }
    close(sockfd);
    return 0;
}