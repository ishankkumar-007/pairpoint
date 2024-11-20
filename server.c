#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024
#define DEFAULT_PORT 8080
#define DEFAULT_DOC_ROOT "./static"
#define THREAD_POOL_SIZE 10

int server_port;
char document_root[BUFFER_SIZE];

// Task Queue Implementation
typedef struct QueueNode
{
    int client_socket;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
    pthread_mutex_t mutex;      // Mutex for thread-safe access     
    pthread_cond_t cond;        // Condition variable to signal worker threads
} Queue;                        

Queue *create_queue()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return q;
}

void enqueue(Queue *q, int client_socket)
{
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    node->client_socket = client_socket;
    node->next = NULL;

    pthread_mutex_lock(&q->mutex);
    if (q->rear)
    {
        q->rear->next = node;
    }
    else
    {
        q->front = node;
    }
    q->rear = node;
    pthread_cond_signal(&q->cond);      // Signal a waiting worker thread
    pthread_mutex_unlock(&q->mutex);
}

int dequeue(Queue *q)
{
    pthread_mutex_lock(&q->mutex);
    while (!q->front)
    {
        pthread_cond_wait(&q->cond, &q->mutex);     // Wait for a task
    }

    QueueNode *node = q->front;
    int client_socket = node->client_socket;
    q->front = node->next;
    if (!q->front)
    {
        q->rear = NULL;
    }
    free(node);
    pthread_mutex_unlock(&q->mutex);
    return client_socket;
}

// Logging
pthread_mutex_t log_mutex;
FILE *log_file;

void log_request(const char *client_ip, const char *request)
{
    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "Client IP: %s - Request: %s\n", client_ip, request);
    fflush(log_file);
    pthread_mutex_unlock(&log_mutex);
}

// HTTP Response
void send_response(int client_socket, const char *status, const char *content_type, const char *body)
{
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

// Authentication
int authenticate(const char *header)
{
    const char *correct_auth = "Basic aXNoYW5rOmlzaGFuaw=="; // "ishank:ishank" in Base64
    return strstr(header, correct_auth) != NULL;
}

// Client Handling
void handle_client(int client_socket, struct sockaddr_in client_addr)
{
    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, sizeof(buffer) - 1);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    log_request(client_ip, buffer);

    char method[BUFFER_SIZE], path[BUFFER_SIZE];
    sscanf(buffer, "%s %s", method, path);

    char full_path[2 * BUFFER_SIZE];
    snprintf(full_path, sizeof(full_path), "%s%s", document_root, path);
    printf("%s\n", method);
    printf("%s\n", full_path);

    if (strcmp(method, "GET") == 0)
    {
        FILE *file = fopen(full_path, "r");
        if (file)
        {
            char file_buffer[BUFFER_SIZE];
            fread(file_buffer, 1, sizeof(file_buffer), file);
            send_response(client_socket, "200 OK", "text/html", file_buffer);
            fclose(file);
        }
        else
        {
            send_response(client_socket, "404 Not Found", "text/html", "<h1>404 Not Found</h1>");
        }
    }
    else if (strcmp(method, "POST") == 0)
    {
        send_response(client_socket, "200 OK", "text/plain", "POST request received");
    }
    else if (strcmp(method, "PUT") == 0 || strcmp(method, "DELETE") == 0)
    {
        if (!authenticate(buffer))
        {
            send_response(client_socket, "401 Unauthorized", "text/plain", "Authentication required");
            close(client_socket);
            return;
        }

        if (strcmp(method, "PUT") == 0)
        {
            FILE *file = fopen(full_path, "w");
            if (file)
            {
                fputs(buffer, file);
                fclose(file);
                send_response(client_socket, "200 OK", "text/plain", "File created/updated");
            }
            else
            {
                send_response(client_socket, "500 Internal Server Error", "text/plain", "Error writing file");
            }
        }
        else if (strcmp(method, "DELETE") == 0)
        {
            if (remove(full_path) == 0)
            {
                send_response(client_socket, "200 OK", "text/plain", "File deleted");
            }
            else
            {
                send_response(client_socket, "404 Not Found", "text/plain", "File not found");
            }
        }
    }
    else
    {
        send_response(client_socket, "405 Method Not Allowed", "text/plain", "Method not supported");
    }
    close(client_socket);
}

// Thread Pool Worker
void *thread_pool_worker(void *arg)
{
    Queue *queue = (Queue *)arg;

    while (1)
    {
        int client_socket = dequeue(queue);

        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        getpeername(client_socket, (struct sockaddr *)&client_addr, &addr_size);

        handle_client(client_socket, client_addr);
    }

    return NULL;
}

// Signal Handling
void handle_signal(int signal)
{
    printf("\n\nShutting down server...\n");
    pthread_mutex_destroy(&log_mutex);
    fclose(log_file);
    exit(0);
}

// Configuration
void read_config()
{
    FILE *config_file = fopen("server.conf", "r");
    if (!config_file)
    {
        perror("Failed to open config file, using defaults");
        server_port = DEFAULT_PORT;
        strcpy(document_root, DEFAULT_DOC_ROOT);
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), config_file))
    {
        sscanf(line, "port=%d", &server_port);
        sscanf(line, "document_root=%s", document_root);
    }
    fclose(config_file);
}

// Main Function
int main()
{
    signal(SIGINT, handle_signal);
    read_config();

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, THREAD_POOL_SIZE) == -1)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", server_port);

    pthread_mutex_init(&log_mutex, NULL);
    log_file = fopen("server_log.txt", "a");

    Queue *queue = create_queue();
    pthread_t thread_pool[THREAD_POOL_SIZE];
    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_create(&thread_pool[i], NULL, thread_pool_worker, queue);
    }

    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket != -1)
        {
            enqueue(queue, client_socket);
        }
    }

    return 0;
}
