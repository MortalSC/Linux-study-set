#include <iostream>
#include "Sock.hpp"
#include <memory>
#include "HttpServer.hpp"
#include "Usage.hpp"
#include "util.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>

// 一般http都要有自己的web根目录
#define ROOT "./wwwroot" // ./wwwroot/index.html
// 如果客户端只请求了一个/,我们返回默认首页
#define HOMEPAGE "index.html"

void HandlerHttpRequest(int sockfd)
{
    char buffer[10240];
    ssize_t s = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    // 读取请求
    if (s > 0)
    {
        buffer[s] = 0;
        // std::cout << buffer << "====================\n"
        //           << std::endl;
        /*  实验数据示例
        GET / HTTP/1.1
        Host: 159.75.137.190:8080
        Connection: keep-alive
        Cache-Control: max-age=0
        Upgrade-Insecure-Requests: 1
        User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36 Edg/115.0.1901.203
        Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*
                /*;q=0.8,application/signed-exchange;v=b3;q=0.7
        Accept-Encoding: gzip, deflate
        Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6
        ====================
        */
    }

    // 获取用户需要访问的页面路径

    // 解析读取到的请求报文，存入容器
    std::vector<std::string> vline;
    Util::cutString(buffer, "\n", &vline);
    // 测试容器内的内容
    // for(auto & e: vline){
    //     std::cout << e << "\n" <<  std::endl;
    // }

    std::vector<std::string> vblock;
    Util::cutString(vline[0], " ", &vblock); // 获取第一行的三个数据：请求方法、访问的资源目录、协议版本号

    // 获取用户需要访问的路径
    std::string file = vblock[1];
    std::string target = ROOT;

    if (file == "/")
        file = "/index.html";
    target += file;
    // std::string target = ROOT;
    // target += vblock[1];

    // std::cout << target << std::endl;

    // 去打开文件
    // int fd = open(target.c_str(), O_RDONLY);        // 只读的方式打开
    // if(fd < 0){
    //     // 打开失败，说明：访问的资源不存在！即返回 404
    // }
    // else{
    //     // 打开成功
    // }
    // C++ 方式：
    std::string content;
    std::ifstream in(target);
    if(in.is_open())
    {
        std::string line;
        while(std::getline(in, line))
        {
            content += line;
        }
        in.close();
    }

    // 尝试返回响应：
    std::string HttpResponse;
    // if(content.empty()) HttpResponse = "HTTP/1.1 404 NotFound\r\n";

    // 实验二：探索 3XX 状态码的重定向
    if(content.empty()){
         HttpResponse = "HTTP/1.1 302 Found\r\n";           
         // 如果当获取的链接为空时，模拟资源地址更新，此时应当向客户端反馈新的地址
        //  HttpResponse += "location :https://www.baidu.com\r\n";      // 仅测试，若我们的资源转移了，让用户重定向访问：百度 
         HttpResponse += "location: http://159.75.137.190:8080/a/b/404.html\r\n";      
        // 仅测试，若我们的资源转移了，让用户重定向访问：我们自己的目录：回显：404资源不存在！ 

    }
    else {
        // 实验三：探索 header 中的常用属性
        HttpResponse = "HTTP/1.1 200 OK\r\n";
        // 上面的：location，是重定向地址属性
        // Content-Type：资源的类型【类型是使用后缀来区分的！】 / 现在的【多数】浏览器：都可以自动推断类型
        HttpResponse += ("Content-Type:text/html\r\n" );      // text/html：是简单文本属性

        // Content-Length：正文长度属性
        HttpResponse += ("Content-Length:" +std::to_string(content.size()) + "\r\n" );      
        // 
    }
    HttpResponse += "\r\n";
    HttpResponse += content;

    send(sockfd, HttpResponse.c_str(), HttpResponse.size(), 0);

    // std::cout << vblock[1] << std::endl;
    // std::cout << "####start################" << std::endl;
    // for (auto &iter : vblock)
    // {
    //     std::cout << "---" << iter << "\n"
    //               << std::endl;
    // }
    // std::cout << "#####end###############" << std::endl;

    // 尝试返回响应：
    // std::string httpRespone = "HTTP/1.1 200 OK\r\n";                // 模拟：状态行
    // httpRespone += "\r\n";                                          // 由于不知道响应报头中的属性有哪些！就暂时忽略
    // httpRespone += "<html><h3>http 协议模拟返回响应！</h3></html>"; // 模拟一个资源返回

    // send(sockfd, httpRespone.c_str(), httpRespone.size(), 0);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    // 实例化
    std::unique_ptr<HttpServer> httpserver(new HttpServer(atoi(argv[1]), HandlerHttpRequest));

    // 启动
    httpserver->Start();

    return 0;
}