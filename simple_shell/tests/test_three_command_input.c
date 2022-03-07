#include "myshell_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	struct pipeline* my_pipeline = pipeline_build("ls -a|wc -l < counts.txt | cat counts.txt &\n");

	// Test that a pipeline was returned
	TEST_ASSERT(my_pipeline != NULL);
	TEST_ASSERT(my_pipeline->is_background);
	TEST_ASSERT(my_pipeline->commands != NULL);

	// Test the parsed args
	TEST_ASSERT(strcmp("ls", my_pipeline->commands->command_args[0]) == 0);
    TEST_ASSERT(strcmp("-a", my_pipeline->commands->command_args[1]) == 0);

    TEST_ASSERT(strcmp("wc", my_pipeline->commands->next->command_args[0]) == 0);
    TEST_ASSERT(strcmp("-l", my_pipeline->commands->next->command_args[1]) == 0);

    TEST_ASSERT(strcmp("cat", my_pipeline->commands->next->next->command_args[0]) == 0);

    TEST_ASSERT(my_pipeline->commands->command_args[2] == NULL);
    
    TEST_ASSERT(my_pipeline->commands->next->command_args[2] == NULL);
    
    TEST_ASSERT(my_pipeline->commands->next->next->command_args[2] == NULL);

	// Test the redirect state
    
    TEST_ASSERT(my_pipeline->commands->next->redirect_in_path != NULL);
    
    TEST_ASSERT(strcmp(my_pipeline->commands->next->redirect_in_path, "counts.txt") == 0);
    
    TEST_ASSERT(my_pipeline->commands->redirect_out_path == NULL);
    
	// Test that there is only three parsed commands in the pipeline
	TEST_ASSERT(my_pipeline->commands->next->next->next == NULL);

	pipeline_free(my_pipeline);
    printf("Done");
}