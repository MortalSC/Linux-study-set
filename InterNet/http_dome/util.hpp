#pragma once

/* 一个根据类，用于处理字符串 */

#include <iostream>
#include <vector>

class Util
{
public:
    // 实现对http请求的报文进行部分切割
    // 参数【sep】：指定的分割符
    static void cutString(const std::string &s, const std::string &sep, std::vector<std::string> *out)
    {
        std::size_t start = 0;
        while (start < s.size())
        {
            auto pos = s.find(sep, start);
            if (pos == std::string::npos)
                break;
            std::string sub = s.substr(start, pos - start);
            // std::cout << "----" << sub << std::endl;
            out->push_back(sub);
            start += sub.size();
            start += sep.size();
        }
        if (start < s.size())
            out->push_back(s.substr(start));
    }
};