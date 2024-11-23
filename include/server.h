#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include "queue.h"

// Server functions
void handle_client(int client_socket, struct sockaddr_in client_addr);
void *thread_pool_worker(void *arg);
void handle_signal(int signal);
int initialize_server(void);

#endif // SERVER_H
