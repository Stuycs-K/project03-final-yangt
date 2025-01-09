CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lpthread -lncurses
TARGET = chat
SRC = main.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
