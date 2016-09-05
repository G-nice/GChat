#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>

#include "g_lib.h"
#include "protocol.h"
#include "epoll.h"
#include "user_manager.h"
#include "signal_handle.h"

using std::vector;
using std::string;



int main()
{
    int socket_fd = 0;
    int worker_fd = 0;
    static int fd_tmp;
    packet pack_buff;
    backdata_package backdata_pack;
    privateMsg_package privateMsg_pack;

//    sighandler_t sh;

    int epfd;  // epoll句柄
    int ready_num;

    static int n = 0;  //parse_pack 记数

    extern sigjmp_buf jmpbuf;
    extern sig_atomic_t canjmp;
    canjmp = 0;

    userManager usermang;  // 用户管理对象

    socket_fd = listen_master(MASTER_UNIXPATH);

    // 创建epoll句柄
    if ((epfd = epoll_create(EPOLL_SIZE)) == -1 )  // 1024
        err_exit("epoll create error");

    epoll_event events[EPOLL_SIZE];
    epoll_add_event(epfd, socket_fd);

    register_signal_handler();


    // 设置信号跳转  使得用户管理类能够调用析构函数
    if (sigsetjmp(jmpbuf, 1))
    {
        close(socket_fd);
        close(epfd);
        return EXIT_SUCCESS;
    }
    canjmp = 1;

    // 等待worker连接
    printf("start wait worker connect(accept)\n");

    static int i;    // epoll 循环变量
    while(true)
    {
        ready_num = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_TIMEOUT);  // -1

        if (ready_num == -1)
        {
            close(socket_fd);
            close(worker_fd);
            close(epfd);
            err_exit("epoll_wait error");
        }

        for (i = 0; i < ready_num; ++i)
        {
            if(events[i].data.fd == socket_fd) //有新的连接
            {
                // 接收客户端连接请求  输出连接相关信息
                if ( (worker_fd = accept_worker(socket_fd)) == -1)
                    continue;
                epoll_add_event(epfd, worker_fd);
            }

            else if( events[i].events == EPOLLIN ) //接收到来自worker数据，读socket  处理数据包
            {
                if ( (n = parse_pack(events[i].data.fd, pack_buff)) == -1)
                {
                    perror("recv message error");
                    exit(EXIT_FAILURE);
                }
                else if (n == 0)
                {
                    fprintf(stderr, "worker has closed.\n");
                    close(worker_fd);
                    close(events[i].data.fd);
                    epoll_remove_event(epfd, events[i].data.fd);
                }


//                if (pack_buff.package_type == GROUPMSG)  // 不会有
//                    continue;
                if (pack_buff.package_type == PRIVATEMSG)
                {
                    memset(&privateMsg_pack, 0, sizeof(privateMsg_pack));
                    int socket_fd = 0;
                    if((socket_fd = usermang.user_socketfd(pack_buff.data.privateMsg_pak)) == -1)  // 用户已经下线
                    {
                        pack_buff.data.backdata_pak.data_1 = -1;
                        build_pack(events[i].data.fd, pack_buff.data.backdata_pak);
                    }
                    else
                    {
                        build_pack(socket_fd, pack_buff.data.privateMsg_pak);  //转发消息
//                        printf("## message: %s\n", pack_buff.data.privateMsg_pak.massage);
                        pack_buff.data.backdata_pak.data_1 = 0;
                        build_pack(events[i].data.fd, pack_buff.data.backdata_pak);  // 告知发送成功
                    }
                }

                else if (pack_buff.package_type == LOGIN)
                {
                    if (!usermang.logincheck(pack_buff.data.login_pak, events[i].data.fd))
                        backdata_pack.data_1 = -1;
                    else
                        backdata_pack.data_1 = 0;

                    build_pack(events[i].data.fd, backdata_pack);
                }

                else if (pack_buff.package_type == LOGOUT)
                {
                    usermang.logout(pack_buff.data.logout_pak);
                }

                else if (pack_buff.package_type == BACKDATA)  // 群聊消息同步请求 -666  // 获取在线用户列表 -888
                {
                    if (pack_buff.data.backdata_pak.data_2 == -666)
                    {
                        backdata_pack.data_5 = usermang.latest_grp_MsgID(pack_buff.data.backdata_pak);
//                        printf("svn MsgID: %u\n", backdata_pack.data_5);
                        build_pack(events[i].data.fd, backdata_pack);
                    }
                    else if(pack_buff.data.backdata_pak.data_2 == -888)
                    {
                        vector<string> online_user_list;
                        memset(&backdata_pack, 0, sizeof(backdata_package));
                        int online_amount = 0;

                        if (usermang.online_user(pack_buff.data.backdata_pak, online_user_list))
                        {
                            online_amount = (unsigned short)online_user_list.size();

                            vector<string>::const_iterator iter = online_user_list.begin();  // 不赋初值会导致段错误  核心转存
                            backdata_pack.data_3 = online_amount;  // 总条数

                            while(online_amount > 0)
                            {
                                if (iter != online_user_list.end())
                                {
                                    strncpy(backdata_pack.msg_1, (*iter).c_str(), sizeof(backdata_pack.msg_1));
                                    --online_amount;
                                    ++iter;
                                }
                                if (iter != online_user_list.end())
                                {
                                    strncpy(backdata_pack.msg_2, (*iter).c_str(), sizeof(backdata_pack.msg_2));
                                    --online_amount;
                                    ++iter;
                                }
                                backdata_pack.data_1 = online_amount;  // 剩余条数
                                build_pack(events[i].data.fd, backdata_pack);
                            }
                        }
                        else  // 无其余在线人员
                        {
                            backdata_pack.data_1 = -1;
                            build_pack(events[i].data.fd, backdata_pack);
                        }
                    }
                }

                else if (pack_buff.package_type == MODPWD)
                {
                    if ( !usermang.modify_password(pack_buff.data.modPwd_pak))
                        backdata_pack.data_1 = -1;
                    else
                        backdata_pack.data_1 = 0;

                    build_pack(events[i].data.fd, backdata_pack);

                }

                else if (pack_buff.package_type == REGISTER)
                {
                    if (!usermang.register_user(pack_buff.data.register_pak, events[i].data.fd))  // 注册账户并登陆
                    {
                        backdata_pack.data_1 = -1;
                        build_pack(events[i].data.fd, backdata_pack);
                    }
                    else
                    {
                        backdata_pack.data_1 = 1;
                        build_pack(events[i].data.fd, backdata_pack);
                    }
                }

            }

            else if (events[i].events == EPOLLHUP)  // 断开
            {
                fd_tmp = events[i].data.fd;
                epoll_remove_event(epfd, events[i].data.fd);
                close(fd_tmp);
printf("~worker closed(EPOLLHUP)\n");
            }

            else  // 其他未知情况
            {
//                continue;
                fd_tmp = events[i].data.fd;
                epoll_remove_event(epfd, events[i].data.fd);
                close(fd_tmp);
printf("~worker closed(else)  events[i].events == %u\n", events[i].events);
            }

        }


    }
    close(socket_fd);
//    exit(EXIT_SUCCESS);
    return 0;
}
