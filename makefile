CC = gcc
CFLAGS = -Wall -Wextra -g

# nome executaveis
SERVER_EXEC = servidor
CLIENT_EXEC = cliente

SERVER_SRC = servidor.c pacote.o sock.o 
CLIENT_SRC = cliente.c pacote.o sock.o

.PHONY: all debug clean clean_cliente

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

debug: CFLAGS += -DDEBUG
debug: $(SERVER_EXEC) $(CLIENT_EXEC)

# limpar arquivos objeto e executáveis
clean:
	rm -f *.o $(SERVER_EXEC) $(CLIENT_EXEC)

clean_cliente:
	rm -rf arq_cliente/*
