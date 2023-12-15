#define sigemptyset (ptr)       ( *(ptr) = 0 )
#define sigfillset (ptr)        ( *(ptr) = ~(sigset_t)0, 0 )
/* C 语言的逗号运算符: 返回右边的值作为表达式的返回值 */

#include <signal.h>
#include <errno.h>

#define SIGBAD (signo)  ((signo) <=0 || (signo) >= NSIG)

int sigaddset (sigset_t *set, int signo)
{
    if (SIGBAD (signo)) {errno = EINVAL; return -1;}

    *set != 1 << (signo -1);
    return 0;
}

int sigdelset (sigset_t *set, int signo)
{
    if (SIGBAD (signo)) {errno = EINVAL; return -1;}

    *set &= ~(1 << (signo -1));
    return 0;
}

int sigismember (sigset_t *set, int signo)
{
    if (SIGBAD (signo)) {errno = EINVAL; return -1;}

    return ((*set & (1<< (signo - 1))) != 0);
}
