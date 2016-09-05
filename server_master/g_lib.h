#ifndef GUARD_G_LIB_H
#define GUARD_G_LIB_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/un.h>

#define BUFFSIZE 4096  //4096
#define SIZEOFADDRSTR 32

//#define MASTER_UNIXPATH "/tmp/Gchat_master"
#define MASTER_UNIXPATH "/tmp/Gchat_master"

// 发生错误  输出错误提示 并且退出
// 参数  #字符串: 提示    #int: 退出代码(默认参数为1)
void err_exit(const char* tips, int exit_code = EXIT_FAILURE);

// 初始化服务器套接字 设置套接字  绑定IP:host  并开启套接字监听请求
// 参数  #字符串: IP  #字符串: host
// 返回值  套接字文件描述符
int listen_server(char* IP, char* host);


// 初始化master套接字  绑定文件
// 开启master服务
// 参数  绑定的文件绝对地址
// 返回socket套接字
int listen_master(char addr[]);

// 接受客户端连接  返回客户端套接字文件描述符
// 参数 int: 服务器套接字
// 返回值 int 户端套接字文件描述符
int accept_client(int socket_fd, char* client_addr_str);


// 接收worker连接
//返回 客户套接字描述符
int accept_worker(int socket_fd);


// 连接到指定服务器 IP:host
// 参数  #字符串: IP  #字符串: host
// 返回值  套接字文件描述符
int connect_server(char* IP, char* host);

// worker连接master
// 参数  绑定的文件绝对地址
// 返回socket套接字
int connect_master(char addr[]);


// 接收保证固定大小数据
// 参数以及返回值与 系统调用recv 相同
ssize_t recvn(int sockfd, void *buf, size_t nbytes, int flags);


// 发送保证固定大小数据
// 参数以及返回值与 系统调用send 相同
ssize_t sendn(int sockfd, void *buf, size_t nbytes, int flags);

// 发送一个int32数据  一般用于先行告知接收方后面数据包的大小
//static
ssize_t send_int32(int sockfd, int32_t data);


// 接收函数send_int32发送过来的数据
ssize_t recv_int32(int sockfd);


// 获取消息发送时间
void get_time(char* buf, size_t maxsize);

// 实现毫秒级的sleep  可以使用usleep( N *1000) 进行代替
// 参数 毫秒
void milliseconds_sleep(unsigned long mSec);




#endif  // GUARD_G_LIB_H
