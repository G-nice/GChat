#include "sharememory.h"

// TODO: 其实将共享内存封装成一个类更容易进行管理以及使用  调用函数也不需要这么多的参数

int shm_id;    // 共享内存标识符

extern int sem_id;


int Shmget(key_t key, size_t size, int shmflg)
{
    int tmp;
//    int ttmp = sem_id;
    if ((tmp = shmget(key, size, shmflg)) == -1)
    {
    // TODO: bug fix
//        del_sem_set(sem_id);    // 会出现 ld returned 1 exit
//        del_sem_set(ttmp);
        err_exit("failed to create sharememory");
    }
    return tmp;
}

// 删除共享内存
// 参数  共享内存标识符
void del_shemem(int shmid)
{
    if (shmctl(shmid, IPC_RMID, 0) == -1)
        err_exit("delete share memory failed");
    return;
}

//  pData = (char*)shmat(shm_id, 0, 0);
//  shmdt(pData);




