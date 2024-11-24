# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lssl -lcrypto -lpthread

# Targets
TARGETS = client multi_client

all: $(TARGETS)

client: client.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

multi_client: multi_client.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f $(TARGETS)

.PHONY: all clean
