#include "signal_handle.h"

sigjmp_buf jmpbuf;
volatile sig_atomic_t canjmp;

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
    switch(signo)
    {
        case SIGALRM:
            break;
        case SIGIO:
            break;
        case SIGINT:
            if (canjmp == 0)   // 跳转点还没有设置好
                return;
            siglongjmp(jmpbuf, 1);
            break;
        case SIGHUP:
            if (canjmp == 0)   // 跳转点还没有设置好
                return;
            siglongjmp(jmpbuf, 1);
            break;
        case SIGTERM:
            if (canjmp == 0)   // 跳转点还没有设置好
                return;
            siglongjmp(jmpbuf, 1);
            break;
        case SIGQUIT:
            if (canjmp == 0)   // 跳转点还没有设置好
                return;
            siglongjmp(jmpbuf, 1);
            break;
        case SIGUSR1:
            break;
        case SIGUSR2:
            break;
    }

    return;
}

void register_signal_handler(){
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
