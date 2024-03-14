FLAGS = gcc -Wall -pedantic -ansi
TARGET = myprog
SCRS = precompile.c transleter.c main.c validationCheck.c 
OBJS = $(SCRS:.c=.o)
HDRS = header.h

$(TARGET): $(OBJS)
	$(FLAGS) -o $@ $^
%.o: %.c $(HDRS)
	$(FLAGS) -c -o $@ $<

clean:
	rm -f *.o myprog output.txt
