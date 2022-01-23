#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


void add_to_tokens(char *beg, char *end, char *tokens_ds[], int *rtc) {
	char end_cpy[strlen(end)]; 
	strcpy(end_cpy, end); 			// Make copy of end
	*end = '\0'; 					// Make end a null character
	tokens_ds[*rtc] = beg; 			// Add beg to tokens_ds
	*rtc = *rtc + 1; 				// Increment rtc by 1
	tokens_ds[*rtc] = end_cpy; 		// Add end copy to tokens_ds
	// Now beg points at original beg, end points at null character
}


void tokenize_symbols(char ptr[], char *tokens_ds[], int *rtc) 
{
	/*
	* Check if any of &, |, <, or > symbols exist
	*/ 
	if (strchr(ptr, '&') == NULL && strchr(ptr, '|') == NULL && strchr(ptr, '<') == NULL && strchr(ptr, '>') == NULL) {
		// If none of the symbols exist, add ptr to tokens_ds and return
		tokens_ds[*rtc] = ptr; 
		*rtc = *rtc + 1; 		// Increment rtc by 1
		return; 
	} 
	else { 		// If they do continue splitting
		if (strchr(ptr, '&') != NULL) {
			/* 
			* Tokenize ptr for '&' and words
			* Search using two-pointers algorithm (similar to strtok implementation under-the-hood)
			*/ 
			char *beg;		// Purpose of beg is to point to the beginning of a token
			char *end; 		// Purpose of end is to point to the end of a token
			beg = ptr; 
			end = ptr; 

			while (end != NULL) {
				if (strcmp(end, "&") == 0 || strcmp(end, "|") == 0 || strcmp(end, "<") == 0 || strcmp(end, ">") == 0) {
					add_to_tokens(beg, end, tokens_ds, rtc); 
					beg = end + 1; 
					end = end + 1; 
				} else
					end = end + 1; 
			}

			if (beg != NULL) {
				add_to_tokens(beg, end, tokens_ds, rtc); 
			}

		}

	}

}

struct pipeline *pipeline_build(const char *command_line)
{
	const int cmdl_len = strlen(command_line); 	// Find size of command_line string
	int rtc = 0; 	// Running tokens count, it is incremented after a token pointer is added to tokens_ds

	/*
	* Initialize array of char pointers (tokens_ds) to point to tokens
	*/
	char **tokens_ds = (char **) malloc(cmdl_len * sizeof(char *)); // Allocate maximum possible memory needed


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
	while (ptr != NULL) {
		/*
		* Call tokenize_symbols on ptr which checks if any symbols &, |, <, or > exist
		* in pt, and if so add them to tokens_ds
		*/
		printf("%s\n", ptr);
		char ptr_cpy[strlen(ptr)]; 
		strcpy(ptr_cpy, ptr);
		tokenize_symbols(ptr_cpy, tokens_ds, &rtc); 
		ptr = strtok(NULL, delim);
	}

	// for (int i = 0; i < sizeof(tokens_ds)/sizeof(tokens_ds[0]); i++) {
	// 	printf("%s\n", tokens_ds[i]);
	// }
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
	// pipeline_build("julia  zeng  ");
	char *end;
	char str[] = "julia"; 
	end = str;
	// if (strcmp(end, "julia") == 0) {
	// 	printf("yes1\n");
	// }
	printf("%s\n", end);
}





