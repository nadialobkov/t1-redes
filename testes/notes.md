# ANOTAÇÕES REDES 1

## Socket

Em redes de computadores, um **socket** é um ponto de comunicação que permite a troca de dados entre dois dispositivos conectados em uma rede.

Um **raw socket** é um modo avançado de usar sockets. Ele dá acesso direto aos bytes que entram e saem da placa de rede.

## Entendendo funções

### Cria Raw Socket

```c 
int cria_raw_socket(char* nome_interface_rede);
```

A interface de rede pode ser ```eth0``` para se comunicar através de cabo ou ```lo``` (loopback) para comunicar via dois terminais.

O retorno da função é um **descritor de arquivo (file descriptor)** do tipo ```int```, que representa o raw socket.

### Send

```c
ssize_t send(int sockfd, const void buf[.len], size_t len, int flags);
```

A função ```send```envia pacotes brutos. Ela recebe como argumentos o file descriptor do socket, um buffer contendo os dados a serem enviados (ex: um frame Ethernet completo), o tamanho do buffer em bytes e flags de controle (geralmente ```0``` para raw sockets).

Retorna o número de bytes enviados com sucesso. Se o retorno for ```-1```, então houve algum erro.

### Receive

```c
ssize_t recv(int sockfd, void buf[.len], size_t len, int flags);
```

A função ```recv``` recebe pacotes brutos da rede. Ela recebe como argumentos o descritor do raw socket, um buffer aonde serão armazenados os dados, o tamanho máximo do buffer e as flags (normalmente ```0```).

Retorna o número de bytes recebidos. Se o retorno for ```0```, o socket foi fechado, se o retorno for ```-1```, houve algum erro.

## Estrutura do projeto

A estrutura base do projeto seriam dois programas básicos:

1. Um **servidor** que fica ouvindo mensagens.
2. Um **cliente** que envia mensagens.

```
projeto/
├── cliente.c
└── servidor.c
```