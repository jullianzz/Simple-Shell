#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


void add_to_tokens(char *beg, char *end, char *tokens_ds[], int *rtc) {

	if (beg == end) {	// beg == end if they both point to a &, |, <, or > symbol
		char *hptr = malloc(sizeof *hptr * 1);  	// hptr stands for heap pointer
		char symbol[2];
		symbol[0] = *end; 
		symbol[1] = '\0';
		strcpy(hptr, symbol);
		tokens_ds[*rtc] = hptr; 		// Add symbol to tokens_ds
		// printf("symbol is %s\n", symbol);
		printf("rtc is %d\n", *rtc);
		printf("Token added: %s\n", tokens_ds[*rtc]);
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
		char *hptr = malloc(sizeof *hptr * 1);  	// hptr stands for heap pointer
		char symbol[2];
		symbol[0] = *end; 
		symbol[1] = '\0';
		strcpy(hptr, symbol);

		/*
		* Add word and symbol to tokens_ds
		*/
		*end = '\0'; 					// Make end NUL to give the word a NUL terminator
		char *hptr_beg = malloc(sizeof *hptr * 1);  	// hptr stands for heap pointer. NOTE: length = 1 here, this could cause problems
		strcpy(hptr_beg, beg); 
		tokens_ds[*rtc] = hptr_beg; 			// Add beg to tokens_ds
		printf("rtc is %d\n", *rtc);
		printf("Token added: %s\n", tokens_ds[*rtc]);
		*rtc = *rtc + 1; 				// Increment rtc by 1
		tokens_ds[*rtc] = hptr; 		// Add symbol to tokens_ds
		printf("rtc is %d\n", *rtc);
		printf("Token added: %s\n", tokens_ds[*rtc]);
		*rtc = *rtc + 1; 				// Increment rtc by 1
	}
}


void tokenize_symbols(char ptr[], char *tokens_ds[], int *rtc) 
{
	/*
	* Check if any of &, |, <, or > symbols exist
	*/ 

	if (strchr(ptr, '&') == NULL && strchr(ptr, '|') == NULL && strchr(ptr, '<') == NULL && strchr(ptr, '>') == NULL) {
		// If none of the symbols exist, add ptr to tokens_ds and return
		char *hptr = malloc(sizeof *hptr * strlen(ptr));
		strcpy(hptr, ptr); 
		tokens_ds[*rtc] = hptr; 
		printf("rtc is %d\n", *rtc);
		printf("Token added: %s\n", tokens_ds[*rtc]);
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
				// TODO: problem lies here. The program doesn't come here even though it should??
				add_to_tokens(beg, end, tokens_ds, rtc); 
				end = end + 1; 
				beg = end; 
			} else
				end = end + 1; 
		}

		if (*beg != '\0') {
			add_to_tokens(beg, end, tokens_ds, rtc); 
		}

	}
}

struct pipeline *pipeline_build(const char *command_line)
{
	const int cmdl_len = strlen(command_line); 	// Find size of command_line string
	int rtc = 0; 	// rtc stands for running tokens count, it increments by 1 after a token pointer is added to tokens_ds

	/*
	* Initialize array of char pointers (tokens_ds) to point to tokens and
	* allocate maximum possible memory needed
	*/
	// char **tokens_ds = (char **) malloc(cmdl_len * sizeof(char *));
	char **tokens_ds = malloc(sizeof **tokens_ds * cmdl_len);

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
	char delim[] = " "; 					
	char *ptr = strtok(cmdl_cpy, delim); 		// Make first call to strtok with parameters cmdl_cpy and delim 
	while (ptr != NULL) {
		/*
		* Call tokenize_symbols on ptr which checks if any symbols &, |, <, or > exist
		* in ptr, and if so add them to tokens_ds
		*/
		printf("%s\n", ptr);
		char ptr_cpy[strlen(ptr)]; 
		strcpy(ptr_cpy, ptr);
		tokenize_symbols(ptr_cpy, tokens_ds, &rtc); 
		printf("Token[0] added: %s\n", tokens_ds[0]);
		printf("Token[1] added: %s\n", tokens_ds[1]);
		printf("Token[2] added: %s\n", tokens_ds[2]);
		ptr = strtok(NULL, delim);
	}

	printf("\n");
	// printf("YASSS Token added: %s\n", tokens_ds[0]);
	// for (int i = 0; i < sizeof(tokens_ds)/sizeof(tokens_ds[0]); i++) {
	// 	printf("%s\n", tokens_ds[i]);
	// }
	// printf("\n");

	/*
	* Shrink size of tokens_ds
	*/ 

	/*
	* Solve the parsing problem
	*/ 
	return NULL;
}

void pipeline_free(struct pipeline *pipeline)
{
	// TODO: Implement this function
}

void rahrah(char *tokens_ds[]) {
	// char *ptr; 
	char str[] = "jelly";
	// ptr = str; 

	char *ptr = malloc(sizeof *ptr * 1); 
	strcpy(ptr, str);
	tokens_ds[0] = ptr; 
	printf("Inside rahrah: tokens_ds is %s\n", tokens_ds[0]); 
}

struct pipeline *pipeline_build_too(const char *command_line) {

	return NULL; 
}

int main() {
	// code here to test pipeline_build
	// pipeline_build("julia <zeng"); 

	// correct output: 
	// julia
	// <
	// zeng

	char **tokens_ds = malloc(sizeof **tokens_ds * 1);
	char *ptr = malloc(sizeof *ptr * 1); 
	char str[] = "julia"; 
	strcpy(ptr, str);
	printf("ptr is %s\n", ptr);
	char str_too[] = "zeng";
	strcpy(ptr, str_too);
	printf("ptr is %s\n", ptr);
	// printf("Before rahrah: tokens_ds is %s\n", tokens_ds[0]); 
	// rahrah(tokens_ds);
	// printf("After rahrah: tokens_ds is %s\n", tokens_ds[0]); 

}





