#pragma once

/* 网络计算器实际功能函数:自定义协议 */
/* 在网络传输中我们统一采用字符串进行传输！ */

#include "Sock.hpp"
#include <iostream>
#include <string>
#include <cstring>

namespace ns_protocol
{
#define MYSELF 1
#define SPACE " " // 空格
#define SPACE_LEN strlen(SPACE)

#define SEP "\r\n"
#define SEP_LEN strlen(SEP)

    class Request
    {
    public:
        // 两个版本的设计：数据转成字符串
        // 1. 自主实现：串格式："_x _op _y"

        // 优化 “协议“ 内容：
        // （由于是面向数据流的：上述方案可能出现："_x _op _y_x _op _y_x _op _y_x _op _y"，会造成数据读取问题！）
        // 新的方案："length\r\n_x _op _y\r\n"【即：头部加入标识报文长度，然后根据长度读取完整报文，特殊符号：\r\n 用于间隔区分！】

        // 提问：你如何保证使用：长度 length 一定可以标识报文长度？
        // 答：length 是一个整数！二进制按位标识不可能出现 \r\n 特殊符号标识！
        //    同时，length 后的 \r\n 就是标识 length 的结束！其后紧跟的就是：被操作数据！

        // 提问：你怎么保证规定长度后，你能正确的读取报文长数据内容？如果内容中含有\r\n呢？
        // 答：使用 length 标识的报文的长度，若在长都范围内出现 \r\n 即表示：\r\n 是数据内容的一部分！
        // 超出 \r\n 后的 \r\n 才是报文结束的标识！
        // 2. 使用线程的方案

        // 序列化：功能：将下面的三个数据转成字符串
        std::string Serialize()
        {
#ifdef MYSELF // 自主实现：串格式："_x _op _y"
            std::string str;
            str = std::to_string(_x);
            str += SPACE;
            str += _op;
            str += SPACE;
            str += std::to_string(_y);
            return str;
#else
            std::cout << "TODO" << std::endl;

#endif
        }

        // 反序列化：功能：将字符串转成下面的三个数据
        bool Deserialized(const std::string &package)
        {
#ifdef MYSELF // 自主实现：串格式："_x _op _y"
            // 反序列化，找空格区分数据
            std::size_t left = package.find(SPACE);
            if (left == std::string::npos) // 检查序列化格式的正误！
                return false;
            std::size_t right = package.rfind(SPACE);
            if (right == std::string::npos) // 检查序列化格式的正误！
                return false;
            _x = atoi(package.substr(0, left).c_str());
            _y = atoi(package.substr(right + SPACE_LEN).c_str());
            if (left + SPACE_LEN > package.size())
                return false;
            _op = package[left + SPACE_LEN];
            return true;
#else
            std::cout << "TODO" << std::endl;

#endif
        }

    public:
        Request() {}
        Request(int x, int y, char op)
            : _x(x), _y(y), _op(op)
        {
        }
        ~Request() {}

    public:
        // 此处公有化数据字段（忽略后续需要实现向外部提供访问数据接口）
        int _x;   // 操作数一
        int _y;   // 操作数二
        char _op; // 运算符【+、-、*、/、%】
    };

    // 关于计算请求的状态表示
    class Response
    {
    public:
        // 规定传输格式："_code _result"

        // 序列化：功能：将下面的三个数据转成字符串
        std::string Serialize()
        {
#ifdef MYSELF
            std::string str;
            str = std::to_string(_code);
            str += SPACE;
            str += std::to_string(_result);
            return str;
#else
            std::cout << "TODO" << std::endl;

#endif
        }

        // 反序列化：功能：将字符串转成下面的三个数据
        bool Deserialized(const std::string &package)
        {
#ifdef MYSELF
            // 反序列化：找到空格
            std::size_t pos = package.find(SPACE);
            if (pos == std::string::npos)
                return false;
            _code = atoi(package.substr(0, pos).c_str());
            // if (pos + SPACE_LEN > package.size())
            //     return false;
            _result = atoi(package.substr(pos + SPACE_LEN).c_str());
            return true;
#else
            std::cout << "TODO" << std::endl;

#endif
        }

    public:
        Response() {}
        Response(int result, int code)
            : _result(result), _code(code)
        {
        }
        ~Response() {}

