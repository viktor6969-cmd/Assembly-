FLAGS = gcc -Wall -pedantic -ansi
TARGET = assembler
SCRS = open_macros.c main.c commands_operations.c validation_check.c first_read.c utilits.c write_file.c
OBJS = $(SCRS:.c=.o)
HDRS = header.h

$(TARGET): $(OBJS)
	$(FLAGS) -o $@ $^
%.o: %.c $(HDRS)
	$(FLAGS) -c -o $@ $<
clean:
	rm -f *.o myprog *.am *.ob *.ext *.ent
