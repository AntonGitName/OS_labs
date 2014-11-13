
#include "commons.h"
#include "my_signal.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <syslog.h>

int myPipe[2];

void signal_handler(int signal)
{
    syslog (LOG_NOTICE, "CHILD: SIGUSR1 received.\n");

    syslog (LOG_NOTICE, "PARENT: Reading strings from the pipe:\n");

    FILE *stream;
    stream = fdopen (myPipe[0], "r");

    const int buffSize = 32;
    char strings[3][buffSize + 2];
    for (int i = 0; i < 3; ++i)
    {
        fgets(strings[i], buffSize, stream);
        if ((strlen(strings[i]) > 0) && (strings[i][strlen(strings[i]) - 1] == '\n'))
        {
            strings[i][strlen(strings[i]) - 1] = '\0';
        }
        syslog (LOG_NOTICE, "[%d] %s\n", i, strings[i]);
    }

    fclose (stream);

    syslog (LOG_NOTICE, "CHILD: Result: %d \n", target_func(strings[0], strings[1], strings[2]));

    syslog (LOG_NOTICE, "CHILD: the work is done.\n");
    closelog();
    exit(EXIT_SUCCESS);
}

void child_job()
{
    openlog ("CHILD", LOG_PID, LOG_DAEMON);

    close (myPipe[1]);

    syslog (LOG_NOTICE, "CHILD: registering signal SIGUSR1\n");
    register_signal(SIGUSR1, signal_handler);
    syslog (LOG_NOTICE, "CHILD: signal SIGUSR1 registered\n");
    for (;;);
}

int main()
{
    /* Open the log file */
    openlog ("PARENT", LOG_PID, LOG_DAEMON);

    std::string str[] = {generate_str(), generate_str(), generate_str()};

    syslog(LOG_NOTICE, "PARENT: creating pipe\n");

    if (pipe(myPipe))
    {
        syslog(LOG_ERR, "PARENT: failed to create a pipe\n");
        exit(EXIT_FAILURE);
    }

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
        syslog(LOG_NOTICE, "PARENT: child proccess %i created\n", (int)pid);

        close(myPipe[0]);

        syslog (LOG_NOTICE, "PARENT: Sending strings to the pipe:\n");

        FILE *stream;
        stream = fdopen (myPipe[1], "w");
        for (int i = 0; i < 3; ++i)
        {
            syslog (LOG_NOTICE, "[%d] %s\n", i, str[i].c_str());
            fprintf (stream, "%s\n", str[i].c_str());
        }
        fclose (stream);

        wait(0);

        syslog (LOG_NOTICE, "PARENT: the work is done.\n");
        closelog();
        exit(EXIT_SUCCESS);
    }
    else
    {
        child_job();
    }
}
