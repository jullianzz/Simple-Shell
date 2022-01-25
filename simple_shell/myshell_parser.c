#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


void split_symbols(char *beg, char *end, char *tokens_ds[], int *rtc) {

	// printf("Beginning is %s\n", beg);

	if (beg == end) {	// beg == end if they both point to a &, |, <, or > symbol
		char *hptr = (char *) malloc(sizeof(char) * 2);  	// hptr stands for heap pointer
		char symbol[2];
		symbol[0] = *end; 
		symbol[1] = '\0';
		strcpy(hptr, symbol);
		tokens_ds[*rtc] = hptr; 		// Add symbol to tokens_ds
		// printf("LOC 1: heap pointer is %s\n", hptr);
		// printf("rtc is %d\n", *rtc)/;
		printf("LOC 1: Token added: %s\n", tokens_ds[*rtc]);
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
		hptr = NULL;

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
		char *hptr_beg = (char *) malloc(sizeof(char) * (strlen(beg) + 1));  	// hptr stands for heap pointer. NOTE: length = 1 here, this could cause problems
		strcpy(hptr_beg, beg); 
		tokens_ds[*rtc] = hptr_beg; 			// Add beg to tokens_ds
		// printf("LOC 2: heap pointer is %s\n", hptr_beg);
		// printf("rtc is %d\n", *rtc);
		printf("LOC 2: Token added: %s\n", tokens_ds[*rtc]);
		*rtc = *rtc + 1; 				// Increment rtc by 1
		if (hptr != NULL) {
			tokens_ds[*rtc] = hptr; 		// Add symbol to tokens_ds
			// printf("LOC 3: heap pointer is %s\n", hptr);
			// printf("rtc is %d\n", *rtc);
			printf("LOC 3: Token added: %s\n", tokens_ds[*rtc]);
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

	printf("\nTokenizing %s\n", ptr); 

	if (strchr(ptr, '&') == NULL && strchr(ptr, '|') == NULL && strchr(ptr, '<') == NULL && strchr(ptr, '>') == NULL) {
		// If none of the symbols exist, add ptr to tokens_ds and return
		char *hptr = (char *) malloc(sizeof(char) * (strlen(ptr) + 1));
		strcpy(hptr, ptr); 
		tokens_ds[*rtc] = hptr; 
		// printf("LOC 4: heap pointer is %s\n", hptr);
		// printf("rtc is %d\n", *rtc);
		printf("LOC 4: Token added: %s\n", tokens_ds[*rtc]);
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
			// printf("End is %s\n", end);
			if (*end == '&' || *end == '|' || *end == '>' || *end == '<') {
				// TODO: problem lies here. The program doesn't come here even though it should??
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

struct pipeline *pipeline_build(const char *command_line)
{
	const int cmdl_len = strlen(command_line) + 1; 	// Find size of command_line string
	int rtc = 0; 	// rtc stands for running tokens count, it increments by 1 after a token pointer is added to tokens_ds

	/*
	* Initialize array of char pointers (tokens_ds) to point to tokens and
	* allocate maximum possible memory needed
	*/
	char **tokens_ds = (char **) malloc(sizeof(char *) * cmdl_len);

	/*
	* Make non-const copy of command_line for the purpose of using with strtok which 
	* modifies the string parameter passed to it
	*/
	char cmdl_cpy[cmdl_len]; 			
	strcpy(cmdl_cpy, command_line);
	
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
		// printf("tokens[0] = %s\n", tokens_ds[0]);
		// printf("tokens[1] = %s\n", tokens_ds[1]);
		// printf("tokens[2] = %s\n", tokens_ds[2]);
		ptr = strtok(NULL, delim);
	}

	printf("\nPRINT TOKENS DATA STRUCTURE:\n");

	for (int i = 0; i < rtc; i++) {
		printf("%s\n", tokens_ds[i]);
	}
	printf("\n");

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


int main() {
	// code here to test pipeline_build
	pipeline_build("Julia |Zeng Chicken>"); // test case 1
	// pipeline_build("Julia < Zeng < Chickens |yar");  // test case 2
	// pipeline_build("Julia Zeng Chickens Yas"); // test case 3

}





