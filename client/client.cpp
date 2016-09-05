#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <vector>
#include <string>


#define BUFFSIZE 4196


#include "g_lib.h"
#include "protocol.h"
#include "signal_handle.h"
#include "epoll.h"


using std::vector;
using std::string;


int main(int argc, char* argv[])
{
    int server_fd;    //, rec_len = 0;
    int latest_ID = 0;  // 上次浏览的最后消息记录标号
    char buff[BUFFSIZE];
    //char identity[32];
    packet pack_buff;
    groupMsg_package groupMsg_send, groupMsg_recv;
    privateMsg_package privateMsg_send, privateMsg_recv;

    char username[sizeof(groupMsg_send.identity)];  // 保存用户姓名
    char privname[sizeof(privateMsg_send.destination)];  // 保存用户姓名

    memset(&groupMsg_send, 0, sizeof(groupMsg_package));
    memset(&groupMsg_recv, 0, sizeof(groupMsg_package));

    sighandler_t sh;

    int epfd;  // epoll句柄
    int ready_num;

    static int n = 0;  //parse_pack 记数
    int menu_select = 0;  // 菜单选项
    int chat_modle = 0;  // 群聊 私聊 工作模式

    // 错误参数  使用方法提示
    if (argc != 3)
    {
        fprintf(stderr, "Usage: chat_client <IP> <PORT>\n");
        exit(EXIT_SUCCESS);
    }

    // 连接服务器
    server_fd = connect_server(argv[1], argv[2]);

    // 创建epoll句柄
    if ((epfd = epoll_create(EPOLL_SIZE)) == -1 )  // 4
        err_exit("epoll create error");

    epoll_event events[EPOLL_SIZE];
    epoll_add_event(epfd, STDIN_FILENO);
    epoll_add_event(epfd, server_fd);

//    printf("                        Welocme to GChat\n");
    printf("\n");
    printf("                      ██████╗  ██████╗██╗  ██╗ █████╗ ████████╗\n");
    printf("                     ██╔════╝ ██╔════╝██║  ██║██╔══██╗╚══██╔══╝\n");
    printf("                     ██║  ███╗██║     ███████║███████║   ██║   \n");
    printf("        Welcome to   ██║   ██║██║     ██╔══██║██╔══██║   ██║   \n");
    printf("                     ╚██████╔╝╚██████╗██║  ██║██║  ██║   ██║   \n");
    printf("                      ╚═════╝  ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   \n");


firstMenu:
    while (true)
    {
        printf("    1. Register Account\n    2. Login\n    3. Modify Password\n    4. Exit\n");
        printf("Your seletion: ");
        fscanf(stdin, "%d", &menu_select);
        getchar(); // 接收回车

        if (menu_select == 1)  // 注册  自带登陆
        {
            register_package register_pack;
            while(true)
            {
                // 询问用户昵称
                printf("Please input your nickname: ");
                fgets(register_pack.user, sizeof(register_pack.user), stdin);
                register_pack.user[strlen(register_pack.user) - 1] = '\0';  //去除结尾回车 \n
                printf("Please input your password(Maximum to 16 bit): ");
                fgets(register_pack.password, sizeof(register_pack.password), stdin);
                register_pack.password[strlen(register_pack.password) - 1] = '\0';  //去除结尾回车 \n

                build_pack(server_fd, register_pack);
                parse_pack(server_fd, pack_buff);

                if (pack_buff.package_type == BACKDATA)
                {
                    if (pack_buff.data.backdata_pak.data_1 == -1)  // 注册不成功
                        printf("The nickname has been used.\nPlease try again.\n");
                    else  // 注册成功
                    {
                        printf("Register success.\n");
                        strncpy(username, register_pack.user, sizeof(username));
                        goto secondMenu;
                    }
                }
            }
        }
        else if (menu_select == 2)  // 登陆
        {
            login_package login_pack;
            for (int i = 2; i >= 0; i--)  // 三次机会
            {
                printf("Please input your nickname: ");
                fgets(login_pack.user, sizeof(login_pack.user), stdin);
                login_pack.user[strlen(login_pack.user) - 1] = '\0';  //去除结尾回车 \n
                printf("Please input your password(Maximum to 16 bit): ");
                fgets(login_pack.password, sizeof(login_pack.password), stdin);
                login_pack.password[strlen(login_pack.password) - 1] = '\0';  //去除结尾回车 \n

                build_pack(server_fd, login_pack);
                parse_pack(server_fd, pack_buff);

                if (pack_buff.package_type == BACKDATA)
                {
                    if (pack_buff.data.backdata_pak.data_1 == -1)
                        printf("Wrong password or incorrect user nickname. Please try again(still has %d time(s))\n", i);
                    else if (pack_buff.data.backdata_pak.data_1 == 0)
                    {
                        strncpy(username, login_pack.user, sizeof(username));
                        printf("Login success.\n");
                        goto secondMenu;
                    }
                }
                if (i == 0)  // 用光密码尝试机会
                    exit(EXIT_FAILURE);
            }
        }
        else if (menu_select == 3) // 修改密码  修改后需要重新使用新密码登陆
        {
            modPwd_package modPwd_pack;

           for (int i = 2; i >= 0; i++)  // 三次机会
            {
                printf("Please input your nickname: ");
                fgets(modPwd_pack.user, sizeof(modPwd_pack.user), stdin);
                modPwd_pack.user[strlen(modPwd_pack.user) - 1] = '\0';  //去除结尾回车 \n
                printf("Please input your previous password: ");
                fgets(modPwd_pack.previous_password, sizeof(modPwd_pack.previous_password), stdin);
                modPwd_pack.previous_password[strlen(modPwd_pack.previous_password) - 1] = '\0';  //去除结尾回车 \n
                printf("Please input your new password(Maximum to 16 bit): ");
                fgets(modPwd_pack.new_password, sizeof(modPwd_pack.new_password), stdin);
                modPwd_pack.new_password[strlen(modPwd_pack.new_password) - 1] = '\0';  //去除结尾回车 \n

                build_pack(server_fd, modPwd_pack);
                parse_pack(server_fd, pack_buff);

                if (pack_buff.package_type == BACKDATA)
                {
                    if (pack_buff.data.backdata_pak.data_1 == -1)
                        printf("Wrong password. Please try again(still has %d time(s))\n", i);
                    else
                    {
                        printf("modify password success.\n");
                        goto firstMenu;
                    }
                }
                if (i == 0)  // 用光密码尝试机会
                    exit(EXIT_FAILURE);
            }
        }
        else if (menu_select == 4) // 登出
        {
            return EXIT_SUCCESS;
        }
        else
        {
            printf("Input seletion error.\n");
            continue;
        }
    }


secondMenu:
    printf("    1. Group chat\n    2. Private chat\n    3. Exit\n");
    while (true)
    {
        printf("Your seletion: ");
        fscanf(stdin, "%d", &menu_select);
        getchar(); // 接收回车

        if (menu_select == 1)
        {
            backdata_package backdata_pack;
            chat_modle = GROUPMSG;
            strncpy(groupMsg_send.identity, username, sizeof(groupMsg_recv.identity) );  // 填写用户名
            backdata_pack.data_2 = -666; // 发送同步消息记录请求   -666 为消息同步请求  放在data2
            strncpy(backdata_pack.msg_1, username, sizeof(username) );
            build_pack(server_fd, backdata_pack);
            printf("Enjoy group chat now.\n(type \"//exit\" to exit, type \"//back\" to back to previous menu.)\n");
            break;
        }
        else if (menu_select == 2)
        {
            chat_modle = PRIVATEMSG;
            backdata_package backdata_pack;
            packet pack;
            vector<string> online_user_list;
            int online_amount = 0;
            int take_mun = 0;  // 该次要取的数据条数 mag_1 & msg_2

            strncpy(privateMsg_send.identity, username, sizeof(username));
            // 请求列出在线用户列表
            strncpy(backdata_pack.msg_1, username, sizeof(username));
            backdata_pack.data_2 = -888;
            build_pack(server_fd, backdata_pack);

            // 接收第一个在线用户列表数据包
            parse_pack(server_fd, pack);
            if(pack.package_type == BACKDATA)
            {
                if (pack.data.backdata_pak.data_1 == -1)  // 无其他在线人员
                {
                    printf("No other online user.\n");
                    chat_modle = 0;
                    goto secondMenu;
                }
                else
                {
                    online_amount = pack.data.backdata_pak.data_3;
                    take_mun = pack.data.backdata_pak.data_1;

                    online_user_list.push_back(string(pack.data.backdata_pak.msg_1));
                    if (online_amount%2 == 0)  // 至少有两个
                        online_user_list.push_back(string(pack.data.backdata_pak.msg_2));

                    while(take_mun > 0)  // data_1 表示还有点数据量
                    {
                        parse_pack(server_fd, pack);
                        if (pack.package_type == BACKDATA)
                        {
                            online_user_list.push_back(string(pack.data.backdata_pak.msg_1));
                            if (take_mun != 1)
                                online_user_list.push_back(string(pack.data.backdata_pak.msg_2));
                            take_mun = pack.data.backdata_pak.data_1;
                        }
                    }
                }
            }

            for (int i = 0; i < online_user_list.size(); i++)
            {
                printf("  %d: %s\n", i, online_user_list[i].c_str());
            }

            unsigned int select_user = 0;
            printf("select the online user: \n");
            fscanf(stdin, "%u", &select_user);
            getchar();  //取走回车
            while (select_user > (unsigned int)online_user_list.size())
            {
                printf("error seletion. selete again\n");
                fscanf(stdin, "%u", &select_user);
                getchar();  //取走回车

            }
            strncpy(privname, online_user_list[select_user].c_str(), sizeof(privname));
            strncpy(privateMsg_send.destination, privname, sizeof(privateMsg_send.destination));

            printf("Chat with %s now.\n(type \"//exit\" to exit, type \"//back\" to back to previous menu.)\n", privname);
            break;
        }
        else if (menu_select == 3)
        {
            logout_package logout_pack;
            strncpy( logout_pack.user, username, sizeof(username) );
            logout_pack.MsgID_record = latest_ID;
            build_pack(server_fd, logout_pack);
            usleep(100 *1000);  // 100毫秒
            return EXIT_SUCCESS;
        }
        else
        {
            printf("Input seletion error.\n");
            continue;
        }
    }

chatLoop:


    static int i;    // epoll 循环变量
    static int nbytes = 0;    // read 读取输入缓冲字节数

    while (true)
    {
        ready_num = epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_TIMEOUT);  // -1

        if (ready_num == -1)
        {
            close(server_fd);
            close(epfd);
            err_exit("epoll_wait error");
        }

        for (i = 0; i < ready_num; ++i)
        {
            // 输入就绪
            if (events[i].data.fd == STDIN_FILENO)
            {
                nbytes = read(STDIN_FILENO, buff, BUFFSIZE);
                if (nbytes == -1 || nbytes == 0)  // read 出错 或 读取到文件末尾
                    continue;

                buff[nbytes] = '\0';
                if (strcmp("//exit\n", buff) == 0)  // 退出客户端程序  登出
                {
                    logout_package logout_pack;
                    strncpy( logout_pack.user, username, sizeof(username) );
                    logout_pack.MsgID_record = latest_ID;
                    build_pack(server_fd, logout_pack);
                    usleep(100 *1000);  // 100毫秒
                    close(server_fd);
                    return EXIT_SUCCESS;
                }
                if (strcmp("//back\n", buff) == 0)  //返回上一级菜单 选择私聊以及群聊
                {
                    chat_modle = 0;  //重置聊天模式
                    goto secondMenu;
                }

                if (chat_modle == GROUPMSG)  // 发送群聊消息
                {
                    // 填写时间戳
                    get_time(groupMsg_send.time_str, sizeof(groupMsg_send.time_str));
                    memset(groupMsg_send.massage, 0, sizeof(groupMsg_send.massage));
                    strncpy(groupMsg_send.massage, buff, sizeof(groupMsg_send.massage));    //strcpy  TO <-- from
                    groupMsg_send.massage[strlen(groupMsg_send.massage) - 1] = '\0';  //去除结尾回车 \n

                    sh = Signal(SIGPIPE, SIG_IGN);    //忽略SIGPIPE
                    if (build_pack(server_fd, groupMsg_send) == -1 )
                    {
                        Signal(SIGPIPE, sh);  //恢复SIGPIPE
                        err_exit("send message error");
                    }
                    Signal(SIGPIPE, sh);  //恢复SIGPIPE
                    latest_ID++;  //最新消息号自增
                }
                else if (chat_modle == PRIVATEMSG)  // 发送私聊消息
                {
                    get_time(privateMsg_send.time_str, sizeof(privateMsg_send.time_str));
                    memset(privateMsg_send.massage, 0, sizeof(privateMsg_send.massage));
                    strncpy(privateMsg_send.massage, buff, sizeof(privateMsg_send.massage));    //strcpy  TO <-- from
                    privateMsg_send.massage[strlen(privateMsg_send.massage) - 1] = '\0';  //去除结尾回车 \n


                    sh = Signal(SIGPIPE, SIG_IGN);    //忽略SIGPIPE
                    if (build_pack(server_fd, privateMsg_send) == -1 )
                    {
                        Signal(SIGPIPE, sh);  //恢复SIGPIPE
                        err_exit("send message error");
                    }
                    Signal(SIGPIPE, sh);  //恢复SIGPIPE

                    parse_pack(server_fd, pack_buff);
                    if (pack_buff.package_type == BACKDATA)
                    {
                        if (pack_buff.data.backdata_pak.data_1 == -1)  // 对方已下线  发送失败
                        {
                            printf("%s han been offline, message send failed.\n", privname);
                            goto secondMenu;
                        }
                        else
                        {
                            fprintf(stdout, "#%s\t%s TO %s\n", privateMsg_send.time_str, privateMsg_send.identity, privateMsg_send.destination);
                            fprintf(stdout, "  %s\n", privateMsg_send.massage);
//                            printf("send message success\n");
                        }
                    }
                }
            }
            // 有消息推送
            else if (events[i].data.fd == server_fd)
            {
                if ( (n = parse_pack(server_fd, pack_buff)) == -1)
                {
                    perror("recv message error");
                    exit(EXIT_FAILURE);
                }
                else if (n == 0)
                {
                    fprintf(stderr, "server has closed.\n");
                    close(server_fd);
                    logout_package logout_pack;
                    strncpy( logout_pack.user, username, sizeof(username) );
                    logout_pack.MsgID_record = latest_ID;
                    build_pack(server_fd, logout_pack);
                    usleep(100 *1000);  // 100毫秒
                    return EXIT_SUCCESS;
                }
                if (pack_buff.package_type == GROUPMSG)
                {
                    if (chat_modle == GROUPMSG)  // 处于群聊模式接收到群聊信息
                    {
                        fprintf(stdout, "#%s\t%s\n", pack_buff.data.groupMsg_pak.time_str, pack_buff.data.groupMsg_pak.identity);
                        fprintf(stdout, "  %s\n", pack_buff.data.groupMsg_pak.massage);
                        // 更新消息号码
                        latest_ID = pack_buff.data.groupMsg_pak.MsgID;
                    }
                    else if(chat_modle == PRIVATEMSG)  // 处于群聊模式接收到私聊消息
                    {
                        printf("## you receive a new message in group chat.  ##\n");  // 提示处于私聊时候接收到一条群聊消息
                        // 缓存私聊消息
//                        FILE* fp;
//                        if (access("./groupchat_tmp.dat", F_OK) == 0)
//                        {
//                            fp = fopen("./groupchat_tmp.dat", "rb+");
//                            fread(&MsgID_record, sizeof(int), 1, fp);
//                            rewind(fp);  // 读写指针重置到文件起始位置
//                        }
//                        else
//                        {
//                            fp = fopen("./groupchat_tmp.dat", "wb");
//                        }
//                        send_int32(server_fd, MsgID_record);
                    }
                }
                else if(pack_buff.package_type = PRIVATEMSG)
                {
                    if (chat_modle == PRIVATEMSG)
                    {
                        if(strcmp(privname, pack_buff.data.privateMsg_pak.identity) == 0)  // 收到别人发来的私聊消息
                        {
                            fprintf(stdout, "#%s\t%s TO %s\n", pack_buff.data.privateMsg_pak.time_str, pack_buff.data.privateMsg_pak.identity, pack_buff.data.privateMsg_pak.destination);
                            fprintf(stdout, "  %s\n", pack_buff.data.privateMsg_pak.massage);
                        }
                        else // 收到当前私聊用户消息
                        {
                            printf("## you receive a new message from %s.  ##\n", pack_buff.data.privateMsg_pak.identity);
                        }
                    }
                    else if (chat_modle == GROUPMSG)  // zai群聊中收到私聊消息
                    {
                        printf("## you receive a new message from %s.  ##\n", pack_buff.data.privateMsg_pak.identity);
                    }


                }
            }
        }
    }

    close(server_fd);
    logout_package logout_pack;
    strncpy( logout_pack.user, username, sizeof(username) );
    logout_pack.MsgID_record = latest_ID;
    build_pack(server_fd, logout_pack);
    usleep(100 *1000);  // 100毫秒
    return EXIT_SUCCESS;
}
