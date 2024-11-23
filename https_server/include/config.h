#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_PORT 8080
#define DEFAULT_DOC_ROOT "./static"
#define BUFFER_SIZE 1024
#define THREAD_POOL_SIZE 10

// Configuration variables
extern int server_port;
extern char document_root[BUFFER_SIZE];

// Configuration functions
void read_config(void);

#endif // CONFIG_H
