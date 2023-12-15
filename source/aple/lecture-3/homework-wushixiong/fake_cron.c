#include <bits/time.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#define CMD_MAX_BYTES 128
struct mycmd {
    char cmd[CMD_MAX_BYTES];
    int  start;  // begin run cmd after 'start' seconds
    int  period; // run cmd after every 'peroid' second
    int  count;  // run cmd 'count' times
};

struct simple_cron {
    struct mycmd cmd1;
    struct mycmd cmd2;
    struct mycmd cmd3;
};
struct simple_cron mycron;

int
load_config(struct simple_cron *cron)
{
#define CONFIGFILE "/tmp/test.cfg"
    FILE   *file;
    char   *line = NULL;
    ssize_t nread;
    size_t  len;
    char   *pnext;
    file = fopen(CONFIGFILE, "r");
    if (file == NULL) {
        syslog(LOG_ERR, "failed to load %s", CONFIGFILE);
        return -1;
    }
    int cmd_count = 0;
    while ((nread = getline(&line, &len, file)) != -1) {
        line[nread] = 0x0;
        // syslog(LOG_INFO, "read=[%s] len=%d nread=%d", line, len, nread);
        if (line[0] == '#') {
            continue; // ignore comments
        }
        int  start;
        int  period;
        int  count;
        char buf[CMD_MAX_BYTES];
        memset(buf, 0x0, sizeof(buf));
        sscanf(line, "%d %d %d %[^\n]%*c", &start, &period, &count, buf);
        syslog(LOG_INFO, "start=%d period=%d count=%d cmd=%s", start, period,
               count, buf);
        struct mycmd tmp;
        tmp.start  = start;
        tmp.period = period;
        tmp.count  = count;
        strncpy(tmp.cmd, buf, strlen(buf));

        switch (cmd_count) {
        case 0:
            memcpy(&cron->cmd1, &tmp, sizeof(struct mycmd));
            cmd_count++;
            break;
        case 1:
            memcpy(&cron->cmd2, &tmp, sizeof(struct mycmd));
            cmd_count++;
            break;
        case 2:
            memcpy(&cron->cmd3, &tmp, sizeof(struct mycmd));
            cmd_count++;
            break;
        defalut:
            syslog(LOG_INFO, "only support 3 comamnds");
            syslog(LOG_INFO, "ignore %s", buf);
            break;
        }
    }
    free(line);
    return 0;
}

void
print_elapsed_time(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    syslog(LOG_INFO, "pid=%d %d.%d", getpid(), now.tv_sec, now.tv_nsec);
}

int
set_timerfd_value(int begin_secs, int interval_secs)
{
    struct timespec   now;
    struct itimerspec new_value;
    int               fd;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1) {
        syslog(LOG_ERR, "clock_gettime failed");
        exit(EXIT_FAILURE);
    }

    // ont-shot tiemr
    // new.it_interval.tv_sec  = 0;
    // new.it_interval.tv_usec = 0;
    // tiemr start at 'start' seconds later
    new_value.it_value.tv_sec     = now.tv_sec + begin_secs;
    new_value.it_value.tv_nsec    = now.tv_nsec;
    new_value.it_interval.tv_sec  = interval_secs;
    new_value.it_interval.tv_nsec = 0;

    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1) {
        syslog(LOG_ERR, "tiemrfd_create failed");
        exit(EXIT_FAILURE);
    }

    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1) {
        syslog(LOG_ERR, "tiemrfd_settime failed");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_INFO, "fd=%d start=%d interval=%d ", fd,
           new_value.it_value.tv_sec, new_value.it_interval.tv_sec);

    return fd;
}

