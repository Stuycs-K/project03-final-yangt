#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ncurses.h>
#include <time.h>
#include <signal.h>

#define SHM_KEY 1234
#define SEM_KEY 5678
#define MAX_MESSAGE_LENGTH 256
#define CHAT_LOG_FILE "chat_log.txt"

// Semaphore operations
void semaphore_lock(int semid) {
    struct sembuf op = {0, -1, 0};
    semop(semid, &op, 1);
}

void semaphore_unlock(int semid) {
    struct sembuf op = {0, 1, 0};
    semop(semid, &op, 1);
}

// Function to append messages to the chat log
void save_message_to_file(const char *message) {
    FILE *file = fopen(CHAT_LOG_FILE, "a");
    if (file == NULL) {
        perror("Failed to open chat log file");
        return;
    }
    fprintf(file, "%s\n", message);
    fclose(file);
}

// Function to load and display the chat log
void load_chat_log() {
    FILE *file = fopen(CHAT_LOG_FILE, "r");
    if (file == NULL) {
        return; // No chat log yet
    }
    char line[MAX_MESSAGE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        printw("%s", line);
    }
    fclose(file);
}

void init_ncurses() {
    initscr();
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
}

void cleanup_ncurses() {
    endwin();
}

// Get a timestamp for each message
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%H:%M:%S]", t);
}

// Process to handle live updates
void live_update_process(int sem_id) {
    load_chat_log();    
    int shm_id = shmget(SHM_KEY, MAX_MESSAGE_LENGTH, 0666);
    if (shm_id < 0) {
        perror("shmget failed in live_update_process");
        exit(EXIT_FAILURE);
    }

    char *shared_memory = (char *)shmat(shm_id, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat failed in live_update_process");
        exit(EXIT_FAILURE);
    }

    while (1) {
        semaphore_lock(sem_id);
        if (strlen(shared_memory) > 0) {
            // Display the message and clear shared memory
            printw("%s\n", shared_memory);
            save_message_to_file(shared_memory);
            memset(shared_memory, 0, MAX_MESSAGE_LENGTH);
            refresh();
        }
        semaphore_unlock(sem_id);

        usleep(1); // Check every 100ms
    }

    shmdt(shared_memory);
    exit(EXIT_SUCCESS);
}

int main() {
    int shm_id, sem_id;
    char *shared_memory;
    char username[50];

    // Initialize shared memory
    shm_id = shmget(SHM_KEY, MAX_MESSAGE_LENGTH, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    shared_memory = (char *)shmat(shm_id, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphore
    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphore value to 1 (unlocked)
    semctl(sem_id, 0, SETVAL, 1);

    // Initialize ncurses
    init_ncurses();

    // Prompt for username
    mvprintw(0, 0, "Enter your username: ");
    echo();
    getnstr(username, sizeof(username));
    noecho();

    // Load chat history
    load_chat_log();
    refresh();

    // Fork to handle live updates
    pid_t pid = fork();
    if (pid == 0) {
        live_update_process(sem_id);
        load_chat_log();
    }

    char input[MAX_MESSAGE_LENGTH];
    char formatted_message[MAX_MESSAGE_LENGTH + 50]; // For timestamp + username

    while (1) {
        load_chat_log();
        // Get user input
        mvprintw(LINES - 2, 0, "> ");
        clrtoeol();
        echo();
        getnstr(input, MAX_MESSAGE_LENGTH);
        noecho();

        if (strcmp(input, "/quit") == 0) {
            break;
        }

        // Add timestamp and username to the message
        char timestamp[20];
        get_timestamp(timestamp, sizeof(timestamp));
        snprintf(formatted_message, sizeof(formatted_message), "%s %s: %s", timestamp, username, input);

        // Save and display message
        save_message_to_file(formatted_message);
        semaphore_lock(sem_id);
        strncpy(shared_memory, formatted_message, MAX_MESSAGE_LENGTH);
        semaphore_unlock(sem_id);

        // Clear shared memory after broadcasting
        usleep(1); // Give clients time to read
        semaphore_lock(sem_id);
        memset(shared_memory, 0, MAX_MESSAGE_LENGTH);
        semaphore_unlock(sem_id);
    }

    // Cleanup
    kill(pid, SIGKILL); // Terminate the child process
    cleanup_ncurses();
    shmdt(shared_memory);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);

    return 0;
}
