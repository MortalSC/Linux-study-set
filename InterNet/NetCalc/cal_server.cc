#include "TcpServer.hpp"
#include <memory>

using namespace Mortal_TcpServer;

// 参数用例提示
static void Usage( const std::string &process){
    std::cout << "\nUsage：" << process << " port\n" << std::endl;
}

// 测试绑定服务代码
void debug(int sock){
    std::cout << "我是测试服务方法绑定的！读到的 sock ：【" << sock << "】" << std::endl;  
}

int main(int argc, char* argv[]){

    if(argc != 2){
        Usage(argv[0]);
        exit(1);
    }

    // 使用智能指针！
    std::unique_ptr<TcpServer> server(new TcpServer(atoi(argv[1])));

    // 绑定服务方法
    server->BandHandler(debug);
    // 启动服务器
    server->Start();

    return 0;
}