
all: compile write

compile:
	@gcc -o semaphore semaphores.c semaphores.h

write:
	@./semaphore

clean:
	@rm -f semaphore