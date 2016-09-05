#ifndef GUARD_PROTOCOL_H
#define GUARD_PROTOCOL_H

//#include <typeinfo>

#include "g_lib.h"

// 通讯协议

// 数据包类型
//#define 0
#define CLIENTEXIT  0
#define REGISTER    1
#define LOGIN       2
#define LOGOUT      3
#define MODPWD      4
#define GROUPMSG    5
#define PRIVATEMSG  6
#define FILESHARE   7
#define BACKDATA    8

//#define
//#define

// 状态码
#define REGISTER_SUCCESS  101
#define REGISTER_FAIL     102
#define LOGIN_SUCCESS     103
#define LOGIN_FAIL        104
//#define
//#define

// 注册用户
typedef struct register_package
{
    char user[24];
    char password[20];  // 支持16位密码
} register_package;

// 登陆
typedef struct login_package
{
    char user[24];
    char password[20];  // 支持16位密码
} login_package;

// 登出
typedef struct logout_package
{
    char user[24];
    unsigned int MsgID_record;
} logout_package;

// 修改用户密码
typedef struct modPwd_package
{
    char user[24];
    char previous_password[20];  // 支持16位密码
    char new_password[20];  // 支持16位密码
} modPwd_package;

// 群聊消息包
typedef struct groupMsg_package
{
    unsigned int MsgID;
    char identity[24];
    char time_str[24];
    char massage[76];
} groupMsg_package;

// 私聊消息包
typedef struct privateMsg_package
{
    unsigned int MsgID;
    char identity[24];
    char destination[24];
    char time_str[24];
    char massage[76];
} privateMsg_package;

// 文件共享包
typedef struct fileShare_package
{
    unsigned int piece_id;
    char data[124];
} fileShare_package;

// 返回数据以及其他数据传送
typedef struct backdata_package
{
    int data_1;
    int data_2;
    int data_3;
    int data_4;
    unsigned int data_5;
    unsigned int data_6;
    char msg_1[32];
    char msg_2[32];

} backdata_package;

union uniondata
{
    register_package    register_pak;
    login_package       login_pak;
    logout_package      logout_pak;
    modPwd_package      modPwd_pak;
    groupMsg_package    groupMsg_pak;
    privateMsg_package  privateMsg_pak;
    fileShare_package   fileShare_pak;
    backdata_package    backdata_pak;
};

// 统一传送包
typedef struct packet
{
    unsigned int package_type;
    union uniondata data;

} packet;


// 处理 发送传送包
// 重载函数  将不同的数据包进行处理封装成统一发送包的格式并进行发送
// 参数  #int 发送目的地的套接字  #数据包常量引用
// 返回值 #int 发送状态  返回值与 系统调用send 相同
//template <typename PACKTYPE>  //使用泛型  但是连接出错
int build_pack(int socket_fd, const register_package& pack);
int build_pack(int socket_fd, const login_package& pack);
int build_pack(int socket_fd, const logout_package& pack);
int build_pack(int socket_fd, const modPwd_package& pack);
int build_pack(int socket_fd, const groupMsg_package& pack);
int build_pack(int socket_fd, const privateMsg_package& pack);
int build_pack(int socket_fd, const fileShare_package& pack);
int build_pack(int socket_fd, const backdata_package& pack);



// 接收  解析传送包
// 重载函数  将接收到的包按照参数的类型进行解析取出数据
// 参数  #int 接收源的套接字  #数据包引用
// 返回值 #int 接收状态 返回值与 系统调用recv 相同
int parse_pack(int socket_fd, packet& pack);
//int parse_pack(int socket_fd, register_package& pack);
//int parse_pack(int socket_fd, login_package& pack);
//int parse_pack(int socket_fd, logout_package& pack);
//int parse_pack(int socket_fd, modPwd_package& pack);
//int parse_pack(int socket_fd, groupMsg_package& pack);
//int parse_pack(int socket_fd, privateMsg_package& pack);
//int parse_pack(int socket_fd, fileShare_package& pack);


#endif // GUARD_PROTOCOL_H

