#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <assert.h>


void split_symbols(char *beg, char *end, char *tokens_ds[], int *rtc) {

	if (beg == end) {	// beg == end if they both point to a &, |, <, or > symbol
		char *hptr = (char *) malloc(sizeof(char) * 2);  	// hptr stands for heap pointer
		char symbol[2];
		symbol[0] = *end; 
		symbol[1] = '\0';
		strcpy(hptr, symbol);
		tokens_ds[*rtc] = hptr; 		// Add symbol to tokens_ds
		*rtc = *rtc + 1; 				// Increment rtc by 1
	} 
	
	/* 
	* If the program reaches this point, the set of characters starting  
	* from the beg pointer and ending at the end pointer are either: (1) one word
	* followed by a symbol (pointed to by end), or (2) one word followed by a 
	* NUL (pointed to by end).
	*/

	else { 			
		/*
		* Copy symbol (will be &, |, <, >, or NUL) end is pointing to 
		* into a char array called symbol
		*/ 
		char *hptr = (char *) malloc(sizeof(char)* 2);  	// hptr stands for heap pointer
		*hptr = '\0';

		if (*end != '\0') {
			char symbol[2];
			symbol[0] = *end; 
			symbol[1] = '\0';
			strcpy(hptr, symbol);
		}
		/*
		* Add word and symbol to tokens_ds
		*/
		*end = '\0'; 					// Make end NUL to give the word a NUL terminator
		char *hptr_beg = (char *) malloc(sizeof(char) * (strlen(beg) + 1));  	// hptr stands for heap pointer
		strcpy(hptr_beg, beg); 
		tokens_ds[*rtc] = hptr_beg; 		// Add beg to tokens_ds
		*rtc = *rtc + 1; 					// Increment rtc by 1
		if (*hptr != '\0') {
			tokens_ds[*rtc] = hptr; 		// Add symbol to tokens_ds
			*rtc = *rtc + 1; 				// Increment rtc by 1
		}
	}
}


void tokenize(char ptr[], char *tokens_ds[], int *rtc) 
{

	/*
	* Check if any of &, |, <, or > symbols exist.
	* If they don't just add ptr to tokens_ds.
	* If they do, continue tokenizing.
	*/ 

	if (strchr(ptr, '&') == NULL && strchr(ptr, '|') == NULL && strchr(ptr, '<') == NULL && strchr(ptr, '>') == NULL) {
		// If none of the symbols exist, add ptr to tokens_ds and return
		char *hptr = (char *) malloc(sizeof(char) * (strlen(ptr) + 1));
		strcpy(hptr, ptr); 
		tokens_ds[*rtc] = hptr; 
		*rtc = *rtc + 1; 		// Increment rtc by 1
		return; 
	} 
	else { 		// At least one of the symbols &, |, <, or > exist
		/* 
		* Search ptr for words and special symbols: &, |, <, or >.
		* Search using two-pointers algorithm (similar to C's strtok implementation)
		*/ 
		char *beg;		// Purpose of beg is to point to the beginning of a token
		char *end; 		// Purpose of end is to point to the end of a token
		beg = ptr; 
		end = ptr; 

		while (*end != '\0') {
			if (*end == '&' || *end == '|' || *end == '>' || *end == '<') {
				split_symbols(beg, end, tokens_ds, rtc); 
				end = end + 1; 
				beg = end; 
			} else
				end = end + 1; 
		}

		if (*beg != '\0') {
			split_symbols(beg, end, tokens_ds, rtc); 
		}

	}
}

struct pipeline_command *new_pipeline_cmd() 
{
	struct pipeline_command *cmd_ptr = (struct pipeline_command *) malloc(sizeof(struct pipeline_command) * 1);
	cmd_ptr->command_args[0] = NULL; 
	cmd_ptr->redirect_in_path = NULL;
	cmd_ptr->redirect_out_path = NULL;
	cmd_ptr->next = NULL;

	return cmd_ptr;
}

struct pipeline *new_pipeline() 
{
	struct pipeline *pipeline = (struct pipeline *) malloc(sizeof(struct pipeline) * 1);
	pipeline->commands = NULL;
	pipeline->is_background = false; 

	return pipeline; 
}

struct pipeline *pipeline_build(const char *command_line)
{
	printf("Command Line: %s", command_line);

