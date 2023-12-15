int srv_daemonize(void)
{
    int pid;

    pid = fork();
    if(pid < 0)
        exit(1);        // 创建错误，退出
    else if(pid > 0)    // 父进程退出
        exit(0);

    setsid();           // 使子进程成为组长
    pid = fork();
    if(pid > 0)
        exit(0);        // 再次退出，使进程不是组长，这样进程就不会打开控制终端
    else if(pid < 0)
        exit(1);

    // 关闭进程打开的文件句柄
    for (int i = 0; i < NOFILE; i++)
        close(i);

    chdir("/");     // 改变目录
    umask(0);       // 重设文件创建的掩码

    return 0;
}

