#ifndef HTTP_H
#define HTTP_H

#define BUFFER_SIZE 1024

// HTTP response handling
void send_response(int client_socket, const char *status, const char *content_type, const char *body);
void handle_get_request(int client_socket, const char *full_path);
void handle_post_request(int client_socket);
void handle_put_request(int client_socket, const char *full_path, const char *buffer);
void handle_delete_request(int client_socket, const char *full_path);

#endif // HTTP_H
