#ifndef GUARD_SIGNAL_HANDLE_H
#define GUARD_SIGNAL_HANDLE_H
// 信号处理

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>    //getpid

#include "g_lib.h"

typedef void (*sighandler_t) (int);


// 设置信号处理程序
// 参数 #int: 信号    #sighandler_t: 信号处理函数指针
sighandler_t Signal(int signo, sighandler_t sigfun);

//// 所有信号的处理程序
//// 参数   int: 发生的信号
//void signal_handler(int signo);
//
////注册多个信号处理函数
//void register_signal_handler();
//
//// 解除对所有信号的阻塞
//// 返回之前的信号集
//void unblock_all_signals(sigset_t* oset = NULL);
//
//// 阻塞较有可能出现的信号
//// 返回之前的信号集
//void block_specified_signals(sigset_t* oset = NULL);




#endif // GUARD_SIGNAL_HANDLE_H


// 服务器子进程终止信号处理  防止僵死进程
//void sig_chld(int signo);
