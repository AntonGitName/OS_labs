#include "commons.h"
#include "my_signal.h"

#include <string.h>
#include  <sys/shm.h>

const int MAGIC_KEY = 2014;

int buffSize;

void parent_job(pid_t childPid)
{
    /* Open the log file */
    openlog (DAEMON_NAME, LOG_PID, LOG_DAEMON);

    syslog (LOG_NOTICE, "PARENT: child proccess %i created\n", (int)childPid);

    const char* str[] = {generate_str(), generate_str(), generate_str()};
    buffSize = strlen(str[0]) + strlen(str[1]) + strlen(str[2]) + 3;

    syslog (LOG_NOTICE, "PARENT: creating segment of shared memory of %i bytes\n", buffsize);

    int shmid = shmget(MAGIC_KEY, buffSize, IPC_CREATE);
    char* shm = (char*) shmat(shmid, 0, 0);

    int offset = 0;
    for (int i = 0; i < 3; ++i)
    {
        strcpy(shm + offset, str[i]);
        offset += strlen(str[i]) + 1;
    }

    int returnStatus;
    waitpid(childPid, &returnStatus, 0);  // Parent process waits here for child to terminate.

    if (returnStatus == 0)  // Verify child process terminated without error.
    {
       syslog (LOG_NOTICE, "PARENT: The child process terminated normally\n");
    } else
    {
       syslog (LOG_NOTICE, "PARENT: The child process terminated with an error!\n");
    }

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    syslog (LOG_NOTICE, "PARENT: the work is done.\n");
    closelog();
    exit(EXIT_SUCCESS);
}

void signal_handler(int signal)
{
    syslog (LOG_NOTICE, "SIGUSR1 received.\n");

    int shmid = shmget(MAGIC_KEY, );

    syslog (LOG_NOTICE, "CHILD: the work is done.\n");
    closelog();
    exit(EXIT_SUCCESS);
}

void child_job()
{
    openlog (DAEMON_NAME, LOG_PID, LOG_DAEMON);

    syslog (LOG_NOTICE, "CHILD: registering signal\n");
    register_signal(SIGUSR1, signal_handler);
    for (;;);
}



int main()
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
        parent_job(pid);

    }
    else
    {
        child_job();
    }
}
