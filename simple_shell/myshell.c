#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

// class myshell {

// public: 
// 	int run_with_prompt; 
// 	myshell(int rwp) {
// 		this.run_with_prompt = rwp; 
// 	}

// }

void execute_cmds(const struct pipeline *pipeline) 
{
    pid_t pid; 
    int status;
    // char *args[2];  // omit for now. using this would require copying from command_args array
    // Simple design that does not account for redirection for now :  
    // iterate through each pipeline_command in pipeline and execute each command using execve
    struct pipeline_command *pcmd; // Initialize pipeline_command pointer
    pcmd = pipeline->commands; // Point pcmd to first command in the linked list
    // pcmd = pcmd>next; 
    while (pcmd != NULL) {
        if ((pid = fork()) > 0) {
            /* Parent Process */ 
            waitpid(pid, &status, 0);   // Unsure what all these arguments do, but this waits for the child process
            exit(status);   // I think this exits out of the parent process, unsure
         
        } else {
            /* Child Process */  
            /*
            * String Processing Step to create execv path argument:
            * Check if the string has a forward slash. If it does, it 
            * indicates that the string contains directory information and there
            * is no need to concatenate command_args[0] with a directory string. 
            */  
            if (strchr(pcmd->command_args[0], '/') != NULL) {
                /* 
                * Forward slash is present and path to binary is specified in command 
                * input - do not need create path
                */
                execv(pcmd->command_args[0], pcmd->command_args);   // TODO: the argv used here might not include command_args[0] 
            } else {
                /*
                * Forward slash is not present and string manipulation on 
                * command_args[0] is needed
                */ 
                char *hptr = (char *) malloc(sizeof(char) * (strlen(pcmd->command_args[0])));  	// hptr stands for heap pointer
                strcpy(hptr, pcmd->command_args[0]);    // Make a copy of command _args[0]
                char directory[] = "/bin";              // Initialize the directory string which is "/bin" for now        
                strcat(directory, hptr);                // Concatenate "/bin" and the command arg
                execv(directory, pcmd->command_args);   // TODO: the argv used here might not include command_args[0] 

            }
        }
    }
}


int main(int argc, char *argv[]) {
	// Run shell as empty prompt
	if (argc == 1) { 
		// instantiate myshell 
		myshell(1); 
	} 
	// Run with 'my_shell$' prompt
	else if (argc == 2) {
		// instantiate myshell 
		myshell(0); 
	} 
	else {
		// error handling goes here for argc > 2
	}


}