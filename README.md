# 🌐 TRABALHO 1 - REDES

## ⚔️ Caça ao Tesouro usando Raw Sockets

Implementação do Trabalho 1 da disciplina de Redes de Computadores 1 (2025/1)

## 📣 Informações Importantes

### ⚙️ Regras Makefile

1. Para compilar tudo

    > make

2. Para compilar apenas o servidor

    > make servidor

3. Para compilar apenas o cliente

    > make cliente

4. Para limpar os arquivos gerados

    > make clean

5. Para limpar os arquivos salvos na pasta do cliente

    > make clean_cliente

### 💻 Execução do Programa

1. Para testar o computador em um mesmo terminal, execute no terminal o ```script.sh``` para criar duas interfaces virtuais de rede.

    > ./script.sh

    Esse comando deve ser executado sempre que o computador é reiniciado.


2. Depois de compilar os programas, abra dois terminais e execute o ```servidor.c``` e o ```cliente.c``` em cada um. Não esqueça de executar como administrador.

    > sudo ./servidor

    > sudo ./cliente


## 💡 Implementação

### 📨 Estrutura dos Pacotes

Os pacotes são implementados em forma de ```struct``` e contem os campos previstos no protocolo proposto.

```c
struct pacote {
    // cabecalho do pacote
    uint8_t marcador;   // 8 bits para marcador de inicio
    uint8_t tam : 7;    // 7 bits para tamanho dos dados
    uint8_t seq : 5;    // 5 bits para campo de sequencia
    uint8_t tipo : 4;   // 4 bits para tipo da mensagem   
    uint8_t checksum;   // 8 bits para checksum

    //dados
    uint8_t dados[127]; // vetor de bytes de dados
};
```

Usamos o tipo ```uint8_t``` (que corresponte a um espaço de 8 bits sem sinal) para poder manipular os dados de maneira mais segura e versátil. Para garantir que o pacote tenha exatamente 131 bytes, usamos ```#pragma pack``` para evitar o alinhamento de bytes dos dados.

## 🔗 Links úteis

- Enunciado: https://docs.google.com/document/d/1uA10zBumyltUo7pb6Rx9Noy6XM1_7Mr-tTTLaIz5ROM/edit?usp=sharing
- Raw Sockets: https://wiki.inf.ufpr.br/todt/doku.php?id=raw_socket
- Timeouts: https://wiki.inf.ufpr.br/todt/doku.php?id=timeouts
