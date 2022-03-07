# override CFLAGS := -Wall -Werror -std=gnu99 -O1 -g $(CFLAGS) -I.
FLAGS = -Wall -Werror -std=gnu99 -g -O1

all: myshell
	$(CC) $(FLAGS) myshell_signals.c myshell_parser.c myshell.c -o myshell
	#./myshell
    
myshell_parser: myshell_parser.c myshell_parser.h
	touch myshell_parser
    
myshell_signals: myshell_signals.c myshell_signals.h
	touch myshell_signals
    
myshell: myshell.c myshell_parser myshell_signals
	touch myshell

clean:
	rm -f *.o
