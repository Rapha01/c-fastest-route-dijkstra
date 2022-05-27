CC=gcc
CC_FLAGS =-Wall -Werror -g -std=c99
SOURCES=$(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
PROGRAMM=programm

all:$(PROGRAMM)
	
$(PROGRAMM): $(OBJECTS)
		$(CC) $(OBJECTS) -o $(PROGRAMM)


%.o: %.c
	$(CC)  $(CC_FLAGS) -c $< 


clean:
	rm -f $(PROGRAMM) $(OBJECTS)


