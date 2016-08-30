/**
*@file SignalUtility.hpp
*@author jasonxiong
*@date 2009-12-09
*@version 1.0
*@brief 信号操作类
*
*
*/

#ifndef __SIGNAL_UTIL_HPP__
#define __SIGNAL_UTIL_HPP__

namespace ServerLib
{

#ifdef WIN32
/* Signals.  */
#define SIGHUP          1       /* Hangup (POSIX).  */
#define SIGINT          2       /* Interrupt (ANSI).  */
#define SIGQUIT         3       /* Quit (POSIX).  */
#define SIGILL          4       /* Illegal instruction (ANSI).  */
#define SIGTRAP         5       /* Trace trap (POSIX).  */
#define SIGABRT         6       /* Abort (ANSI).  */
#define SIGIOT          6       /* IOT trap (4.2 BSD).  */
#define SIGBUS          7       /* BUS error (4.2 BSD).  */
#define SIGFPE          8       /* Floating-point exception (ANSI).  */
#define SIGKILL         9       /* Kill, unblockable (POSIX).  */
#define SIGUSR1         10      /* User-defined signal 1 (POSIX).  */
#define SIGSEGV         11      /* Segmentation violation (ANSI).  */
#define SIGUSR2         12      /* User-defined signal 2 (POSIX).  */
#define SIGPIPE         13      /* Broken pipe (POSIX).  */
#define SIGALRM         14      /* Alarm clock (POSIX).  */
#define SIGTERM         15      /* Termination (ANSI).  */
#define SIGSTKFLT       16      /* Stack fault.  */
#define SIGCLD          SIGCHLD /* Same as SIGCHLD (System V).  */
#define SIGCHLD         17      /* Child status has changed (POSIX).  */
#define SIGCONT         18      /* Continue (POSIX).  */
#define SIGSTOP         19      /* Stop, unblockable (POSIX).  */
#define SIGTSTP         20      /* Keyboard stop (POSIX).  */
#define SIGTTIN         21      /* Background read from tty (POSIX).  */
#define SIGTTOU         22      /* Background write to tty (POSIX).  */
#define SIGURG          23      /* Urgent condition on socket (4.2 BSD).  */
#define SIGXCPU         24      /* CPU limit exceeded (4.2 BSD).  */
#define SIGXFSZ         25      /* File size limit exceeded (4.2 BSD).  */
#define SIGVTALRM       26      /* Virtual alarm clock (4.2 BSD).  */
#define SIGPROF         27      /* Profiling alarm clock (4.2 BSD).  */
#define SIGWINCH        28      /* Window size change (4.3 BSD, Sun).  */
#define SIGPOLL         SIGIO   /* Pollable event occurred (System V).  */
#define SIGIO           29      /* I/O now possible (4.2 BSD).  */
#define SIGPWR          30      /* Power failure restart (System V).  */
#define SIGSYS          31      /* Bad system call.  */
#define SIGUNUSED       31

/* Type of a signal handler.  */
typedef void (*__sighandler_t) (int);

/* A `sigset_t' has a bit for each signal.  */
# define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))

typedef struct
{
    unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

/* Structure describing the action to be taken when a signal arrives.  */
struct sigaction
{
    /* Signal handler.  */
#ifdef __USE_POSIX199309
    union
    {
        /* Used if SA_SIGINFO is not set.  */
        __sighandler_t sa_handler;
        /* Used if SA_SIGINFO is set.  */
        void (*sa_sigaction) (int, siginfo_t *, void *);
    }
    __sigaction_handler;
# define sa_handler     __sigaction_handler.sa_handler
# define sa_sigaction   __sigaction_handler.sa_sigaction
#else
    __sighandler_t sa_handler;
#endif //__USE_POSIX199309

    /* Additional set of signals to be blocked.  */
    __sigset_t sa_mask;

    /* Special flags.  */
    int sa_flags;

    /* Restore handler.  */
    void (*sa_restorer) (void);
};

/* Set the handler for the signal SIG to HANDLER, returning the old
handler, or SIG_ERR on error.
By default `signal' has the BSD semantic.  */
extern __sighandler_t signal (int __sig, __sighandler_t __handler)
__THROW;

/* Get and/or set the action for signal SIG.  */
extern int sigaction (int __sig, _const struct sigaction *__restrict __act,
                      struct sigaction *__restrict __oact) __THROW;

/* Clear all signals from SET.  */
extern int sigemptyset (sigset_t *__set) __THROW __nonnull ((1));
#endif //WIN32

typedef void (*Function_SignalHandler)(int);

class CSignalUtility
{
private:
    CSignalUtility() {}
    ~CSignalUtility() {}

public:
    //!忽略某个信号
    static void IgnoreSignal(int iSignalValue);

    //!设置某个信号的捕捉函数
    static void SetSignalHandler(int iSignalValue, Function_SignalHandler pfHandler);

public:

    static void IgnoreSignalSet();

    static void SetHandler_USR1(Function_SignalHandler pfUSR1, int iSignalArg);
    static void SetHandler_USR2(Function_SignalHandler pfUSR2, int iSignalArg);
    static void SetHandler_QUIT(Function_SignalHandler pfQuit, int iSignalArgQuit);


private:

    //注册
    static void RegisterSignal_USR1();

    //注册
    static void RegisterSignal_USR2();

    //注册
    static void RegisterSignal_Quit();

    static void NotifySignal_USR1(int iSignalValue);

    static void NotifySignal_USR2(int iSignalValue);

    static void NotifySignal_Quit(int iSignalValue);

private:

    static Function_SignalHandler m_pfUSR1;
    static Function_SignalHandler m_pfUSR2;
    static Function_SignalHandler m_pfQuit;

    static int m_iSignalArgUSR1;
    static int m_iSignalArgUSR2;
    static int m_iSignalArgQuit;
};

}

#endif //__SIGNAL_UTIL_HPP__
///:~

----------------------------------------------------------------
This file is converted by NJStar Communicator - www.njstar.com
----------------------------------------------------------------
