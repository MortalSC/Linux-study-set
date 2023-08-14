#pragma once

#include <iostream>
#include "Sock.hpp"
#include <signal.h>
#include <functional>


class HttpServer
{

    public:
    using func_t = std::function<void (int)>;

private:
    uint16_t _port;  // 服务器端口号
    int _listenSock; // 套接字
    Sock sock;
    func_t _func;
public:
    HttpServer(const uint16_t &port, func_t func)
        : _port(port), _func(func) // 第二个参数：外部传递过来的请求处理方式
    {
        // 服务器构建
        _listenSock = sock.Socket();  // 获取套接字
        sock.Bind(_listenSock, port); // 套接字绑定
        sock.Listen(_listenSock);     // 监听套接字
    }

    void Start(){
        signal(SIGCHLD, SIG_IGN);
        // 启动服务器
        for(;;){
            std::string client_ip;
            uint16_t client_port;
            // 接收客户端信息（ip / port）
            int sockfd = sock.Accept(_listenSock, &client_ip, &client_port);
            if( sockfd < 0 ) continue;

            // 获取成功！
            if(fork() == 0){
            // 不需要文件描述符，就直接关闭
                close(_listenSock);
                // 子进程执行任务：去处理客户端请求！
                _func(sockfd);
                close(sockfd);
                // 为了防止僵尸进程 和 进程等待！引入信号处理！
                exit(0);
            }
            // 不需要文件描述符，就直接关闭
            close(sockfd);
        }
    }

    ~HttpServer()
    {
        if (_listenSock >= 0)
            close(_listenSock);
    }
};