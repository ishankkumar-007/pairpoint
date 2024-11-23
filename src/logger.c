#include <stdio.h>
#include "logger.h"

pthread_mutex_t log_mutex;
FILE *log_file;

void init_logger(void) {
    pthread_mutex_init(&log_mutex, NULL);
    log_file = fopen("server_log.txt", "a");
}

void log_request(const char *client_ip, const char *request) {
    pthread_mutex_lock(&log_mutex);
    fprintf(log_file, "Client IP: %s - Request: %s\n", client_ip, request);
    fflush(log_file);
    pthread_mutex_unlock(&log_mutex);
}

void cleanup_logger(void) {
    pthread_mutex_destroy(&log_mutex);
    fclose(log_file);
}