    public:
        int _result; // 计算结果
        int _code;   // 计算结果的状态码
        // 计算结果的状态码：可用于反馈是否出现计算的错误：如：除 0、模 0 等问题！
        // 状态码为：0【正常】
        // 状态码：其他【待定】
        // 计算结果错误码【1】：除 0 错误！
        // 计算结果错误码【2】：模 0 错误！
        // 计算结果错误码【3】：非法运算符！
    };

    // 在协议中规定数据读取方式【临时版】（注意函数内的注释）
#if 0
    std::string Recv(int sock)
    {
        /*
            注意：tcp是，面向字节流的！
            在 recv 接收时！你怎么保证接收到的数据是完整的？（即：如果实际数据大于 inbuffer 的 1024 ）并执行一个完整的请求？
            答：此时，不能保证请求的完整性！

            我们的期望是读取到我们规定的格式："_x _op _y"
            但是，仅凭recv接收函数是无法保证的！可能会出现："_x _op _y_x _op _y_x _op _y_x _op _y"！
            想要保证数据读取的完整性，就必须升级我们的”协议“！或者说优化我们的格式！
            使其可以明确的区分一个运算体的：左操作数 符号 右操作数！
        */
        char inbuffer[1024];
        ssize_t s = recv(sock, inbuffer, sizeof(inbuffer), 0);
        if (s > 0)
        {
            return inbuffer;
        }
        else if (s == 0)
        {
            std::cout << "client quit！" << std::endl;
        }
        else
        {
            std::cout << "recv error！" << std::endl;
        }
        return "";
    }
#endif

    // 新的修改版读取方案实现
    // 参数一：文件描述符
    // 参数二：全局式的缓冲区！（解决：某次读取后如：9\r\n123 +【后续无内容！说明数据暂时未到达！】）
    bool Recv(int sock, std::string *out)
    {
        char buffer[1024];
        ssize_t s = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (s > 0)
        {
            buffer[s] = 0;
            *out += buffer;
        }
        else if (s == 0)
        {
            std::cout << "client quit！" << std::endl;
            return false;
        }
        else
        {
            std::cout << "recv error！" << std::endl;
            return false;
        }
        return true;
    }

    // 在协议中规定数据在网络中发送的方式【临时版】
    void Send(int sock, const std::string str)
    {
        std::cout << "send in" << std::endl;
        int n = send(sock, str.c_str(), str.size(), 0);
        if (n < 0)
            std::cout << "send error！" << std::endl;
        // (void)n;
    }

    // 解析一个完整报文！
    // 报文格式："length\r\n_x _op _y\r\n"
    std::string Decode(std::string &str)
    {
        ssize_t pos = str.find(SEP); // 寻找报文中的分割标识

        // 如果没找到说明，读取的数据是 length / _x _op _y 的部分不完整数据【不做处理】
        if (pos == std::string::npos)
            return "";

        // 解析：
        // 解析的前提是：一个完整报文段！"length\r\n_x _op _y\r\n"
        // 该方式同时保证被操作的str起始一定是 length 开头
        // 1. 提取报文长度数据：
        int size = atoi(str.substr(0,pos).c_str());
        // pos 之前是 length ，
        // 第一个 SEP_LEN 对标 length 后的 \r\n
        // 第二个 SEP_LEN 对标报文核心数据后的 \r\n
        int content_size = str.size() - pos - SEP_LEN - SEP_LEN;

        if (content_size >= size)
        {
            // 到此，保证了至少具有一个合法完整的报文！
            // 如："length\r\n_x _op _y\r\n[.........]"
            str.erase(0, pos + SEP_LEN);            //"length\r\n_x _op _y\r\n[.........]" => "_x _op _y\r\n[.........]"
            std::string s = str.substr(0, size);    // 获取到：_x _op _y；剩余："_x _op _y\r\n[.........]"
            str.erase(0,size + SEP_LEN);            // 去掉：_x _op _y\r\n；剩余："[.........]"
            return s;
        }
        else
        {
            // 到此，说明内容不具备合法性！
            // 如："length\r\n_x _op "
            // 如："length\r\n_x _op _y\r\"
            // ...
            return "";
        }
    }

    // 结果报文格式化处理
    std::string EnCode(std::string& s){
        std::string new_package =std::to_string(s.size());       // 获取报文核心数据长度
        new_package += SEP;
        new_package += s;
        new_package += SEP;
        return new_package;
    }
}
