#include "commons.h"
#include "my_signal.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <syslog.h>

const int MAGIC_KEY = 2014;
const int BUFF_SIZE = (32 + 1) * 3 * sizeof(char);

int shmid;
char* shm;

void signal_handler(int signal)
{
    syslog (LOG_NOTICE, "CHILD: SIGUSR1 received.\n");

    syslog (LOG_NOTICE, "PARENT: Reading strings from shared memory:\n");

    int offset = 0;
    const char* strings[3];
    for (int i = 0; i < 3; ++i)
    {
        strings[i] = shm + offset;
        offset += strlen(strings[i]) + 1;
        syslog (LOG_NOTICE, "[%d] %s\n", i, strings[i]);
    }

    syslog (LOG_NOTICE, "CHILD: Result: %d \n", target_func(strings[0], strings[1], strings[2]));

    syslog (LOG_NOTICE, "CHILD: the work is done.\n");
    closelog();
    exit(EXIT_SUCCESS);
}

void child_job()
{
    openlog ("CHILD", LOG_PID, LOG_DAEMON);

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

    syslog (LOG_NOTICE, "PARENT: creating segment of shared memory of %i bytes\n", BUFF_SIZE);

    shmid = shmget(IPC_PRIVATE, BUFF_SIZE, IPC_CREAT|0660);
    if (shmid == -1)
    {
        syslog (LOG_ERR, "PARENT: shmget error");
        exit(EXIT_FAILURE);
    }

    shm = (char*) shmat(shmid, 0, 0);

    if (shm == (char*) -1)
    if (shmid == -1)
    {
        syslog (LOG_ERR, "PARENT: shmat error");
        exit(EXIT_FAILURE);
    }

    syslog (LOG_NOTICE, "PARENT: Memory attached\n");

    syslog (LOG_NOTICE, "PARENT: Strings generated:\n");

    int offset = 0;
    for (int i = 0; i < 3; ++i)
    {
        syslog (LOG_NOTICE, "[%d] %s\n", i, str[i].c_str());
        strcpy(shm + offset, str[i].c_str());
        offset += str[i].length() + 1;
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
        syslog (LOG_NOTICE, "PARENT: child proccess %i created\n", (int)pid);
        wait(0);
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);

        syslog (LOG_NOTICE, "PARENT: the work is done.\n");
        closelog();
        exit(EXIT_SUCCESS);
    }
    else
    {
        child_job();
    }
}
