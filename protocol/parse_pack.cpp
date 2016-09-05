#include "protocol.h"

// 接收  解析传送包
// 泛型函数  将接收到的包按照参数的类型进行解析取出数据
// 参数  #int 接收源的套接字  #数据包引用
// 返回值 #int 接收状态 返回值与 系统调用recv 相同
//template <typename PACKTYPE>
//int parse_pack(int socket_fd, PACKTYPE& pack)
int parse_pack(int socket_fd, packet& pack)
{
    memset(&pack, 0, sizeof(packet));
    recvn(socket_fd, &pack.package_type, sizeof(pack.package_type), 0);    // 获取数据包类型

    switch(pack.package_type)
    {
    case CLIENTEXIT:
        return CLIENTEXIT;  // 0
        break;
    case REGISTER:
        return recvn(socket_fd, &pack.data.register_pak, sizeof(register_package), 0);
        break;
    case LOGIN:
        return recvn(socket_fd, &pack.data.login_pak, sizeof(login_package), 0);
        break;
    case LOGOUT:
        return recvn(socket_fd, &pack.data.logout_pak, sizeof(logout_package), 0);
        break;
    case MODPWD:
        return recvn(socket_fd, &pack.data.modPwd_pak, sizeof(modPwd_package), 0);
        break;
    case GROUPMSG:
        return recvn(socket_fd, &pack.data.groupMsg_pak, sizeof(groupMsg_package), 0);
        break;
    case PRIVATEMSG:
        return recvn(socket_fd, &pack.data.privateMsg_pak, sizeof(privateMsg_package), 0);
        break;
    case FILESHARE:
        return recvn(socket_fd, &pack.data.fileShare_pak, sizeof(fileShare_package), 0);
        break;
    case BACKDATA:
        return recvn(socket_fd, &pack.data.backdata_pak, sizeof(backdata_package), 0);
        break;
    default:
printf("~packet protocol type error.\n");
        return -1;
    }
    return 0;
}

