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

    while (true)
    {
        Request req(10, 20, '+');
        std::string str = req.Serialize();
        Send(sockfd, str);

        std::string rstr = Recv(sockfd);
        Response temp;
        temp.Deserialized(rstr);
        std::cout << "_code: " << temp._code << std::endl;
        std::cout << "_result: " << temp._result << std::endl;
        sleep(1);
    }

    return 0;
}