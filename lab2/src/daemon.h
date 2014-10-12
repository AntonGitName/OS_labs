#pragma once
#ifndef DAEMON_H
#define DAEMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <memory.h>

const char* const DAEMON_NAME = "labDaemon";

void startDaemon(char* cfgFilename);

#endif /* DAEMON_H */
