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


int main(int argc, char *argv[]) {
}

// Server functionality
void start_server() {


void *handle_client(void *arg) {

}

void broadcast_message(const char *message, int sender_socket) {

}

// Client functionality
void start_client(const char *name) {

}

