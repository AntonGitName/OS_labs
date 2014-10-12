#include <iostream>

#include "daemon.h"

int main(int argc, char** argv)
{
    /*if (argc != 2)
    {
        printf("Usage: ./%s filename.cfg\n", DAEMON_NAME);
        return EXIT_FAILURE;
    }
*/
    startDaemon(argv[1]);
}
