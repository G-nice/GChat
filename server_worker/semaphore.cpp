#include "semaphore.h"

int sem_id;    //信号量标识符

// 创建信号量  返回信号量标识符
// 参数同系统调用 semget
// 返回信号量标识符
int Semget(key_t key, int nsems, int semflg)
{
    int tmp;
    if ((tmp = semget(key, nsems, semflg)) == -1 )
        err_exit("create semaphore set error");
    return tmp;
}


// 设置信号量的值
// 参数  #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始  #int val 要设置的值
void set_semvalue(int semid, int semnum, int val)
{
    union semun sem_union;

    sem_union.val = val;
    if(semctl(semid, semnum, SETVAL, sem_union) == -1)
        err_exit("set semaphore value error");
    return;
}

// 返回具体信号量的值
// 参数  #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
// 返回 int 信号量的值
int get_semvalue(int semid, int semnum)
{
    int semval;
    union semun sem_union;
    if(( semval = semctl(semid, semnum, GETVAL, sem_union)) == -1)
        err_exit("get semaphore value error");

    return semval;
}

// 进行信号量等待 wait
// 参数 #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
void semaphore_p(int semid, int semnum)
{
    struct sembuf sem_b;

    sem_b.sem_num = semnum;
    sem_b.sem_op = -1;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(semid, &sem_b, 1) == -1)
        err_exit("semaphore_p failed");
    return;
}


// 进行信号量释放 signal
// 参数 #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
void semaphore_v(int semid, int semnum)
{
    struct sembuf sem_b;

    sem_b.sem_num = semnum;
    sem_b.sem_op = +1;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(semid, &sem_b, 1) == -1)
        err_exit("semaphore_v failed");
    return;
}

// 等待信号量值变为0
// 参数 #int sem_id 信号量标识符  #int NO 信号量在信号量集中的位置 从0开始
void semaphore_z(int semid, int semnum)
{
    struct sembuf sem_b;

    sem_b.sem_num = semnum;
    sem_b.sem_op = 0;
    sem_b.sem_flg = SEM_UNDO;
    if(semop(semid, &sem_b, 1) == -1)
        err_exit("semaphore_z failed");
    return;
}




// 删除信号量
// 参数 #int sem_id 信号量标识符
void del_sem_set(int semid)
{
    union semun sem_union;

    if(semctl(semid, 0, IPC_RMID, sem_union) == -1)
        err_exit("failed to delete semaphore");
    return;
}