	/*******************************************************************************************
	********************************** Solve the Lexing Problem ********************************
	********************************************************************************************/

	const int cmdl_len = strlen(command_line) + 1; 	// Find size of command_line string
	int rtc = 0; 	// rtc stands for running tokens count, it increments by 1 after a token pointer is added to tokens_ds

	/*
	* Initialize array of char pointers (tokens_ds) to point to tokens and
	* allocate maximum possible memory needed
	*/
	char **tokens_ds = (char **) malloc(sizeof(char *) * (cmdl_len + 1));

	/*
	* Make non-const copy of command_line for the purpose of using with strtok which 
	* modifies the string parameter passed to it
	*/
	char cmdl_cpy[cmdl_len]; 			
	strcpy(cmdl_cpy, command_line);

	/*
	* Remove the newline character
	*/ 
	char *newl_ptr; 
	if ((newl_ptr = strchr(cmdl_cpy, '\n')) != NULL) {
		*newl_ptr = '\0';
	}
	
	/* 
	* Split cmdl_cpy by whitespace by calling strtok iteratively until the strtok's 
	* next pointer points to NULL 
	*/
	char delim[] = " "; 					
	char *ptr = strtok(cmdl_cpy, delim); 		// Make first call to strtok with parameters cmdl_cpy and delim 
	while (ptr != NULL) {
		/*
		* Call tokenize on ptr which checks if any symbols &, |, <, or > exist
		* in ptr, and if so add them to tokens_ds
		*/
		tokenize(ptr, tokens_ds, &rtc); 
		ptr = strtok(NULL, delim);
	}

	/* 
	* After while loop finishes, the tokens_ds data stucture holds 
	* pointers to each token in the command line
	*/ 
	printf("\nPRINT TOKENS DATA STRUCTURE:\n");

	for (int i = 0; i < rtc; i++) {
		printf("%s\n", tokens_ds[i]);
	}
	printf("\n");


	/*******************************************************************************************
	******************************** Solve the parsing problem *********************************
	*******************************************************************************************/

	/*
	* Initialize pipeline with NULL fields
	*/ 
	struct pipeline *pipeline = new_pipeline(); 
 
	/* 
	* Create the first pipeline_command object and initialize a
	* pipeline_command iterator (pcmds_iterator) 
	*/ 
	pipeline->commands = new_pipeline_cmd();
	struct pipeline_command *pcmds_iterator = pipeline->commands; 

	/*
	* Parse tokens_ds to build pipeline struct
	*/ 	
	int tokens_ds_idx = 0; 
	int cmd_args_idx = 0; 
	while (tokens_ds_idx != rtc) {
		switch (*tokens_ds[tokens_ds_idx]) {

			/* Token '|' indicates the end of a command */ 
			case '|':
				/* Create new pipeline_command and assign current pipeline_command next to new pipeline_command */
				pcmds_iterator->next = new_pipeline_cmd(); 
				pcmds_iterator = pcmds_iterator->next; 		// Point pcmds_iterator to new pipeline_command
				cmd_args_idx = 0; 							// Reset command args index to 0
				break;

			/* Token '&' indicates the pipeline runs in the background */ 
			case '&': 	
				pipeline->is_background = true; 
				break; 

			/* Token '<' indicates a redirect in path */
			case '<':
				tokens_ds_idx++; 							// Increment tokens_ds_idx to get the next word token
				pcmds_iterator->redirect_in_path = tokens_ds[tokens_ds_idx]; 
				break;

			/* Token '>' indicates a redirect out path */
			case '>':
				tokens_ds_idx++; 							// Increment tokens_ds_idx to get the next word token
				pcmds_iterator->redirect_out_path = tokens_ds[tokens_ds_idx]; 
				break; 

			/* Default indicates the token is a non-symbol word */
			default: 
				pcmds_iterator->command_args[cmd_args_idx] = tokens_ds[tokens_ds_idx]; 	// Overwrite with NULL
				cmd_args_idx++; 							// Increment cmmd_args_idx
				pcmds_iterator->command_args[cmd_args_idx] = NULL; // Pre-write next one with NULL

		}
		tokens_ds_idx++; 	// Incremement the tokens_ds index
	}

	return pipeline;
}

void pipeline_free(struct pipeline *pipeline)
{
	// TODO: Implement this function
}





