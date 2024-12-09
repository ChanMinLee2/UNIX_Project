# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files
SERVER_SRC = server.c tictactoe.c
CLIENT_SRC = client.c tictactoe.c

# Output binaries
SERVER_BIN = server
CLIENT_BIN = client

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Build server binary
$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRC)

# Build client binary
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC)

# Clean up build files
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
