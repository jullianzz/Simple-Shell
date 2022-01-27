
#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TEST_ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, "%s:%d: Assertion (%s) failed!\n", \
				__FILE__, __LINE__, #x); \
	       	abort(); \
	} \
} while(0)

int
main(void) 
{
	struct pipeline * my_pipeline = pipeline_build("ls | mkdir token1 | donuts > file.txt &");
    struct pipeline_command * curr_ptr = my_pipeline -> commands;
    
	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("ls", curr_ptr->command_args[0]) == 0);
	TEST_ASSERT(strcmp("mkdir", curr_ptr->command_args[1]) == 0);
	TEST_ASSERT(strcmp("token1", curr_ptr->command_args[2]) == 0);
                    
    // checking for the elements after the pipe
    curr_ptr = curr_ptr -> next;
	TEST_ASSERT(strcmp("donuts", curr_ptr->command_args[0]) == 0);  
	TEST_ASSERT(curr_ptr->command_args[1] == NULL);  
    
	// Test the redirect state (should be null for both commands)
	TEST_ASSERT(my_pipeline->commands->redirect_in_path == NULL);
	TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);
	TEST_ASSERT(curr_ptr->redirect_in_path == NULL);
	TEST_ASSERT(strcmp("file.txt", curr_ptr->redirect_out_path) == 0);
                   
	// Test that there is only one parsed command in the pipeline
	TEST_ASSERT(curr_ptr->next == NULL);
    
	pipeline_free(my_pipeline);
}