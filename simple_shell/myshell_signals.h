#ifndef MYSHELL_SIGNALS_H
#define MYSHELL_SIGNALS_H
#include "signals.h"


void send_val_to_parent(pid_t pid, int fd);

void signal_handler(int signo, siginfo_t *siginfo, void *context);

void sig_set_handler(int signo, void *handler);

void sig_handler(); 




#endif MYSHELL_SIGNALS_H