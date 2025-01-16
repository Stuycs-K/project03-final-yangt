CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses
TARGET = chat chat_log.txt
SRC = main.c

$(TARGET): $(SRC)
	touch chat_log.txt
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) 
