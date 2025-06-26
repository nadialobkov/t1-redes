# Caça ao Tesouro
### Trabalho 1 - Redes de Computadores (2025/1)
### Alunas:  
Giovanna Fioravante Dalledone (GRR: 20232370)  
Nadia Luana Lobkov (GRR: 20232381)

## Implementação

### Estrutura dos Pacotes

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

Usamos o tipo ```uint8_t``` (que corresponte a um espaço de 8 bits sem sinal) para poder manipular os dados 
de maneira mais segura e versátil. Para garantir que o pacote tenha exatamente 131 bytes, usamos ```#pragma pack``` 
para evitar o alinhamento de bytes dos dados.

### Protocolo

As mensagens com tipos iguais a 3 e 14 estavam livres para a escolha de suas respectivas funcionalidades. 
Para o número 3 foi escolhido o tipo SYN que é utilizado na conexão inicial. 

Ele é responsável pela sincroniação, ou seja, subir primeiro o cliente e depois o servidor não faz diferença por 
conta dele. O tipo número 14 é auxiliar e representa a ocorrência do timeout.  

Além disso, um novo código de erro foi definido: `MOV_INV` (movimento inválido). Duas macros foram definidas para 
o caso do OK: OK `NORMAL` e OK `TESOURO`. No primeiro caso, a posição atual do jogador não contém nenhum tesouro e, 
por consequência, o OK Tesouro significa o oposto.

```c
// tipos de mensagens
#define SYN     3   // conexao inicial
#define TIMEOUT 14  // tipo auxiliar que indica que houve timeout

// codigos de erros
#define SEM_PERM    0   // sem permissao de acesso
#define SEM_ESP     1   // espaco insuficiente
#define MOVE_INV    2   // movimento invalido no tabuleiro

// codigos para o OK
#define NORMAL      0   // posicao normal (vazia)
#define TESOURO     1   // posicao com tesouro
```

As funções de **enviar e receber** pacotes sofreram um versinonamento interessante. No início da implementação do trabalho, 
ambas as funções alocavam um ponteiro pacote para cada mensagem que deveria ser recebida, ou enviada. Além de muito estranho 
de manipular e visualizar, era muito fácil e perigoso liberar o ponteiro em um momento errado e acabar perdendo a informação 
apontada por ele. Como solução do problema, atualmente, ambas as funções mantém apenas um ponteiro pacote que é sobrescrito 
sempre que necessário. A função de enviar pacotes não tem retorno, enquanto a função que os recebe retorna o tipo do pacote recebido
ou TIMEOUT se houve timeout.

Portanto, foi estabelecida a seguinte notação

- `pack_send`: pacote que contem a mensagem a ser enviada
- `pack_recv`: pacote por onde vai receber a mensagem

O lógica **timeout** foi implementada com o auxilio da biblioteca `signal`. A cada milissegundo é disparada uma função que conta os ticks 
de relógio e ao chegar no TIMEOUT_MAX uma flag global é setada para informar que houve timeout. A função que verifica isso é a de recebimento de 
pacotes. Quando acontece um timeout, a função para de receber pacotes e retorna o tipo TIMEOUT, assim, funções de **espera de pacotes** podem usar
essa sinalização para **reenviar a sua mensagem**.

O **envio de ACKS e NACKS** acontece na verificação dos pacotes. A função `espera_pacote()` fica aguardando o recebimento de uma mensagem e assim que
recebe, caso seu tipo não for TIMEOUT, faz a validação do checksum. Se houve erro, é enviado um NACK e em caso de sucesso é enviado um ACK. Enquanto não
enviar um ACK ele ficará esperando pacotes válidos. 


## Estrutura do Trabalho
```
t1-redes
├── makefile
├── script.sh
├── pacote.h
├── sock.h
├── timer.h
├── jogo.h
├── servidor.c
├── cliente.c
```


O trabalho foi dividido em 8 tipos de arquivos, dos quais:
### 1. `Makefile`:
 Além de compilar os arquivos para gerar o executável, o makefile possue utilizades específicas para o projeto como:
    1. Limpar a pasta de arquivos (tesouros) encontrados pelo cliente;
    2. Debug. O comando `make_debug` gera os executáveis em "modo depuração", onde o tabuleiro é omitido para que a transferência de mensagens possa ser exibida e analisada na tela. Ele define a flag DEBUG que altera e/ou comenta linhas estratégicas de todos os arquivos. 

### 2. `pacote.h`:
 Contém as macros com os tipos das mensagens, a estrutura do pacote e as funções que criam e manipulam os pacotes.
