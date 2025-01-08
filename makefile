
all: compile write

compile:
	@gcc -o main main.c main.h

write:
	@./main

clean:
	@rm -f main