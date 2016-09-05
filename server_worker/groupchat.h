#ifndef GUARD_PACKET_H
#define GUARD_PACKET_H

#include "g_lib.h"
#include "protocol.h"
#include "sharememory.h"
#include "signal_handle.h"

// 通讯协议

//// 群聊消息包
//typedef struct
//{
//    unsigned int MsgID;
//    char identity[24];
//    char time_str[24];
//    char massage[76];
//} groupMsg_package;


// 发送数据包
// 参数  #int:目的地套接字   数据包常量引用
// 返回值为  发送状态  -1 发送失败
int send_packet(int sockfd, const groupMsg_package& pac);

// 接收数据包
// 参数 #int 接收源的套接字
// 返回接收状态
int recv_packet(int sockfd, groupMsg_package& pac);


// 获取当前消息记录的最新MsgID
// 参数 #groupMsg_package* 共享内存首地址  #int 信号量集标识符
unsigned int get_MsgID(groupMsg_package* pack_ptr , int semid);

// 获取一个数据包
// 参数 #groupMsg_package* 共享内存首地址  #int 信号量集标识符    #int 数据包索引  从1开始  #groupMsg_package& 数据包引用
// packnum 为0 时候默认获取最新的消息包
void get_packet(groupMsg_package* pack_ptr, int semid, unsigned int packnum, groupMsg_package& pack);

// 写入一个数据包
// 参数 #groupMsg_package* 共享内存首地址  #int 信号量集标识符   #groupMsg_package& 数据包常量引用
void write_packet(groupMsg_package* pack_ptr, int semid, groupMsg_package& pack);

#endif // GUARD_PACKET_H
