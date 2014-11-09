#include "my_signal.h"

void register_signal(int sig, my_sig_handler handler)
{
    signal(sig, handler);
}
