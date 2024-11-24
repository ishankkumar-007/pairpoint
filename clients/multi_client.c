#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#define HOST "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024
#define NUM_CLIENTS 5

char *base64_encode(const char *input) {
    BIO *bio, *b64;
    BUF_MEM *buffer;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);
    BIO_write(bio, input, strlen(input));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &buffer);
    char *encoded = malloc(buffer->length + 1);
    memcpy(encoded, buffer->data, buffer->length);
    encoded[buffer->length] = '\0';
    BIO_free_all(bio);
    return encoded;
}

void send_request(const char *request) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    send(sock, request, strlen(request), 0);

    char response[BUFFER_SIZE];
    int bytes_received = recv(sock, response, BUFFER_SIZE - 1, 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Received: %s\n", response);
    }
    close(sock);
}

void *client_task() {
    char *auth = base64_encode("ishank:ishank");
    char auth_header[128];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Basic %s", auth);

    // GET Request
    char *get_request = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    printf("GET Request:\n");
    send_request(get_request);

    // POST Request
    char *post_request = "POST /post_test.html HTTP/1.1\r\nHost: localhost\r\nContent-Length: 0\r\n\r\n";
    printf("\nPOST Request:\n");
    send_request(post_request);

    // PUT Request
    char *put_message = "Hello, world!!!";
    char put_request[BUFFER_SIZE];
    snprintf(put_request, sizeof(put_request),
             "PUT /put_file.txt HTTP/1.1\r\nHost: localhost\r\n%s\r\nContent-Length: %zu\r\n\r\n%s",
             auth_header, strlen(put_message), put_message);
    printf("\nPUT Request:\n");
    send_request(put_request);

    // DELETE Request
    char delete_request[BUFFER_SIZE];
    snprintf(delete_request, sizeof(delete_request),
             "DELETE /delete_test.txt HTTP/1.1\r\nHost: localhost\r\n%s\r\n\r\n",
             auth_header);
    printf("\nDELETE Request:\n");
    send_request(delete_request);

    free(auth);
    return NULL;
}

int main() {
    pthread_t threads[NUM_CLIENTS];

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_create(&threads[i], NULL, client_task, NULL);
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
