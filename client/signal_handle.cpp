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

//// 所有信号的处理程序
//// 参数   int: 发生的信号
//void signal_handler(int signo)
//{
//    printf("# possess %u get a signal: ", (unsigned int)getpid());
//
//    bool do_exit = false;
//
//    switch(signo)
//    {
//        case SIGCHLD:
//            printf("SIGCHLD");
//            int status;
//            pid_t pid;
//            //-1 等待任一子进程 0 等待调用进程组中的任一子进程  &status 可以替换成 NULL
//            while((pid = waitpid(-1, &status, WNOHANG)) > 0);
//            break;
//        case SIGALRM:
//            printf("SIGALRM");
//            break;
//        case SIGIO:
//            printf("SIGIO");
//            break;
//        case SIGINT:
//            printf("SIGINT");
//            do_exit = true;
//            break;
//        case SIGHUP:
//            printf("SIGHUP");
//            do_exit = true;
//            break;
//        case SIGTERM:
//            printf("SIGTERM");
//            do_exit = true;
//            break;
//        case SIGQUIT:
//            printf("SIGQUIT");
//            do_exit = true;
//            break;
//        case SIGUSR1:
//            printf("SIGUSR1");
//
//            break;
//        case SIGUSR2:
//            printf("SIGUSR2");
//            break;
//    }
//
//    printf("\n");
//
//    if(do_exit)
//        exit(EXIT_SUCCESS);
//
//    return;
//}
//
//void register_signal_handler(){
//    if (Signal(SIGCHLD, signal_handler) == SIG_ERR )  err_exit("set SIGCHLD handler error");
//    if (Signal(SIGALRM, signal_handler) == SIG_ERR )  err_exit("set SIGALRM handler error");
//    if (Signal(SIGIO,   signal_handler) == SIG_ERR )  err_exit("set SIGIO handler error");
//    if (Signal(SIGINT,  signal_handler) == SIG_ERR )  err_exit("set SIGINT handler error");
//    if (Signal(SIGHUP,  signal_handler) == SIG_ERR )  err_exit("set SIGHUP handler error");
//    if (Signal(SIGTERM, signal_handler) == SIG_ERR )  err_exit("set SIGTERM handler error");
//    if (Signal(SIGQUIT, signal_handler) == SIG_ERR )  err_exit("set SIGQUIT handler error");
//    if (Signal(SIGUSR1, signal_handler) == SIG_ERR )  err_exit("set SIGUSR1 handler error");
//    if (Signal(SIGUSR2, signal_handler) == SIG_ERR )  err_exit("set SIGUSR2 handler error");
//    return;
//}
//
//// 阻塞较有可能出现的信号
//void block_specified_signals(sigset_t* oset)
//{
//    sigset_t sigset;
//
//    sigemptyset(&sigset);
//
//    sigaddset(&sigset, SIGCHLD);
//    sigaddset(&sigset, SIGALRM);
//    sigaddset(&sigset, SIGIO);
//    sigaddset(&sigset, SIGINT);
//    sigaddset(&sigset, SIGHUP);
//    sigaddset(&sigset, SIGTERM);
//    sigaddset(&sigset, SIGQUIT);
//    sigaddset(&sigset, SIGUSR1);
//    sigaddset(&sigset, SIGUSR2);
//
//    if(sigprocmask(SIG_BLOCK, &sigset, NULL) == -1){
//        err_exit("sigprocmask block signal failed!");
//    }
////    cout << "sigprocmask block signal success!" << endl;
//}
//
//// 解除对所有信号的阻塞
//void unblock_all_signals(sigset_t* oset)
//{
//    sigset_t sigset;
//    if (oset == NULL)
//        sigemptyset(&sigset);
//    else
//        sigset = *oset;
//
//    if(sigprocmask(SIG_SETMASK, &sigset, NULL) == -1){
//        err_exit("sigprocmask block signal failed!");
//    }
////    cout << "sigprocmask block signal success!" << endl;
//}



















//void sig_chld(int signo)
//{
//    pid_t pid;
//    int status;
//    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
//    return;
//}

