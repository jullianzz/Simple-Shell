#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"

// Call execv with redirection
void call_and_redirect(const struct pipeline_command *pcmd) {
    /*
    * Check if the pipeline_command contains a redirect out.
    * If it does, open the file and use dup2() to direct stdout 
    * of execv() to the file. 
    */
    int fd;
    if (pcmd->redirect_out_path != NULL) {  // Need to open redirect out file for writing 
        fd = open(pcmd->redirect_out_path, O_WRONLY | O_CREAT); // Add O_CREAT flag if needed, need to check out specifications
        dup2(fd, 1);
    }
    close(fd);  // With dup2, stdout will be written to the file pointed to by fd. Can close fd.
    execv(pcmd->command_args[0], pcmd->command_args);
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

            call_and_redirect(pcmd); 

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
        struct pipeline *pb = pipeline_build("ls -al > outfile.txt\n"); 
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