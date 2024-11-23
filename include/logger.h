#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <pthread.h>

// Logger variables
extern pthread_mutex_t log_mutex;
extern FILE *log_file;

// Logger functions
void init_logger(void);
void log_request(const char *client_ip, const char *request);
void cleanup_logger(void);

#endif // LOGGER_H
