CC = gcc
CFLAGS = -Wall -Wextra -g

# nome executaveis
SERVER_EXEC = servidor
CLIENT_EXEC = cliente

SERVER_SRC = servidor.c sock.o
CLIENT_SRC = cliente.c sock.o

.PHONY: all clean

# regra para compilar tudo
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# compilar o servidor
$(SERVER_EXEC): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# compilar o cliente
$(CLIENT_EXEC): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c sock.h
	$(CC) $(CFLAGS) -c $< -o $@

# limpar arquivos objeto e executáveis
clean:
	rm -f *.o $(SERVER_EXEC) $(CLIENT_EXEC)
