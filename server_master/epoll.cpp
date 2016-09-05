#include "epoll.h"

// 加入事件
// 参数  #epoll句柄    #文件描述符
void epoll_add_event(int epfd, int fd)
{
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        err_exit("epoll add event failed!");

    return;
}



// 删除事件
// 参数  #epoll句柄    #文件描述符
void epoll_remove_event(int epfd, int fd)
{
    if(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        err_exit("epoll remove event failed!");

    return;
}


