#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "server.h"
#include "queue.h"
#include "config.h"
#include "logger.h"

int main() {
    signal(SIGINT, handle_signal);
    read_config();
    init_logger();

    int server_socket = initialize_server();
    if (server_socket == -1) {
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", server_port);

    Queue *queue = create_queue();
    pthread_t thread_pool[THREAD_POOL_SIZE];
    
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], NULL, thread_pool_worker, queue);
    }

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket != -1) {
            enqueue(queue, client_socket);
        }
    }

    return 0;
}
