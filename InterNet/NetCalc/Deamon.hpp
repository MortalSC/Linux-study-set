#pragma once

#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// 设计实现守护进程分离！

// 工作内容：
// 1. 忽略信号，如：SIGPIPE，SIGCHLD
// 2. 不要让自己称为组长，进程组的组长不能被分离出来并成立新的独立会话！
// 3. 调用 setsid：创建新的进程组
// 4. 标准输入输出错误的重定向！【守护进程不能向显示器打印消息，否则会被暂停 / 终止】

void MyDaemon()
{
    // 1. 忽略信号，如：SIGPIPE，SIGCHLD
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    // 2. 不要让自己称为组长，进程组的组长不能被分离出来并成立新的独立会话！
    // 实现方式，创建子进程
    if (fork() > 0)
        exit(0); // fork > 0 说明是父进程！使其退出！后续就是子进程

    // 3. 调用 setsid：创建新的进程组
    setsid();

    // 4. 标准输入输出错误的重定向！【守护进程不能向显示器打印消息，否则会被暂停 / 终止】
    // 介绍一个 dev/null 文件：作用：在Linux下由于一个文件黑洞！
    // 你可以向其中任意写入 / 读出内容（不会读出任何内容，就是空），都不影响我们的主线进程！
    // dev/null 文件效果比喻：时刻进行永久删除写入内容的垃圾桶！
    int devnull = open("/dev/null", O_RDONLY | O_WRONLY);
    if (devnull > 0)
    {
        // 重定向
        dup2(0, devnull); // 参数：（一）旧的输出位置；（二）重定向到的新位置
        dup2(1, devnull);
        dup2(2, devnull);
        close(devnull);
    }
}