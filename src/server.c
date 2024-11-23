#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include "server.h"
#include "http.h"
#include "auth.h"
#include "logger.h"
#include "config.h"

void handle_client(int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, sizeof(buffer) - 1);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    log_request(client_ip, buffer);

    char method[BUFFER_SIZE], path[BUFFER_SIZE];
    sscanf(buffer, "%s %s", method, path);

    char full_path[2 * BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", document_root, path);

    if (strcmp(method, "GET") == 0) {
        handle_get_request(client_socket, full_path);
    }
    else if (strcmp(method, "POST") == 0) {
        handle_post_request(client_socket);
    }
    else if (strcmp(method, "PUT") == 0 || strcmp(method, "DELETE") == 0) {
        if (!authenticate(buffer)) {
            send_response(client_socket, "401 Unauthorized", "text/plain", "Authentication required");
            close(client_socket);
            return;
        }

        if (strcmp(method, "PUT") == 0) {
            handle_put_request(client_socket, full_path, buffer);
        }
        else {
            handle_delete_request(client_socket, full_path);
        }
    }
    else {
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method not supported");
    }
    close(client_socket);
}

void *thread_pool_worker(void *arg) {
    Queue *queue = (Queue *)arg;

    while (1) {
        int client_socket = dequeue(queue);

        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_size);

        handle_client(client_socket, client_addr);
    }

    return NULL;
}

void handle_signal(int signal) {
    printf("\n\nShutting down server...\n");
    cleanup_logger();
    exit(0);
}

int initialize_server() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, THREAD_POOL_SIZE) == -1) {
        perror("Listen failed");
        close(server_socket);
        return -1;
    }

    return server_socket;
}
