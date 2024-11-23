#include <stdio.h>
#include <string.h>
#include "config.h"

int server_port;
char document_root[BUFFER_SIZE];

void read_config() {
    FILE *config_file = fopen("server.conf", "r");
    if (!config_file) {
        perror("Failed to open config file, using defaults");
        server_port = DEFAULT_PORT;
        strcpy(document_root, DEFAULT_DOC_ROOT);
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), config_file)) {
        sscanf(line, "port=%d", &server_port);
        sscanf(line, "document_root=%s", document_root);
    }
    fclose(config_file);
}
