#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "http.h"

void send_response(int client_socket, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %lu\r\n"
             "\r\n"
             "%s",
             status, content_type, strlen(body), body);
    send(client_socket, response, strlen(response), 0);
}

void handle_get_request(int client_socket, const char *full_path) {
    FILE *file = fopen(full_path, "r");
    if (file) {
        char file_buffer[BUFFER_SIZE];
        fread(file_buffer, 1, sizeof(file_buffer), file);
        send_response(client_socket, "200 OK", "text/html", file_buffer);
        fclose(file);
    } else {
        send_response(client_socket, "404 Not Found", "text/html", "<h1>404 Not Found</h1>");
    }
}

void handle_post_request(int client_socket) {
    send_response(client_socket, "200 OK", "text/plain", "POST request received");
}

void handle_put_request(int client_socket, const char *full_path, const char *buffer) {
    FILE *file = fopen(full_path, "w");
    if (file) {
        fputs(buffer, file);
        fclose(file);
        send_response(client_socket, "200 OK", "text/plain", "File created/updated");
    } else {
        send_response(client_socket, "500 Internal Server Error", "text/plain", "Error writing file");
    }
}

void handle_delete_request(int client_socket, const char *full_path) {
    if (remove(full_path) == 0) {
        send_response(client_socket, "200 OK", "text/plain", "File deleted");
    } else {
        send_response(client_socket, "404 Not Found", "text/plain", "File not found");
    }
}