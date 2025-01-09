#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 256
#define MAX_NAME_LENGTH 50

typedef struct {
    int socket;
    char name[MAX_NAME_LENGTH];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;


void *handle_client(void *arg);
void broadcast_message(const char *message, int sender_socket);
void start_server();
void start_client(const char *name);
void init_ui();
void end_ui();
void display_message(const char *message);


#endif 
	