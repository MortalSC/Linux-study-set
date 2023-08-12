#pragma

#include "Sock.hpp"
#include <functional>
#include <pthread.h>
#include <vector>

namespace Mortal_TcpServer
{

    using func_t = std::function<void(int)>;

    class TcpServer;

    // 用于实现在 静态成员函数中访问到必要的数据！
    class ThreadData{
    public:
        ThreadData(int sock, TcpServer*server)
            :_sock(sock), _server(server)
        {}
        ~ThreadData(){}
    public:
        int _sock;
        TcpServer *_server;
    };

    class TcpServer
    {
    private:
        static void *ThreadRoutine(void *args)
        {
            // 直接线程分离（脱离主线程独立执行任务，执行完毕后直接被释放！）
            pthread_detach(pthread_self());
            ThreadData* td = static_cast<ThreadData*>(args);
            // 执行方法
            td->_server->Execute(td->_sock);
            close(td->_sock);       // 具体看功能场景
            delete td;
        }

    public:
        TcpServer(const uint16_t &port, const std::string &ip = "0.0.0.0")
        {
            // 构造对象中直接创建套接字
            _listensock = _sock.Socket();
            // 绑定服务对象（port / ip）
            _sock.Bind(_listensock, port, ip);
            // 监听服务请求
            _sock.Listen(_listensock);
        }
        // 被执行的方法与对象绑定
        void BandHandler(func_t func)
        {
            // _func = func;
            // 对于多方法，直接压入方法”库“
            _funcs.push_back(func);
        }

        // 执行
        void Execute(int sock){
            // _func(sock);
            // 多方法执行
            for(auto & f : _funcs){
                f(sock);
            }
        }


        // 服务器的启动
        void Start()
        {
            for (;;)
            {
                std::string client_ip;
                uint16_t client_port;
                // 通过输出型参数获取到客户端的ip和port
                int sock = _sock.Accept(_listensock, &client_ip, &client_port);
                if (sock == -1)
                    continue;
                logMessage(NORMAL, "create new link success！sock = %d", sock);

                // 启用多线程
                pthread_t tid;
                ThreadData* td = new ThreadData(sock, this);
                pthread_create(&tid, nullptr, ThreadRoutine, td);
            }
        }


        ~TcpServer()
        {
            if (_listensock >= 0)
                close(_listensock);
        }

    private:
        int _listensock;
        Sock _sock;
        func_t _func;
        // 一堆方法
        std::vector<func_t> _funcs;
        // 如果每个方法还有名字
        // std::unordered_map<std::string, func_t> _funcs;
    };
}