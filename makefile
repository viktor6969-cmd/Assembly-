FLAGS = gcc -Wall -pedantic -ansi
TARGET = myprog
SCRS = open_macros.c main.c commands_sort.c validationCheck.c first_compile.c
OBJS = $(SCRS:.c=.o)
HDRS = header.h

$(TARGET): $(OBJS)
	$(FLAGS) -o $@ $^
%.o: %.c $(HDRS)
	$(FLAGS) -c -o $@ $<

clean:
	rm -f *.o myprog *.am *.ob *.ext *.ent
