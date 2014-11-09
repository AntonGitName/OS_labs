#ifndef MY_SIGNAL_H
#define MY_SIGNAL_H

#include <signal.h>

typedef void (*my_sig_handler)(int);
void register_signal(int sig, my_sig_handler handler);

#endif /* MY_SIGNAL_H */
