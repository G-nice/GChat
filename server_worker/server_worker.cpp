#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/wait.h>


#include "g_lib.h"
#include "semaphore.h"
#include "sharememory.h"
#include "protocol.h"
#include "epoll.h"
#include "groupchat.h"
#include "signal_handle.h"



#define CHAT_RECORD_NUM 8192

#ifndef BUFFSIZE
    #define BUFFSIZE 4096
#endif // BUFFSIZE


// 全局变量
int socket_fd, client_fd;  // 套接字放全局只是为了让信号处理程序使用
unsigned int latest_ID;  // 最新消息标号
groupMsg_package* pack_ptr = nullptr;  //C++11  nullptr  // 放全局是为了让信号处理程序使用
packet packet_tmp;
unsigned int previous_MsgID = 0;  // 之前消息记录的数量



int main(int argc, char* argv[])
{
    char client_addr_str[SIZEOFADDRSTR];
    static int n = 0;  //parse_pack 记数
    time_t ticks;
    pid_t clild_pid = 0;
    groupMsg_package message_buff;
    memset(&message_buff, 0, sizeof(groupMsg_package));

    extern int sem_id;
    extern int shm_id;    // 信号量集  共享内存  标识符

    sigset_t oset;  // 信号阻塞

    int epfd;  // epoll句柄
    int ready_num;
    static int i;    // epoll 循环变量

    int master_fd;


// 检查参数  使用方法提示
    if (argc != 3)
    {
        fprintf(stderr, "Usage: chat_server <IP> <PORT>\n");
        exit(1);
    }

    // 创建 服务器监听 套接字
    socket_fd = listen_server(argv[1], argv[2]);

    //  设置所有信号处理函数  除SIGUSR1
    register_signal_handler();


    // 读者----写者问题  共享内存并发同步
    // 创建信号量集
    sem_id = Semget(IPC_PRIVATE, 3, 0666 | IPC_CREAT);
    // 设置信号量集初值
    set_semvalue(sem_id, MUTEX, 1);
    set_semvalue(sem_id, PREWMUTEX, 0);
    set_semvalue(sem_id, READERNUM, 128);    // 最多允许128个读者同时读取数据


    // 创建共享内存
    shm_id = Shmget(IPC_PRIVATE, CHAT_RECORD_NUM*sizeof(groupMsg_package), 0666 | IPC_CREAT);
    pack_ptr = (groupMsg_package*)shmat(shm_id, 0, 0);
    // 设置聊天记录标志为 0
    pack_ptr[0].MsgID = 0;

    // 读取消息记录文件
    FILE* fp;
    if (access("./message_record.dat", F_OK) == 0)    // 记录文件存在
    {
        fp = fopen("./message_record.dat", "rb");
        fread(pack_ptr, sizeof(groupMsg_package), 1, fp);
        previous_MsgID = get_MsgID(pack_ptr, sem_id);
printf("~read message record amount: %d\n", previous_MsgID);
        fread(pack_ptr + 1, sizeof(groupMsg_package), previous_MsgID, fp);
    }
    else    // 记录文件不存在  创建消息记录文件
    {
        latest_ID = 0;  // 记录文件中消息的条数
        fp = fopen("./message_record.dat", "wb");
    }
    fclose(fp);
    memset(&message_buff, 0, sizeof(groupMsg_package));


    // 等待客户连接
    printf("start wait client connect(accept)\n");

    while(true)  // true
    {

        // 接收客户端连接请求  输出连接相关信息
        if ( (client_fd = accept_client(socket_fd, client_addr_str)) == -1)
            continue;


        if ( (clild_pid =  fork()) == -1)
        {
            close(client_fd);
            // 保存消息记录
            fp = fopen("./message_record.dat", "rb+");
            fwrite(pack_ptr, sizeof(groupMsg_package), 1, fp);
            fseek(fp, 0, SEEK_END);
            fwrite(pack_ptr + previous_MsgID +1, sizeof(groupMsg_package), get_MsgID(pack_ptr, sem_id) - previous_MsgID, fp);
            fclose(fp);

            default_handler();

            shmdt(pack_ptr);  // 解除共享内存 绑定
            del_sem_set(sem_id);
            del_shemem(shm_id);

            err_exit("fork error");
        }
        else if (clild_pid == 0)  //clild poccess
        {
            close(socket_fd);

            if (Signal(SIGUSR1, sigusr1_handler) == SIG_ERR)    //忽略SIGUSR1
                    perror("set signal SIGUSR1 handler");

            // 连接master
            master_fd = connect_master(MASTER_UNIXPATH);

            // 创建epoll句柄
            if ((epfd = epoll_create(EPOLL_SIZE)) == -1 )  // 4
                err_exit("epoll create error");

            epoll_event events[EPOLL_SIZE];
            epoll_add_event(epfd, master_fd);
            epoll_add_event(epfd, client_fd);


            while(true)  // true
            {
                ready_num = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_TIMEOUT);  // -1

                if (ready_num == -1)  // epoll_wait 会被信号处理程序中断 返回 -1
                {
                        continue;
                }

                for (i = 0; i < ready_num; ++i)
                {
                    if (events[i].data.fd == master_fd)  // master 返回数据  私聊数据
                    {
                        if ((n = parse_pack(master_fd,packet_tmp)) == -1)
                        {
                            close(master_fd);
                            close(client_fd);
                            shmdt(pack_ptr);  // 解除共享内存 绑定
                            err_exit("recv massage error");
                        }
                        else if (n == 0)    // 对方关闭连接
                        {
                            ticks = time(NULL);
                            printf("at %.24s\t exit connection with %s\n", ctime(&ticks), client_addr_str );
                            close(master_fd);
                            close(client_fd);
                            shmdt(pack_ptr);  // 解除共享内存 绑定
                            exit(EXIT_SUCCESS);
                            break;
                        }

                        if (packet_tmp.package_type == PRIVATEMSG)
                        {
                            build_pack(client_fd, packet_tmp.data.privateMsg_pak);  // 返回发送状态
                        }

                    }

                    else if (events[i].data.fd == client_fd)  // 客户端发来数据
                    {
                        if ((n = parse_pack(client_fd,packet_tmp)) == -1)
                        {
                            close(client_fd);
                            close(master_fd);
                            shmdt(pack_ptr);  // 解除共享内存 绑定
                            err_exit("recv massage error");
                        }
                        else if (n == 0)    // 对方关闭连接
                        {
                            ticks = time(NULL);
                            printf("at %.24s\t exit connection with worker\n", ctime(&ticks));
                            close(master_fd);
                            close(client_fd);
                            shmdt(pack_ptr);  // 解除共享内存 绑定
                            exit(EXIT_SUCCESS);
                            break;
                        }


                        if (packet_tmp.package_type == GROUPMSG)
                        {

                            message_buff = packet_tmp.data.groupMsg_pak;
                            if (message_buff.MsgID = 0)


                            // 填充消息包  并写入消息记录共享内存
                            get_time(message_buff.time_str, sizeof(message_buff.time_str));
                            printf("%s %s USER: %s\n##  send: %s\n", client_addr_str, message_buff.time_str, message_buff.identity, message_buff.massage);  // 服务器日志
                            write_packet(pack_ptr, sem_id, message_buff);  // 写入消息记录共享内存  并且更新message_buff中的MsgID

                            block_specified_signals(&oset);
                            if (kill(0, SIGUSR1) == -1)  // 通知推送消息
                                perror("Kill send signal");

                            unblock_all_signals(&oset);
                        }

                        else if (packet_tmp.package_type == PRIVATEMSG)
                        {
                            build_pack(master_fd, packet_tmp.data.privateMsg_pak);
                            parse_pack(master_fd, packet_tmp);
                            if (packet_tmp.package_type == BACKDATA)
                                build_pack(client_fd, packet_tmp.data.backdata_pak);
                        }

                        else if (packet_tmp.package_type == LOGIN)
                        {
                            build_pack(master_fd, packet_tmp.data.login_pak);
                            parse_pack(master_fd, packet_tmp);
                            if (packet_tmp.package_type == BACKDATA)
                                build_pack(client_fd, packet_tmp.data.backdata_pak);
                            if (packet_tmp.data.backdata_pak.data_1 == -1)
                                printf("login failed\n");
                            else if (packet_tmp.data.backdata_pak.data_1 == 0)
                                printf("login success\n");
                        }

                        else if (packet_tmp.package_type == LOGOUT)
                        {
                            build_pack(master_fd, packet_tmp.data.logout_pak);  // 转发
                        }

                        else if (packet_tmp.package_type == BACKDATA)
                        {
                            if (packet_tmp.data.backdata_pak.data_2 == -666)  // 群聊同步消息请求
                            {
                                latest_ID = get_MsgID(pack_ptr, sem_id);  // 获取当前最新的消息标号
                                build_pack(master_fd, packet_tmp.data.backdata_pak);  // 转发群聊消息记录同步请求
                                parse_pack(master_fd, packet_tmp);

                                // 对客户端群聊进行消息同步
                                unsigned int record_ID = packet_tmp.data.backdata_pak.data_5;
                                for (unsigned int t = record_ID + 1; t <= latest_ID; t++)
                                {
                                    get_packet(pack_ptr, sem_id, t, message_buff);
                                    build_pack(client_fd, message_buff);
                                    usleep(100 *1000);    // 挂起100毫秒
                                }
printf("~synchronization finished.\n");  // 同步完成
                            }
                            else if (packet_tmp.data.backdata_pak.data_2 == -888) // 获取在线用户列表 -888
                            {
                                build_pack(master_fd, packet_tmp.data.backdata_pak);
                                parse_pack(master_fd, packet_tmp);

                                if (packet_tmp.data.backdata_pak.data_1 == -1)  // 无其他在线人员
                                    build_pack(client_fd, packet_tmp.data.backdata_pak);
                                else
                                {
                                    build_pack(client_fd, packet_tmp.data.backdata_pak);
                                    while(packet_tmp.data.backdata_pak.data_1 > 0)
                                    {
                                        parse_pack(master_fd, packet_tmp);
                                        build_pack(client_fd, packet_tmp.data.backdata_pak);
                                    }
                                }
                            }
                        }
                        else if (packet_tmp.package_type == MODPWD)
                        {
                            build_pack(master_fd, packet_tmp.data.modPwd_pak);
                            parse_pack(master_fd, packet_tmp);
                            if (packet_tmp.package_type == BACKDATA)
                                build_pack(client_fd, packet_tmp.data.backdata_pak);
                        }
                        else if (packet_tmp.package_type == REGISTER)
                        {
                            build_pack(master_fd, packet_tmp.data.register_pak);  //中继转发
                            parse_pack(master_fd, packet_tmp);  // 接收注册结果
                            if (packet_tmp.package_type == BACKDATA)
                                build_pack(client_fd, packet_tmp.data.backdata_pak);  //  回送注册状态
                        }
                    }

                    else if (events[i].events == EPOLLHUP)  // 断开
                    {
                        close(events[i].data.fd);
                        epoll_remove_event(epfd, events[i].data.fd);
                    }

                    else
                        continue;
                }

            }
            shmdt(pack_ptr);  // 解除共享内存 绑定
            exit(EXIT_SUCCESS);
        }
        else  //parent poccess
        {
            close(client_fd);
        }
    }

    // 保存消息记录
    fp = fopen("./message_record.dat", "rb+");
    fwrite(pack_ptr, sizeof(groupMsg_package), 1, fp);
    fseek(fp, 0, SEEK_END);
    fwrite(pack_ptr + previous_MsgID +1, sizeof(groupMsg_package), get_MsgID(pack_ptr, sem_id) - previous_MsgID, fp);
    fclose(fp);

    default_handler();

    del_sem_set(sem_id);
    del_shemem(shm_id);

    exit(EXIT_SUCCESS);
}