```C
    //Cabeçalho do pacote
    struct pacote_t {
        uint8_t marcador;       //8 bits para o marcador de início
        uint8_t tam: 7;         //7 bits para o tamanho da mensagem
        uint8_t seq: 5;         //5 bits para o campo de sequência
        uint8_t tipo: 4;        //4 bits para o tipo da mensagem
        uint8_t checksum;       //8 bits para o checksum 

        uint8_t dados[TAM_MAX]  //Vetor de bytes para os dados (127 posições)
    } 
```
Considerações importantantes: As mensagens com tipos iguais a 3 e 14 estavam livres para a escolha de suas respectivas funcionalidades. Para o número 3 foi escolhido o tipo SYN que é utilizado na conexão inicial. 
Ele é responsável pela sincroniação, ou seja, subir primeiro o cliente e depois o servidor não faz diferença por conta dele. O tipo número 14 é auxiliar e representa a ocorrência do timeout.  
Além disso, um novo código de erro foi definido: `movimento inválido`. Duas macros foram definidas para o caso do OK: OK `normal` e OK `tesouro`. No primeiro caso, a posição atual do jogador não contém nenhum tesouro e, por consequência, o OK Tesouro significa o oposto.

As funções de enviar e receber pacotes sofreram um versinonamento interessante. No início da implementação do trabalho, ambas as funções alocavam um ponteiro pacote para cada mensagem que deveria ser recebida, ou enviada. Além de muito estranho de manipular e visualizar, era muito fácil e perigoso liberar o ponteiro em um momento errado e acabar perdendo a informação apontada por ele. Como solução do problema, atualmente, ambas as funções mentém apenas um ponteiro pacote que é sobrescrito sempre que necessário. A função de enviar pacotes não tem retorno, enquanto a função que os recebe retorna o tipo do pacote recebido.

Um último detalhe é um tópico do checksum. Na função que calcula o cheksum existe uma manipulação (`checksum = checksum & 0xFF`) que garante que o valor final contenha sempre 8 bits, pois realiza um AND  bit a bit com 8 bits 1.

### 3. `sock.h`:   
É a biblioteca responsável por lidar com socket `Raw Sockets` e manipular os dados dos arquivos - com manipulação entende-se obter extensão, exibir e enviar e receber dados. 

A função que devolve a extensão abre e lê o cabeçalho do arquivo, que é único para cada tipo de extensão. Então, armazena esses valores em um vetor e compara com o cabecalho de imagem e vídeo, se não for nenhum dos dois é atribuído o tipo texto. Retorna o tipo do arquivo (imagem, texto ou vídeo) em caso de sucesso ou erro em caso de falha.

Para o arquivo ser exibido, um processo filho é executado. Nesse processo, com base na extensão obtida, um respectivo aplicativo é chamado para exibir o arquivo recebido. Nesse caso, os aplicativos são:
1. Para imagens: `feh`
2. Para vídeos: `ffplay`
3. Para texto: `gedit` 
>OBS: O gedit emite warnings quando exibe o arquivo na tela.

A função de enviar dados recebe os sockes, os pacotes (de envio e recebimento, ambos previamente alocados) e o nome do arquivo a ser enviado. O arquivo é, então, quebrado em partes (sequencialização) para entrar no campo de dados da mensgem que só é enviada após o recebimento de um ACK.  
O fluxo é: 
> Envia o nome do arquivo recebido -> espera ACK

> Obtém tamanho do arquivo (com base no caminho recebido)

> Escreve e envia o tamanho do arquivo -> espera ACK

> Abre e percorre o arquivo de 127 em 127 bytes (tamanho do vetor de dados) e incializa o contador com 0

> Enquantou houverem bytes a serem lidos no arquivo:  
    atualiza o contador
    escreve os bytes no pacote  
    envia o pacote  
    espera o ACK

> Quando sai do laço, o arquivo foi todo enviado, então envia a mensagem especial do FIM do arquivo.

> Espera o ACK

> Termina

>OBSERVAÇÃO: Quando o testamos o timeout com a remoção do cabo, na primeira remoção ocorre o comportamento esperado de envio de NACK e reenvio das mensagems. O problema ocorre quanto testamos uma segunda remoção do cabo, nesse caso, o comportamento torna-se indefinifo com um erro aparente de envio de informações sem cabo.

Em seguida, a função responsável por receber os dados recebe pacotes, previamente alocados, de recebimento e envio. Ela salva o nome do arquivo recebido no diretório **arq_cliente** e cria um caminho até ele. Enquanto o pacote FIM não for recebido, escreve no caminho criado os dados dos pacotes que ela recebe.

### 4. `timer.h` 
É a biblioteca que administra o **timeout**, para tanto é composta por 3 funções:  
1. `inicia_timer`: a cada milissegundo, dispara uma função que conta os milissegundos, quando chega em 2000 milissegundos, dispara o timeout e reseta.
2. `deu_timeout`: retorna 1 caso o timeou tenha sido disparado ou 0 em caso contrário.
3. `timer`: retorna o valor do tick de relógio atual.

