#include "myshell_signals.h"

bool rd_from_pipe;   // rd_from_pipe is true for all commands in the pipeline, except for the first
int rd_pipefd;               // File descriptor of pipe file to read from for input to current command 


void send_val_to_parent(pid_t pid, int fd) {
    union sigval *sigdata;
    sigdata = malloc(sizeof(union sigval)); 
    sigdata->sival_int = fd ;
    sigqueue(pid, SIGUSR1, *sigdata);
    free(sigdata); 
}

void signal_handler(int signo, siginfo_t *siginfo, void *context) {
    rd_pipefd = siginfo->si_value; // Set the global rd_pipefd to the value passed from the child process
    
}

void sig_set_handler(int signo, void *handler) {
    struct sigaction *act;
    act = malloc(sizeof(struct sigaction)); 
    act->sa_sigaction = handler; 
    act->sa_flags = SA_SIGINFO; 
}

void sig_handler() {
    // child calls kill, parent signal function fires
    // this sig_handler sets the global rd_pipefd to the one passed
    // to kill <- figure out how to pass args to signal in kill
    // that is visible to parent process
}