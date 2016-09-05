#include "signal_handle.h"

// 设置信号处理程序
// 参数 #int: 信号    #sighandler_t: 信号处理函数指针
sighandler_t Signal(int signo, sighandler_t sigfun)
{
    struct sigaction act, oact;
    act.sa_handler = sigfun;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM)
    {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif // SA_INTERRUPT
    }
    else
        act.sa_flags |= SA_RESTART;
    if (sigaction(signo, &act, &oact) < 0)
        return(SIG_ERR);
    return(oact.sa_handler);
}

// 所有信号的处理程序
// 参数   int: 发生的信号
void signal_handler(int signo)
{

    bool do_exit = false;

    switch(signo)
    {
        case SIGCHLD:
            int status;
            pid_t pid;
            //-1 等待任一子进程 0 等待调用进程组中的任一子进程  &status 可以替换成 NULL
            while((pid = waitpid(-1, &status, WNOHANG)) > 0);
            break;
        case SIGALRM:
            break;
        case SIGIO:
            break;
        case SIGINT:
            do_exit = true;
            {
                extern int sem_id;
                extern int shm_id;    // 信号量集  共享内存  标识符
                extern groupMsg_package* pack_ptr;
                extern unsigned int previous_MsgID;

                // 保存消息记录
                FILE* fp = fopen("./message_record.dat", "rb+");
                fwrite(pack_ptr, sizeof(groupMsg_package), 1, fp);
                fseek(fp, 0, SEEK_END);
                fwrite(pack_ptr + previous_MsgID +1, sizeof(groupMsg_package), get_MsgID(pack_ptr, sem_id) - previous_MsgID, fp);
                fclose(fp);

                default_handler();

                del_sem_set(sem_id);
                del_shemem(shm_id);
            }
            break;
        case SIGHUP:    // 终端关闭产生此信号
            do_exit = true;
            {
                extern int sem_id;
                extern int shm_id;    // 信号量集  共享内存  标识符
                extern groupMsg_package* pack_ptr;
                extern unsigned int previous_MsgID;

                // 保存消息记录
                FILE* fp = fopen("./message_record.dat", "rb+");
                fwrite(pack_ptr, sizeof(groupMsg_package), 1, fp);
                fseek(fp, 0, SEEK_END);
                fwrite(pack_ptr + previous_MsgID +1, sizeof(groupMsg_package), get_MsgID(pack_ptr, sem_id) - previous_MsgID, fp);
                fclose(fp);

                default_handler();

                del_sem_set(sem_id);
                del_shemem(shm_id);
            }
            break;
        case SIGTERM:
            do_exit = true;
            break;
        case SIGQUIT:
            do_exit = true;
            {
                extern int sem_id;
                extern int shm_id;    // 信号量集  共享内存  标识符
                extern groupMsg_package* pack_ptr;
                extern unsigned int previous_MsgID;

                // 保存消息记录
                FILE* fp = fopen("./message_record.dat", "rb+");
                fwrite(pack_ptr, sizeof(groupMsg_package), 1, fp);
                fseek(fp, 0, SEEK_END);
                fwrite(pack_ptr + previous_MsgID +1, sizeof(groupMsg_package), get_MsgID(pack_ptr, sem_id) - previous_MsgID, fp);
                fclose(fp);

                default_handler();

                del_sem_set(sem_id);
                del_shemem(shm_id);
            }
            break;
        case SIGUSR1:
            break;
        case SIGUSR2:
            break;
    }

    if(do_exit)
        exit(EXIT_SUCCESS);

    return;
}


// 用户信号专用信号处理程序  用于消息推送
void sigusr1_handler(int signo)
{

    // 推送消息

    extern int  client_fd;
    extern groupMsg_package* pack_ptr;
    extern int sem_id;
    sigset_t oset;
    groupMsg_package packet_buff;

    block_specified_signals(&oset);
    sighandler_t sh = Signal(SIGPIPE, SIG_IGN);    //忽略SIGPIPE

    get_packet(pack_ptr, sem_id, 0, packet_buff);  // packnum = 0 表示默认获取最新的消息包

    if (build_pack(client_fd, packet_buff) == -1 )
    {
        close(client_fd);
        Signal(SIGPIPE, sh);  //恢复SIGPIPE
        err_exit("send error");
    }
    Signal(SIGPIPE, sh);  //恢复SIGPIPE
    unblock_all_signals(&oset);

    return;
}

void register_signal_handler()
{
    if (Signal(SIGCHLD, signal_handler) == SIG_ERR )  err_exit("set SIGCHLD handler error");
    if (Signal(SIGALRM, signal_handler) == SIG_ERR )  err_exit("set SIGALRM handler error");
    if (Signal(SIGIO,   signal_handler) == SIG_ERR )  err_exit("set SIGIO handler error");
    if (Signal(SIGINT,  signal_handler) == SIG_ERR )  err_exit("set SIGINT handler error");
    if (Signal(SIGHUP,  signal_handler) == SIG_ERR )  err_exit("set SIGHUP handler error");
    if (Signal(SIGTERM, signal_handler) == SIG_ERR )  err_exit("set SIGTERM handler error");
    if (Signal(SIGQUIT, signal_handler) == SIG_ERR )  err_exit("set SIGQUIT handler error");
    if (Signal(SIGUSR1, signal_handler) == SIG_ERR )  err_exit("set SIGUSR1 handler error");
    if (Signal(SIGUSR2, signal_handler) == SIG_ERR )  err_exit("set SIGUSR2 handler error");
    return;
}


// 将各种修改过的信号恢复成默认处理程序
void default_handler()
{
    if (Signal(SIGCHLD, SIG_DFL) == SIG_ERR )  err_exit("set SIGCHLD handler error");
    if (Signal(SIGALRM, SIG_DFL) == SIG_ERR )  err_exit("set SIGALRM handler error");
    if (Signal(SIGIO,   SIG_DFL) == SIG_ERR )  err_exit("set SIGIO handler error");
    if (Signal(SIGINT,  SIG_DFL) == SIG_ERR )  err_exit("set SIGINT handler error");
    if (Signal(SIGHUP,  SIG_DFL) == SIG_ERR )  err_exit("set SIGHUP handler error");
    if (Signal(SIGTERM, SIG_DFL) == SIG_ERR )  err_exit("set SIGTERM handler error");
    if (Signal(SIGQUIT, SIG_DFL) == SIG_ERR )  err_exit("set SIGQUIT handler error");
    if (Signal(SIGUSR1, SIG_DFL) == SIG_ERR )  err_exit("set SIGUSR1 handler error");
    if (Signal(SIGUSR2, SIG_DFL) == SIG_ERR )  err_exit("set SIGUSR2 handler error");
    return;
}

// 阻塞较有可能出现的信号
void block_specified_signals(sigset_t* oset)
{
    static sigset_t sigset;

    sigemptyset(&sigset);

    sigaddset(&sigset, SIGCHLD);
    sigaddset(&sigset, SIGALRM);
    sigaddset(&sigset, SIGIO);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGQUIT);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);

    if(sigprocmask(SIG_BLOCK, &sigset, oset) == -1){
        err_exit("sigprocmask block signal failed!");
    }
}

// 解除对所有信号的阻塞
void unblock_all_signals(sigset_t* oset)
{
    static sigset_t sigset;
    if (oset == NULL)
        sigemptyset(&sigset);
    else
        sigset = *oset;

    if(sigprocmask(SIG_SETMASK, &sigset, NULL) == -1){
        err_exit("sigprocmask block signal failed!");
    }
}



















//void sig_chld(int signo)
//{
//    pid_t pid;
//    int status;
//    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
//    return;
//}

