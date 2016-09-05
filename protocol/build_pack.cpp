#include "protocol.h"

// 处理 发送传送包
// 重载函数  将不同的数据包进行处理封装成统一发送包的格式并进行发送
// 参数  #int 发送目的地的套接字  #数据包常量引用
// 返回值 #int 发送状态  返回值与 系统调用send 相同
//template <typename PACKTYPE>  //使用泛型  但是连接出错

int build_pack(int socket_fd, const register_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = REGISTER;
    pack_buff.data.register_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(register_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const login_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = LOGIN;
    pack_buff.data.login_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(login_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const logout_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = LOGOUT;
    pack_buff.data.logout_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(logout_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const modPwd_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = MODPWD;
    pack_buff.data.modPwd_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(modPwd_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const groupMsg_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = GROUPMSG;
    pack_buff.data.groupMsg_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(groupMsg_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const privateMsg_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = PRIVATEMSG;
    pack_buff.data.privateMsg_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(privateMsg_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const fileShare_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = FILESHARE;
    pack_buff.data.fileShare_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(fileShare_package) + sizeof(pack_buff.package_type), 0);
}

int build_pack(int socket_fd, const backdata_package& pack)
{
    static packet pack_buff;
    pack_buff.package_type = BACKDATA;
    pack_buff.data.backdata_pak = pack;
    return sendn(socket_fd, &pack_buff, sizeof(backdata_package) + sizeof(pack_buff.package_type), 0);
}

