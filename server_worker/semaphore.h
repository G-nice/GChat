#ifndef GUARD_SEMAPHORE_MY_H
#define GUARD_SEMAPHORE_MY_H


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#include "g_lib.h"

// TODO 将其封装成一个类比较合适  不用带着semid作为每一个参数

#define MUTEX 0      // 读写互斥信号量
#define PREWMUTEX 1   // 准备写信号量
#define READERNUM 2   // 读者数量限制



union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *_buf;
};

// 创建信号量集  返回信号量标识符
// 参数同系统调用 semget
// 返回信号量标识符
int Semget(key_t key, int nsems, int semflg);

// 设置信号量的值
// 参数  #int sem_id 信号量标识符  #int semnum 信号量在信号量集中的位置 从0开始  #int val 要设置的值
void set_semvalue(int semid, int semnum, int val);

// 返回具体信号量的值
// 参数  #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
// 返回 int 信号量的值
int get_semvalue(int semid, int semnum);

// 进行信号量等待 wait  -1
// 参数 #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
void semaphore_p(int semid, int semnum);

// 进行信号量释放 signal  +1
// 参数 #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
void semaphore_v(int semid, int semnum);

// 等待信号量值变为0
// 参数 #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
void semaphore_z(int semid, int semnum);



// 删除信号量
// 参数 #int sem_id 信号量标识符
void del_sem_set(int semid);






#endif // GUARD_SEMAPHORE_MY_H
