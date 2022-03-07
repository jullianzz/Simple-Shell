# Simple Unix Shell

### Installation and Build
- Download file from the main branch
- cd into simple_shell 
- run makefile with "make"

### How to Use 
1. Option 1: Execute ./myshell to open a shell with the "my_shell$" prompt. 
2. Option 2: Execute ./myshell -n to open a shell with no prompt. 
3. At the prompt enter Unix shell commands. This simple shell is capable of
   piping, file input/output redirection, and background processing.
4. To exit the shell, type Ctrl+D.  

### Sample commands 
- whoami
- pwd
- /bin/pwd
- ls -al
- ls -al | wc -l 
- ls -al |wc -l>lines_in_ls
- cat lines_in_ls