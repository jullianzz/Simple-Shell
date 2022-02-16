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


bool rd_from_pipe = false;   // Define and initialize rd_from_pipe global variable
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
//         printf("%s doing this shit\n", pcmd->command_args[0]);
        int fd_in = open(pcmd->redirect_in_path, O_RDONLY); // Don't use O_RDONLY here
//         close(0); 
        dup2(fd_in, 0);  // Points stdin file descriptor to fd_in file
        close(fd_in);    // Release fd_in file descriptor
    }
    
    /*
    * Check if the pipeline_command contains a redirect out.
    * If it does, open the file and use dup2() to direct stdout 
    * of execv() to the file. 
    */
    if (pcmd->redirect_out_path != NULL) {
//         printf("%s doing this shit\n", pcmd->command_args[0]);
        int fd_out = open(pcmd->redirect_out_path, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IWGRP | S_IWOTH);
//         close(1); 
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
//         close(0); 
        dup2(current_rd_pipefd, 0);  // Point stdin to pipe file
        close(current_rd_pipefd);    // Release old pipe RD FD
//         printf("%s command is middle or final command in the pipeline\n", pcmd->command_args[0]); 
 
    } else {    /* pcmd is the first command in the pipeline */ 
//         printf("%s command is first command in the pipeline\n", pcmd->command_args[0]); 
    }

    if (pcmd->next != NULL) {
//         printf("%s not the final cmd\n", pcmd->command_args[0]);
//         close(1); 
        dup2(wr_pipefd, 1);     // Write the stdout of the command to the pipe file
    } else {
//         printf("%s is the final cmd\n", pcmd->command_args[0]);
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
        
//         printf("top of while loop\n");

        int pipefd[2]; 
        if (pipe(pipefd) != 0) {  // pipe() returns 0 if success and -1 if failed
            perror("Pipe creation was unsuccessful");
        }
        next_rd_pipefd = pipefd[0]; 
        wr_pipefd = pipefd[1]; 
        
        
        /*
        * Fork and execute commands in a while loop
        */ 
        int status; 
        pid_t child_pid = fork();
        if (child_pid > 0) { /* Parent Process */ 
//             printf("waiting for the child\n");
            close(wr_pipefd);  // Close write end of the pipe
            waitpid(child_pid, &status, 0);

            close(current_rd_pipefd);  // Close read end of the pipe
            current_rd_pipefd = next_rd_pipefd; // Set current RD FD
            if (!rd_from_pipe) {
//                 printf("changed read cond!\n");
                rd_from_pipe = true; 
            }
            
//             printf("HAAIR\n");
            
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
                perror("ERROR: try: No such file or directory");
            }
            exit(EXIT_FAILURE); 

        } else {
            perror("Fork was unsuccessful");
            exit(EXIT_FAILURE);
        }
        
    }
//     printf("exited!\n");
//     exit(EXIT_SUCCESS);
}

const char* read_cmds() {
//     printf("my_shell$");
    char *input_line = (char *) malloc(sizeof(char)* MAX_LINE_LENGTH);
    input_line = fgets(input_line, MAX_LINE_LENGTH, stdin);   

    return input_line; 
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
        pb = pipeline_build(input_line); 
        execute_cmds(pb); 
        free(input_line);
        free(pb); 
//         pipeline_free(pb); 
    }
    
}

int main(int argc, char *argv[]) {
	// Run shell as empty prompt
	if (argc == 1) {   /* Run with "my_shell$" prompt */
        // printf("hereee___");
//         struct pipeline *pb = pipeline_build("/bin/ls\n"); 
//         struct pipeline *pb = pipeline_build("ls\n");
//         struct pipeline *pb = pipeline_build("ls > outfile.txt\n"); 
        // struct pipeline *pb = pipeline_build("ls -al | cat garbo_file.txt\n"); 
//         struct pipeline *pb = pipeline_build("ls -al > outfile.txt\n"); 
//         struct pipeline *pb = pipeline_build("ls -al > outfile.txt | cat garbo_file.txt\n"); 
//         struct pipeline *pb = pipeline_build("echo wowza > outfile.txt \n"); 
//         struct pipeline *pb = pipeline_build("cat < garbo_file.txt > outfile.txt\n"); 
//         struct pipeline *pb = pipeline_build("ls -al | wc -l\n"); // this command tests piping
//         struct pipeline *pb = pipeline_build("ls -al | wc\n");
//         struct pipeline *pb = pipeline_build("wc \n"); // Use this instruction to test signal and kill
//             struct pipeline *pb = pipeline_build("ls -al | ls -al\n");
//         struct pipeline *pb = pipeline_build("ls -al | wc -l > outfile.txt \n");
//         struct pipeline *pb = pipeline_build("ls -l | more\n");
//         struct pipeline *pb = pipeline_build("sort garbo_file.txt | uniq\n");
//         struct pipeline *pb = pipeline_build("whoami  \n");
//         execute_cmds(pb); 
        
        repl_cmds(true);
	} 
	else if (argc == 2 && strcmp(argv[1], "-n") == 0) {  /* Run without prompt */  
        repl_cmds(false); 
	} 
	else {
		// error handling goes here for argc > 2
        perror("ERROR: Invalid environment variables passed to shell.");
	}


}
