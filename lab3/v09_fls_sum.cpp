#include "commons.h"
#include "my_signal.h"

#include <string.h>
#include <cstdio>
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

const char* const THE_FILE = "\\tmp\\fls_file_example";

char *theFile;

void signal_handler(int signal)
{
    syslog (LOG_NOTICE, "CHILD: SIGUSR1 received.\n");

    syslog (LOG_NOTICE, "PARENT: Reading strings from maped memory:\n");

    int offset = 0;
    const char* strings[3];
    for (int i = 0; i < 3; ++i)
    {
        strings[i] = theFile + offset;
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

    syslog(LOG_NOTICE, "PARENT: creating file %s\n", THE_FILE);

    int fd;
    if ((fd = open(THE_FILE, O_CREAT | O_RDWR, 0666)) < 0) {
        syslog(LOG_ERR, "PARENT: failed to create the file %s\n", THE_FILE);
        exit(EXIT_FAILURE);
    }

    syslog (LOG_NOTICE, "PARENT: Writing strings to the file:\n");

    for (int i = 0; i < 3; ++i)
    {
        syslog (LOG_NOTICE, "[%d] %s\n", i, str[i].c_str());
        write(fd, str[i].c_str(), str[i].length() + 1);
    }

    struct stat buf;

    if ((theFile = (char *)mmap(0, (size_t)buf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (void *)-1)
    {
        syslog(LOG_ERR, "mmap failure");
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
