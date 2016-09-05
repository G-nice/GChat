#include "g_lib.h"



// 发生错误  输出错误提示 并且退出
// 参数  #字符串: 提示    #int: 退出代码(默认参数为1)
// TODO: 添加向日志文件写入
void err_exit(const char* tips, int exit_code)
{
    fprintf(stderr, "%s: %s(errno: %d)\n",tips, strerror(errno), errno);
    exit(exit_code);
    return;
}


// 连接到指定服务器 IP:host
// 参数  #字符串: IP  #字符串: host
// 返回值  套接字文件描述符
int connect_server(char* IP, char* host)
{
    int server_fd;
        struct sockaddr_in servaddr;


        // 初始化socket_fd
    if ( (server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
        err_exit("create socket error");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = htonl(inet_addr(argv[1]));  //wrong
    if( inet_pton(AF_INET, IP, &servaddr.sin_addr) == -1)    //设置服务器IP地址
    {
        printf("inet_pton error for %s\n",IP);
        err_exit("inet_pton error");
    }
    servaddr.sin_port = htons((uint32_t)atol(host));   //设置端口

    if ( connect(server_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1)
    {
        close(server_fd);
        err_exit("connect to server error");
    }


    return server_fd;
}


// 初始化服务器套接字 设置套接字  绑定IP:host  并开启套接字监听请求
// 参数  #字符串: IP  #字符串: host
// 返回值  套接字文件描述符
int listen_server(char* IP, char* host)
{
    int socket_fd = 0;
    struct sockaddr_in servaddr;
    char buff[128];

    // 初始化socket_fd
    if ( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
        err_exit("create socket error", 1);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = htonl(servip);    //设置服务器IP地址
    if( inet_pton(AF_INET, IP, &servaddr.sin_addr.s_addr) == -1)
    {
        printf("inet_pton error for %s\n",IP);
        err_exit("inet_pton error");
    }
    // servaddr.sin_addr.s_addr = htonl(inet_addr(argv[1]));    //  wrong  wrong
    servaddr.sin_port = htons((uint32_t)atol(host));   //设置端口

    // 设置套接字  能够重用blind中的地址
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)))
    {
        close(socket_fd);
        err_exit("socket option set error");
    }

    // 将运行参数提供的地址绑定到创建的套接字上
    if( bind(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        fprintf(stderr, "socket bind address error: %s(errno: %d)\n",strerror(errno), errno);
        close(socket_fd);
        exit(1);
    }

    // server status
    printf("start server on %s : port %d \n", inet_ntop(AF_INET, (void*) &servaddr.sin_addr, buff, INET_ADDRSTRLEN), ntohs(servaddr.sin_port) );

    // 监听客户端连接
    if ( listen(socket_fd, 128) == -1)  //最大入队未完成连接数
    {
        close(socket_fd);
        err_exit("listen error");
    }

    return socket_fd;
}


// 接受客户端连接  返回客户端套接字文件描述符
// 参数 int: 服务器套接字
// 返回值 int 户端套接字文件描述符
int accept_client(int socket_fd, char* client_addr_str)
{
    int client_fd = 0;
    sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    time_t ticks;
    char buff[128];

    // 接受客户端连接
    //printf("new accept\n");
    if((client_fd = accept(socket_fd, (struct sockaddr *) &cliaddr, &len) ) == -1)
    {
        if (errno == EINTR)    // 被信号处理中断    重新继续
            return -1;
        else
        {
            fprintf(stderr, "server accept error: %s(errno: %d)\n",strerror(errno), errno);
            return -1;
        }
    }
    ticks = time(NULL);  //接收到连接的时间

    // 输出连接到服务器的时间以及客户端的IP地址以及端口号
    inet_ntop(AF_INET, (void*) &cliaddr.sin_addr, buff, INET_ADDRSTRLEN);
    printf("at %.24s\t build connection from %s, port %d \n", ctime(&ticks), buff, ntohs(cliaddr.sin_port) );
    // snprintf(client_addr_str, sizeof(client_addr_str), "%.24s\r\n", ctime(&ticks));
//    snprintf(client_addr_str, sizeof(client_addr_str), "%s : %d", buff, ntohs(cliaddr.sin_port));
    snprintf(client_addr_str, SIZEOFADDRSTR, "%s : %d", buff, ntohs(cliaddr.sin_port));
    // printf("%s\n", client_addr_str);

    return client_fd;
}

// 接收保证固定大小数据
// 参数以及返回值与 系统调用recv 相同
ssize_t recvn(int sockfd, void* buf, size_t nbytes, int flags)
{
    size_t nleft = nbytes;  //剩余字符数
    ssize_t nrecv;  //用于返回值
    uint8_t* buf_ptr = (uint8_t*)buf;

    while(nleft > 0)
    {
        nrecv = recv(sockfd, buf, nbytes, flags);
        if (nrecv == -1)
        {   if (errno == EINTR)  // 被信号打断
                continue;
            return -1;  //err
        }
        else if( nrecv == 0)  //对方结束  关闭了套接字
            return nrecv;  //0;  //读完

        nleft -= nrecv;
        buf_ptr += nrecv;
    }
    return (nbytes - nleft);
}


// 发送保证固定大小数据
// 参数以及返回值与 系统调用send 相同
ssize_t sendn(int sockfd, void* buf, size_t nbytes, int flags)
{
    size_t nleft = nbytes;  //剩余字符数
    ssize_t nsend;  //用于返回值
    uint8_t* buf_ptr = (uint8_t*)buf;

    while(nleft > 0)
    {
        nsend = send(sockfd, buf, nbytes, flags);    //在无连接时候会产生SIGPIPE
        if (nsend <= 0)  //err
        {   if (nsend == -1 && errno == EINTR)  // 被信号打断
                continue;
            return -1;  //err
        }
        else if( nsend == 0)  //对方结束  关闭了套接字
            return nsend;  //0;  //读完

        nleft -= nsend;
        buf_ptr += nsend;
    }
    return (nbytes - nleft);
}

// 发送一个int32数据  一般用于先行告知接收方后面数据包的大小
//static
ssize_t send_int32(int sockfd, int32_t val)
{
    int32_t tmp = htonl(val);    //转化为网络字节序
    if (sendn(sockfd, &tmp, sizeof(int32_t), 0) != sizeof(int32_t))
        err_exit("sendn error");
    return ntohl(tmp);
}


// 接收函数send_int32发送过来的数据
ssize_t recv_int32(int sockfd)
{
    int32_t tmp;
    if (recvn(sockfd, &tmp, sizeof(int32_t), 0) != sizeof(int32_t))
        err_exit("recvn error");
    return ntohl(tmp);
}


// 获取消息发送时间
void get_time(char* buf, size_t maxsize)
{
    time_t tim;
    struct tm* tmp;

    time(&tim);
    tmp = gmtime(&tim);
    strftime(buf, maxsize, "%F %T", tmp);

    return;
}



// 实现毫秒级的sleep
// 参数 毫秒
void milliseconds_sleep(unsigned long mSec)
{
    struct timeval tv;
    tv.tv_sec=mSec/1000;
    tv.tv_usec=(mSec%1000)*1000;
    int err;
    do{
       err=select(0,NULL,NULL,NULL,&tv);
    }while(err<0 && errno==EINTR);
}


