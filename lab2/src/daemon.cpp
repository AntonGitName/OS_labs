#include "daemon.h"

static char      s_cfgFilename[PATH_MAX + 1];
static int       s_updatePeriod;
static char      s_dirToDelete[PATH_MAX + 1];

static void loadConfig()
{
    FILE* cfgFile = fopen(s_cfgFilename, "r");
    if (cfgFile == NULL)
    {
        syslog(LOG_ERR, "Failed to find configuration file (%s). Terminating daemon\n", s_cfgFilename);
        closelog();
        exit(EXIT_FAILURE);
    }
    if (fscanf(cfgFile, "%s %i", s_dirToDelete, &s_updatePeriod) != 2)
    {
        syslog(LOG_ERR, "Failed to read configuration file. Terminating daemon\n");
        fclose(cfgFile);
        closelog();
        exit(EXIT_FAILURE);
    }
    syslog(LOG_NOTICE, "Configuration file read successfully\n");
    fclose(cfgFile);
}

void reloadCfgFile(int signum)
{
    syslog(LOG_NOTICE, "Received signal SIGHUP: reloading .cfg file\n");
    loadConfig();
}

void stopDaemon(int signum)
{
    syslog(LOG_NOTICE, "Received signal SIGTERM: terminating daemon\n");
    closelog();
    exit(EXIT_SUCCESS);
}

static void setSignalHandlers()
{
    signal(SIGTERM, stopDaemon);
    signal(SIGHUP, reloadCfgFile);
}

static void daemonMainLoop()
{
    static int loopNum = 1;
    syslog(LOG_NOTICE, "Doing loop #%i\n", loopNum++);

    std::ostringstream shellCommand;
    shellCommand << "#!/bin/bash \n";
    shellCommand << "dir=" << s_dirToDelete << " \n";
    shellCommand << "if test -z $(find $dir -type f -name dont.erase) ; then \n";
    shellCommand << "     rm -rf $dir/*\n";
    shellCommand << "fi";

    system(shellCommand.str().c_str());

    syslog(LOG_NOTICE, "Loop ended\n");
    sleep(s_updatePeriod);
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

    strcpy(s_cfgFilename, cfgFilename);
    loadConfig();
    realpath(cfgFilename, s_cfgFilename);


    /* Catch, ignore and handle signals */
    setSignalHandlers();

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

    syslog (LOG_NOTICE, "Daemon main loop has been started\n");

    for (;;)
    {
        daemonMainLoop();
    }
}
