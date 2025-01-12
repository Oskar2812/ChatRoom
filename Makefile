# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra

# Winsock Library
LIBS = -lws2_32

# Source directory
SRC = src

# Build directory
BUILD = build

# Source Files
SERVER_SRC = $(SRC)/ChatRoomServer.c $(SRC)/osk_networking.c
CLIENT_SRC = $(SRC)/ChatRoomClient.c $(SRC)/osk_networking.c

# Executable Names
SERVER_EXEC = $(BUILD)/ChatRoomServer
CLIENT_EXEC = $(BUILD)/ChatRoomClient

# Default Target
all: $(SERVER_EXEC) $(CLIENT_EXEC)

client: $(CLIENT_EXEC)

server: $(SERVER_EXEC)

# Build ChatRoomServer
$(SERVER_EXEC): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Build ChatRoomClient
$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Clean Up
clean:
	rm -f $(SERVER_EXEC).exe $(CLIENT_EXEC).exe

# Phony Targets
.PHONY: all clean