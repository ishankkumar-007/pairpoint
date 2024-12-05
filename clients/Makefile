# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lssl -lcrypto -lpthread

# Targets and directories
TARGETS = client multi_client
BIN_DIR = bin

# Create bin directory if it doesn't exist
all: $(BIN_DIR) $(addprefix $(BIN_DIR)/, $(TARGETS))

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/client: client.c
	$(CC) $(CFLAGS) -o $@ client.c $(LIBS)

$(BIN_DIR)/multi_client: multi_client.c
	$(CC) $(CFLAGS) -o $@ multi_client.c $(LIBS)

clean:
	rm -rf $(BIN_DIR)

# Run executables
run_client: all
	./$(BIN_DIR)/client

run_multi_client: all
	./$(BIN_DIR)/multi_client

.PHONY: all clean run_client run_multi_client
