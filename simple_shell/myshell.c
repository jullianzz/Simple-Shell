#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/wait.h"
#include "stdbool.h"

bool rd_from_pipe = false; // rd_from_pipe is true for all commands in the pipeline, except for the first
int rd_pipefd; // File descriptor of pipe file to read from for input to current command 

/*
* Point stdin and stdout to different files if input or output 
* redirection paths is not null in pipeline_command
* For valid pipelines (and this assignment assumes valid
* pipeline inputs) only the  first and last pipeline 
* commands can have input and output redirection, respectively. 
*/ 
void setup_redirection(const struct pipeline_command *pcmd) {
    /*
    * Check if the pipeline_command contains a redirect in.
    * If it does, open the file and use dup2() to write the
    * file contents into stdin. Then execute execv(). 
    */
    int fd_in; 
    if (pcmd->redirect_in_path != NULL) {  // Need to open redirect in file for reading
        fd_in = open(pcmd->redirect_in_path, O_RDONLY); // Don't use O_RDONLY here
        dup2(fd_in, 0);  // Points stdin file descriptor to fd_in file
    }
    close(fd_in);     
    
    /*
    * Check if the pipeline_command contains a redirect out.
    * If it does, open the file and use dup2() to direct stdout 
    * of execv() to the file. 
    */
    int fd_out;
    if (pcmd->redirect_out_path != NULL) {  // Need to open redirect out file for writing 
        fd_out = open(pcmd->redirect_out_path, O_WRONLY | O_CREAT | O_TRUNC); // Add O_CREAT flag if needed, need to check out specifications
        dup2(fd_out, 1);
    }
    close(fd_out);  // With dup2, stdout will be written to the file pointed to by fd. Can close fd.
}

// Set up a new pipe for interprocess communication with the next pipeline command. 
void setup_pipe(const struct pipeline_command *pcmd) {
    /*
    * Check if the pcmd is either a middle or final command in the pipeline. 
    * If pcmd is the first command in the pipeline, create a new pipe file,
    * point stdout to new pipe WR FD, set rd_pipefd to new pipe RD FD,
    * and set rd_from_pipe to true. 
    * If pcmd is the middle or last command in the pipeline, point stdin to 
    * rd_pipefd, and do the same steps as the first command in the pipeline. 
    */

    if (rd_from_pipe) {    /* pcmd is a middle or final command in the pipeline */ 
        dup2(rd_pipefd, 0);  // Point stdin to pipe file
        close(rd_pipefd);    // Release old pipe RD FD
 
    } else {    /* pcmd is the first command in the pipeline */ 
        rd_from_pipe = true;   // Set rd_from_pipe to true to signal pipe read for next command
    }
    
    int pipefd[2]; 
    pipe(pipefd); // Create a new pipe file, pipefd[0] = RD, pipefd[1] = WR
    rd_pipefd = pipefd[0];    // Set rd_pipefd to the new pipe RD FD
    dup2(pipefd[1], 1);     // Write the stdout of the command to the pipe file
    close(pipefd[1]);       // Close the pipe WR FD because it is no longer needed
    
}


// Call execute_cmds after pipeline_build
void execute_cmds(const struct pipeline *pipeline) 
{
    // printf("yass");
    int status;
    // char *args[2];  // omit for now. using this would require copying from command_args array
    // Simple design that does not account for redirection for now :  
    // iterate through each pipeline_command in pipeline and execute each command using execve
    struct pipeline_command *pcmd; // Initialize pipeline_command pointer
    pcmd = pipeline->commands; // Point pcmd to first command in the linked list

    while (pcmd != NULL) {
        pid_t pid = fork(); 
        if (pid > 0) { /* Parent Process */ 
            // printf("PARENT process pid is %d \n", pid);
            waitpid(0, &status, 0);
            pcmd = pcmd->next; 
            // exit(status);   // I think this exits out of the parent process, unsure
         
        } else if (pid == 0) { /* Child Process */  
            /*
            * String Processing Step to create execv path argument:
            * Check if the string has a forward slash. If it does, it 
            * indicates that the string contains directory information and there
            * is no need to concatenate command_args[0] with a directory string. 
            */  
            // printf("CHILD process pid is %d\n", pid);
            if (strchr(pcmd->command_args[0], '/') == NULL) {
                /*
                * Forward slash is not present and string manipulation on 
                * command_args[0] is needed
                */
                char directory[] = "/bin/";              // Initialize the directory string which is "/bin/" for now  
                char *hptr = (char *) malloc(sizeof(char) * (strlen(pcmd->command_args[0]) + strlen(directory) + 1));   // +1 for nul
                hptr = directory;
                strcat(hptr, pcmd->command_args[0]);
                pcmd->command_args[0] = hptr; 
                // printf("the new directory is %s\n", pcmd->command_args[0]);
            }

            setup_redirection(pcmd); 
            setup_pipe(pcmd); 
            execv(pcmd->command_args[0], pcmd->command_args);

            exit(EXIT_SUCCESS); 

        } else {
            perror("Fork was unsuccessful");
            exit(EXIT_FAILURE);
            // pid is negative and fork was unsuccessful
        }
    }
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
	// Run shell as empty prompt
	if (argc == 1) { 
		// instantiate myshell 
		// myshell(1); 
        // printf("hereee___");
        // struct pipeline *pb = pipeline_build("/bin/ls\n"); 
        // struct pipeline *pb = pipeline_build("ls\n");
        // struct pipeline *pb = pipeline_build("ls > outfile.txt\n"); 
        // struct pipeline *pb = pipeline_build("ls -al | cat garbo_file.txt\n"); 
//         struct pipeline *pb = pipeline_build("ls -al > outfile.txt\n"); 
//         struct pipeline *pb = pipeline_build("ls -al > outfile.txt | cat garbo_file.txt\n"); 
//         struct pipeline *pb = pipeline_build("echo wowza > outfile.txt \n"); 
//         struct pipeline *pb = pipeline_build("cat < garbo_file.txt > outfile.txt\n"); 
        struct pipeline *pb = pipeline_build("ls -al | wc -l\n");
        // printf("hereee***");
        execute_cmds(pb); 
        // printf("hereeeBLEEEEEP");
	} 
	else if (argc == 2) {
        // Run with 'my_shell$' prompt
        // Check if argv[1] == "-n"
		// instantiate myshell 
		// myshell(0); 
	} 
	else {
		// error handling goes here for argc > 2
	}


}