int
tobeDaemon(void)
{
    int   maxfd, fd;
    pid_t pid;

    // generate a child and let parent exit
    pid = fork();
    switch (pid) {
    case -1:
        syslog(LOG_ERR, "fork failed");
        return -1;
    case 0: // child
        break;
    default: // parent
        exit(EXIT_SUCCESS);
    }
    // create a session
    if (setsid() == -1) {
        syslog(LOG_ERR, "set session id failed");
        return -1;
    }

    // avoid to be the leader of the session group
    pid = fork();
    switch (pid) {
    case -1:
        syslog(LOG_ERR, "fork failed");
        return -1;
    case 0: // child
        break;
    default: // parent
        exit(EXIT_SUCCESS);
    }

    umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    chdir("/");
    maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1) maxfd = 8192;
    for (fd = 0; fd < maxfd; fd++) {
        close(fd); // close all opend files
    }
    // reopen stdin,stdout,stderr to /dev/null
    fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO) {
        syslog(LOG_ERR, "reopen stdin failed");
        return -1;
    }
    if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO) {
        syslog(LOG_ERR, "dup stdout failed");
        return -1;
    }
    if (dup2(fd, STDERR_FILENO) != STDERR_FILENO) {
        syslog(LOG_ERR, "dup stderr failed");
        return -1;
    }
    syslog(LOG_INFO, "new daemon[%d] start", getpid());
    return 0; // sucess to be a daemon
}

void
signal_handler(int signum)
{
    switch (signum) {
    case SIGTERM:
        // cleanup
        syslog(LOG_ERR, "exit cleanup");
        break;
    case SIGHUP:
        // reload config
        memset(&mycron, 0x0, sizeof(struct simple_cron));
        load_config(&mycron);
        break;
    default:
        break;
    }
}

int
main()
{
    // TODO: man 7 daemon -> New-Style Daemons
    // TODO: need process only run one instance

    struct sigaction sa;
    memset(&sa, 0x0, sizeof(struct sigaction));
    sa.sa_handler = &signal_handler;

    if (sigaction(SIGTERM, &sa, NULL) != 0) {
        printf("register SIGTERM signal process failed\n");
        return -1;
    }

    if (sigaction(SIGHUP, &sa, NULL) != 0) {
        printf("register SIGHUP signal process failed\n");
        return -1;
    }

    tobeDaemon();

    memset(&mycron, 0x0, sizeof(struct simple_cron));
    load_config(&mycron);
    int      fd1, fd2, fd3;
    int      nread;
    fd_set   rfds;
    uint64_t exp;
    int      nready;
    if (strlen(mycron.cmd1.cmd) != 0) {
        fd1 = set_timerfd_value(mycron.cmd1.start, mycron.cmd1.period);
    }
    if (strlen(mycron.cmd2.cmd) != 0) {
        fd2 = set_timerfd_value(mycron.cmd2.start, mycron.cmd2.period);
    }
    if (strlen(mycron.cmd3.cmd) != 0) {
        fd3 = set_timerfd_value(mycron.cmd3.start, mycron.cmd3.period);
    }
    FD_ZERO(&rfds);

    // for (int i = 0; i < 10; i++) {
    while (1) {
        // sleep(5);
        // syslog(LOG_INFO, "daemon [%d] running", getpid());
        // syslog(LOG_INFO, "cmd1=%s", mycron.cmd1.cmd);
        // syslog(LOG_INFO, "cmd2=%s", mycron.cmd2.cmd);
        // syslog(LOG_INFO, "cmd3=%s", mycron.cmd3.cmd);
        FD_SET(fd1, &rfds);
        FD_SET(fd2, &rfds);
        FD_SET(fd3, &rfds);
        // ret = select(fd3 + 1, &rfds, NULL, NULL, &tv);
        nready = select(fd3 + 1, &rfds, NULL, NULL, NULL); // wait forever
        if (nready == -1) {
            syslog(LOG_ERR, "select error");
            exit(EXIT_FAILURE);
        }
        if (nready == 0) continue;

        while (nready) {
            print_elapsed_time();
            if (FD_ISSET(fd1, &rfds)) {
                read(fd1, &exp, sizeof(uint64_t));
                syslog(LOG_INFO, "timer1 read: %d", exp);
                nready--;
            }
            if (FD_ISSET(fd2, &rfds)) {
                read(fd2, &exp, sizeof(uint64_t));
                syslog(LOG_INFO, "timer2 read: %d", exp);
                nready--;
            }
            if (FD_ISSET(fd3, &rfds)) {
                read(fd3, &exp, sizeof(uint64_t));
                syslog(LOG_INFO, "timer3 read: %d", exp);
                nready--;
            }
        }
    }

    syslog(LOG_INFO, "daemon exit");
    return 0;
}
