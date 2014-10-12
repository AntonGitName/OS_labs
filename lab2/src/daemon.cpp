#include "daemon.h"

static struct sigaction act;

static void loadConfig(char* filename)
{

}

static void sighandler(int signum, siginfo_t *info, void *ptr)
{
    printf("Received signal %d\n", signum);
    printf("Signal originates from process %lu\n",
        (unsigned long)info->si_pid);
}

static void setSignalHandler()
{
    memset(&act, 0, sizeof(act));

    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &act, NULL);
}

static void daemonMainLoop()
{

}

void startDaemon(char* cfgFilename)
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Open the log file */
    openlog (DAEMON_NAME, LOG_PID, LOG_DAEMON);

    for (;;)
    {
        daemonMainLoop();
    }

    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();

    exit(EXIT_SUCCESS);
}
