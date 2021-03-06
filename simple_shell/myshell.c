#include "myshell_parser.h"
#include "myshell_signals.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/wait.h"
#include "signal.h"
#include "errno.h"


bool rd_from_pipe = false;   // Define and initialize rd_from_pipe global variable. False = first command
int next_rd_pipefd = -1;       // Define global variable
int current_rd_pipefd = -1; 
int wr_pipefd = -1; 
// char prompt[] = "myshell"

/*
* Point stdin and stdout to different files if input or output 
* redirection paths is not null in pipeline_command
* For valid pipelines (and this assignment assumes valid
* pipeline inputs) only the first and last pipeline 
* commands can have input and output redirection, respectively. 
*/ 
void setup_redirection(const struct pipeline_command *pcmd) {
    /*
    * Check if the pipeline_command contains a redirect in.
    * If it does, open the file and use dup2() to write the
    * file contents into stdin. Then execute execv(). 
    */
    if (pcmd->redirect_in_path != NULL) {
        if (rd_from_pipe) {
            perror("ERROR: Redirection error"); 
        }
        int fd_in = open(pcmd->redirect_in_path, O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH); // Don't use O_RDONLY here
        dup2(fd_in, 0);  // Points stdin file descriptor to fd_in file
        close(fd_in);    // Release fd_in file descriptor
    }
    
    /*
    * Check if the pipeline_command contains a redirect out.
    * If it does, open the file and use dup2() to direct stdout 
    * of execv() to the file. 
    */
    if (pcmd->redirect_out_path != NULL) {
        if (pcmd->next != NULL) {
            perror("ERROR: Redirection error"); 
        }
        int fd_out = open(pcmd->redirect_out_path, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IWGRP | S_IWOTH);
        dup2(fd_out, 1);
        close(fd_out);  // Release fd_out file descriptor
    }
    
}

/*
* Read from pipe file for input to current command, and create
* a new pipe for interprocess communication with the next command. 
*/ 
void setup_pipe(const struct pipeline_command *pcmd, pid_t parent_pid) {
    /*
    * Check if the pcmd is either a middle or final command in the pipeline. 
    * If pcmd is the first command in the pipeline, create a new pipe file, 
    * point stdout to new pipe WR FD, set rd_pipefd to new pipe RD FD,
    * and set rd_from_pipe to true. 
    * If pcmd is the middle or last command in the pipeline, point stdin to 
    * rd_pipefd, and do the same steps as the first command in the pipeline. 
    */
    
    if (rd_from_pipe) {    /* pcmd is a middle or final command in the pipeline */ 
        dup2(current_rd_pipefd, 0);  // Point stdin to pipe file
        close(current_rd_pipefd);    // Release old pipe RD FD
    }

    if (pcmd->next != NULL) {
        dup2(wr_pipefd, 1);     // Write the stdout of the command to the pipe file
    }
    
    close(wr_pipefd);       // Release the pipe WR FD because it is no longer needed
    
}

void execute_cmds(const struct pipeline *pipeline) 
{
    struct pipeline_command *pcmd; // Initialize pipeline_command pointer
    pcmd = pipeline->commands; // Point pcmd to first command in the linked list
    pid_t parent_pid = getpid(); 

    while (pcmd != NULL) {
        /*
        * Create a new pipe file, pipefd[0] = RD, pipefd[1] = WR
        */ 
        
        int pipefd[2]; 
        if (pipe(pipefd) != 0) {  // pipe() returns 0 if success and -1 if failed
            perror("ERROR: Pipe creation was unsuccessful");
        }
        next_rd_pipefd = pipefd[0]; 
        wr_pipefd = pipefd[1]; 
        
        
        /*
        * Fork and execute commands in a while loop
        */ 
        int status; 
        pid_t child_pid = fork();
        if (child_pid > 0) { /* Parent Process */ 
            close(wr_pipefd);  // Close write end of the pipe
            if (!(pipeline->is_background)) {   // If is_background is false, wait for the child process to terminate
                waitpid(child_pid, &status, 0);
            }

            close(current_rd_pipefd);  // Close read end of the pipe
            current_rd_pipefd = next_rd_pipefd; // Set current RD FD
            if (!rd_from_pipe) {
                rd_from_pipe = true; 
            }
                        
            pcmd = pcmd->next; 
         
        } else if (child_pid == 0) { /* Child Process */  
            close(next_rd_pipefd);
            
            setup_redirection(pcmd); 
            setup_pipe(pcmd, parent_pid); 
            
            /*
            * Check if the first command args has a forward slash. If it does, it 
            * indicates that the command arg contains directory information and 
            * need to use execv. Else, use execvp to search PATH environment. 
            */  
            int exec_status; 
            if (strchr(pcmd->command_args[0], '/') == NULL) {
                // Forward slash is not present in command_args[0] - use execvp
                exec_status = execvp(pcmd->command_args[0], pcmd->command_args);
            } else {
                // Foward slash is present in command_args[0] - use execv 
                exec_status = execv(pcmd->command_args[0], pcmd->command_args);
            }
            
            if (exec_status == -1) {
                perror("ERROR: try");
            }
//             printf("GOT ERROR\n");
//             if (errno == EACCES) {
//                 perror("access error!");
//             }
            exit(EXIT_FAILURE); 

        } else {
            perror("ERROR: Fork was unsuccessful");
            exit(EXIT_FAILURE);
        }
        
    }
//     exit(EXIT_SUCCESS);
}

void repl_cmds(bool print_prompt) {
    // while signal not received - implement with while loop
    char *input_line; 
    struct pipeline *pb; 
    
    while (true) {
        if (print_prompt) {
            printf("my_shell$"); 
        }
        input_line = NULL; 
        input_line = malloc(sizeof(char)* MAX_LINE_LENGTH);
        input_line = fgets(input_line, MAX_LINE_LENGTH, stdin);
        if (input_line != NULL) {
            pb = pipeline_build(input_line); 
            execute_cmds(pb); 
            free(input_line);
            free(pb); 
//         pipeline_free(pb); 
        } else {
            break;
        }
    }
    
}

int main(int argc, char *argv[]) {
	// Run shell as empty prompt
	if (argc == 1) {   /* Run with "my_shell$" prompt */
        repl_cmds(true);
	} 
	else if (argc == 2 && strcmp(argv[1], "-n") == 0) {  /* Run without prompt */  
        repl_cmds(false); 
	} 
	else {
		// error handling for argc > 2
        perror("ERROR: Invalid environment variables passed to shell.");
	}


}
