#ifndef MYSHELL_SIGNALS_H
#define MYSHELL_SIGNALS_H
#include "stdbool.h"
#include "stdlib.h"
#include "signal.h"

extern bool rd_from_pipe;
extern int rd_pipefd; // File descriptor of pipe file to read from for input to current command 
extern bool is_final_cmd; // Is true when the current pipeline_command is the final command in the pipeline

void install_action(int signo, void *handler);
void send_fd_to_parent(pid_t pid, int fd);
void send_rdselect_to_parent(pid_t pid, int fd);
void recv_rdfd_action(int signo, siginfo_t *siginfo, void *context);
void recv_rdselect_action(int signo, siginfo_t *siginfo, void *context); 

#endif