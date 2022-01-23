#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


void tokenize_symbols(char *ptr, char* tokens_ds, int *rtc) 
{
	/*
	* Check if any of &, |, <, or > symbols exist
	*/ 
	if (strchr(ptr, "&") == NULL && strchr(ptr, "|") == NULL && strchr(ptr, "<") == NULL && strchr(ptr, ">") == NULL) {
		// If none of the symbols exist, add ptr to tokens_ds and return
		*(tokens_ds + rtc) = ptr;			// TODO: fix this. Need to have the char pointer from tokens_ds point to ptr
		rtc++; 			// Increment rtc by 1
		return; 
	}


	// If they do continue splitting
}

struct pipeline *pipeline_build(const char *command_line)
{
	const int cmdl_len = strlen(command_line); 	// Find size of command_line string
	int rtc = 0; 	// Running tokens count, it is incremented after a token is added to tokens_ds

	/*
	* Initialize array of char pointers (tokens_ds) to hold tokens
	*/
	char *tokens_ds = (char *) malloc(cmdl_len * sizeof(char *)); 


	/*
	* Make non-const copy of command_line for the purpose of using with strtok which 
	* modifies the string parameter passed to it
	*/
	char cmdl_cpy[cmdl_len]; 					// Initialize char[] of same length as command_line
	strcpy(cmdl_cpy, command_line); 			// Copy command_line into cmdl_cpy
	
	/* 
	* Split cmdl_cpy by whitespace by calling strtok iteratively until the strtok's 
	* next pointer points to NULL 
	*/
	char delim[] = " "; 						// Define delim to be " "	
	char *ptr = strtok(cmdl_cpy, delim); 		// Make first call to strtok with parameters cmdl_cpy and delim 
	// printf("%s\n", cmdl_cpy);
	while (ptr != NULL) {
		printf("%s\n", ptr); 
		/*
		* Call tokenize_symbols on ptr which checks if any symbols &, |, <, or > exist
		* in pt, and if so add them to tokens_ds
		*/
		tokenize_symbols(ptr, tokens_ds, &rtc); 
		ptr = strtok(NULL, delim);
	}

	for (int i = 0; i < cmdl_len; i++)
	{
		printf("%d ", cmdl_cpy[i]); /* Convert the character to integer, in this case
							   the character's ASCII equivalent */
	}
	printf("\n");
	// Solve the parsing problem
	return NULL;
}

void pipeline_free(struct pipeline *pipeline)
{
	// TODO: Implement this function
}


int main() {
	// code here to test pipeline_build
	// pipeline_build("julia  < zeng");
	pipeline_build("julia  zeng  ");
}