#include "myshell_signals.h"


/*
* General purpose signal action installer specified by 
* the handler parameter associated with the signal specified by
* the signo parameter. Uses sigaction for installation. 
*/
void install_action(int signo, void *handler) {
    struct sigaction *act;
    act = malloc(sizeof(struct sigaction)); 
    act->sa_sigaction = handler; 
    act->sa_flags = SA_SIGINFO; 
    
    sigaction(signo, act, NULL); 
}

 
// Send the pipe RD file descriptor to the parent process using sigqueue on SIGUSR1
void send_fd_to_parent(pid_t pid, int fd) {
    union sigval *sigdata;
    sigdata = malloc(sizeof(union sigval)); 
    sigdata->sival_int = fd;
    sigqueue(pid, SIGUSR1, *sigdata);
    free(sigdata); 
}

// Send the RD pipe select to parent process using sigqueue on SIGUSR2
void send_rdselect_to_parent(pid_t pid, int fd) {
    union sigval *sigdata;
    sigdata = malloc(sizeof(union sigval)); 
    sigdata->sival_int = fd;
    sigqueue(pid, SIGUSR2, *sigdata);
    free(sigdata); 
}


// Handle sigqueue from child process that sends the pipe RD file descriptor
void recv_rdfd_action(int signo, siginfo_t *siginfo, void *context) {
    rd_pipefd = siginfo->si_value; // Set the global variable srd_pipefd to the value passed to sigaction from sigqueue
}

// Handle sigqueue from child process that sends the RD select
void recv_rdselect_action(int signo, siginfo_t *siginfo, void *context) {
    rd_from_pipe = siginfo->si_value; // Set the global variable srd_pipefd to the value passed to sigaction from sigqueue
}