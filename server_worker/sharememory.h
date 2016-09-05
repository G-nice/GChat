#ifndef GUARD_SHAREMEMORY_H
#define GUARD_SHAREMEMORY_H


#include <sys/ipc.h>
#include <sys/shm.h>

#include "semaphore.h"

// TODO: 其实将共享内存封装成一个类更容易进行管理以及使用  调用函数也不需要这么多的参数



// 创建共享内存  返回共享内存标识符
// 参数同系统调用 shmget
// 返回共享内存标识符
int Shmget(key_t key, size_t size, int shmflg);


// 删除共享内存
// 参数  共享内存标识符
void del_shemem(int shmid);








#endif // GUARD_SHAREMEMORY_H
