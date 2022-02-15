override CFLAGS := -Wall -Werror -std=gnu99 -O1 -g $(CFLAGS) -I.

all: myshell_parser myshell_signals myshell
    $(CC) $(CFLAGS) myshell_parser.o myshell_signals.o myshell.o -o main
    #./myshell
    
myshell_parser: myshell_parser.c myshell_parser.h
    $(CC) $(CFLAGS) myshell_parser.c -o myshell_parser.o
    
myshell_signals: myshell_signals.c myshell_signals.h
    $(CC) $(CFLAGS) myshell_signals.c -o myshell_signals.o
    
myshell: myshell.c myshell_parser.o myshell_signals.o
    $(CC) $(CFLAGS) myshell_signals.c -o myshell.o
    

clean:
	rm -f *.o
