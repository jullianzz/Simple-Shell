#include "myshell_parser.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


class myshell {

public: 
	int run_with_prompt; 
	myshell(int rwp) {
		this.run_with_prompt = rwp; 
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