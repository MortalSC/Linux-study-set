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
    class Request
    {
    public:
        // 两个版本的设计：数据转成字符串
        // 1. 自主实现：串格式："_x _op _y"
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
            if(left + SPACE_LEN > package.size()) return false;
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
            if(pos == std::string::npos) return false;
            _code = atoi(package.substr(0, pos).c_str());
            if(pos + SPACE_LEN > package.size()) return false;
            _result = atoi(package.substr(pos+SPACE_LEN).c_str());
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

    // 在协议中规定数据读取方式【临时版】
    std::string Recv(int sock)
    {
        char inbuffer[1024];
        ssize_t s = recv(sock, inbuffer, sizeof(inbuffer), 0);
        if (s > 0)
        {
            return inbuffer;
        }
        else if(s == 0){
            std::cout << "client quit！" << std::endl; 
        }else{
            std::cout << "recv error！" << std::endl;
        }
        return "";
    }

    // 在协议中规定数据在网络中发送的方式【临时版】
    void Send(int sock, const std::string str)
    {
        std::cout << "send in" << std::endl;
        int n = send(sock, str.c_str(), str.size(), 0);
        if( n < 0) std::cout << "send error！" << std::endl;
        (void)n;
    }
}
