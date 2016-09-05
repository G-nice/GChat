#ifndef GUARD_EPOLL_H
#define GUARD_EPOLL_H

#include <sys/epoll.h>

#include "g_lib.h"

#define EPOLL_SIZE 4
#define EPOLL_TIMEOUT -1

// 加入事件
// 参数  #epoll句柄    #文件描述符
void epoll_add_event(int epfd, int fd);


// 删除事件
// 参数  #epoll句柄    #文件描述符
void epoll_remove_event(int epfd, int fd);


#endif // GUARD_EPOLL_H