### 5. `jogo.h`:
A funções implementadas em `jogo.h` manipulam o tabuleiro, o jogador e as regras do jogo.  
As estruturas definidas foram:
```C
    struct coordenadas_t {                         //Navegação no tabuleiro
    unsigned int x;
    unsigned int y;
    }

    struct tabuleiro_t {
        unsigned int posicoes[8][8];               //Matriz de posições (são 64 posições disponíveis)
        struct coordenadas_t posicao_tesouro[8];   //Coordenadas x e y dos tesouros
        struct coordenadas_t posicao_jogador;      //Posição atual do jogador
        unsigned int tesouros;                     //Número de tesouros que o jogador encontrou
    }


    struct jogador_t{
    unsigned int pos_x;                            //Noordenada x da posição atual
    unsigned int pos_y;                            //Coordenada y da posição atual
    unsigned int tesouros;                         //Número de tesouros que o jogador encontrou
    unsigned int casas_percorridas;                //Quantidade de casas que o jogador já passou
    unsigned int mapa[8][8];                       //Mapa do jogador
    }
```

As maiores considerações a serem feitas sobre esse arquivo são três:
1. A navegação no tabuleiro ocorre depois de desativar o modo canônico do teclado, ou seja, as teclas pressionadas não são mais ecoadas na tela e são processadas sem o ENTER. Quando a leitura da movimentação termina, o modo canônico é reativado, para não causar prejuízos ao terminal.
2. Foram definidas macros para a movimentação, os valores são os mesmos dos recebidos no protocolo das mensagens e facilitam a movimentação do jogador.
3. A função de movimentar o jogador verifica se o movimento é possível e a função de atualizar o jogador imprime o movimento na tela.

>Observação: Atualmente o programa mantém os tesouros sempre nas mesmas posições, pois facilita os testes, contudo, caso o professor queira, podemos aleatorizar as posições para eles a cada nova rodada. Para tanto, basta substituir `srand(0)` por `srand(time=NULL)` e incluir a biblioteca `<time.h>`

### 6. `servidor.c`:
Após a integração com as funções definidas em `jogo.h`e `jogo.c`, ele é o responsável pelas informações gerenciais do jogo. 

FLuxo:
> Cria o Socket e os pacotes de envio e recebimento.

> Inicia o timer e limpa o terminal.

> Espera a conexão do cliente, depois cria o tabuleiro.

> Enquanto houver tesouros não encontrados:  
1 - Imprime as informações gerenciais (ex: posição do jogador e posições com tesouro, etc)  
2 - Espera o pacote com a movimentação solicitada  
3 - Verifica a possibilidade da movimentação  
4 - Caso seja possível, verifica se tem tesouro  
5 - Movimentação possível e com tesouro -> envia arquivo 
6 - Movimentação possível e sem tesouro -> envia uma resposta com movimentação normal  
7 - Movimentação inválida - envia erro e não deixa mover

> Quando todos os tesouros forem encontrados, desaloca as estrutuas e termina.

### 7. `cliente.c`:
Da mesma forma que o **servidor**, após a integração com as funções do jogo, o **cliente** também cria um socket, a diferença é que ele solicita as movimentações e recebe os tesouros.

Fluxo:

> Cria o Socket para se comunicar com o servidor.

> Inicia o timer. 

> Cria o jogador e envia um pacote para iniciar a conexão com o servidor.

> Enquanto houver tesouros não encontrados:  
1 - Lê o movimento do teclado
2 - Escreve um pacote com a solicitação de movimento
3 - Envia o pacote com a solicitação de movimento e espera a resposta NORMAL ou TESOURO  
4 - Resposta NORMAL -> atualiza o jogador  
5 - Resposta TESOURO -> atualiza o jogador e recebe os dados  
6 - Caso a resposta seja de ERRO, a movientação foi inválida.

> Quando todos os tesouros forem encontrados, uma mensagem de vitória é exibida e as estruturas desalocadas.

### 8. `script.sh`:  
Foi o script utilizado para criar duas máquinas virtuais, uma para o servidor e outra para o cliente.

### Instruções para rodar os programas
O programa tem algumas opções de **make**:
> `make`: Tradicional, compila e gera os executáveis **servidor** e **cliente**.

> `make_degub`: Não imprime o tabuleiro para o cliente, mostra, somente as informações gerenciais do jogo e a transmissão dos pacotes.  

> `make clean`: Remove os executáveis.

> `make clean_cliente`: Limpa a pasta de arquivos do cliente e remove os executáveis.

### Considerações Finais:

1. O envio e recebimento dos pacotes não trata o caso da sequência de bytes 0x88 e 0x81, portanto, os arquivos que contiverem tais bytes apresentarão erro.

2. 