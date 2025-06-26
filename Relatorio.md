# Caça ao Tesouro
### Trabalho 1 - Redes de Computadores  
### Alunas:  
>Giovanna FIoravante Dalledone (GRR: 20232370)  
Nadia Luana Lobkov (GRR: 20232381)

## Estrutura do Trabalho
```
    makefile
    pacote.h
    sock.h
    timer.h
    jogo.h
    servidor.c
    cliente.c
    script.sh
```
O trabalho foi dividido em 6 tipos de arquivos, nos quais:
1. `Makefile`: Além de compilar os arquivos para gerar o executável, o makefile possue utilizades específicas para o projeto como:
    1. Limpar a pasta de arquivos (tesouros) encontrados pelo cliente;
    2. Debug. O comando `make_debug` gera os executáveis em "modo depuração", onde o tabuleiro é omitido para que a transferência de mensagens possa ser exibida e analisada na tela. Ele define a flag DEBUG que altera e/ou comenta linhas estratégicas de todos os arquivos. 

2. `pacote.h`: Contém as macros com os tipos das mensagens, a estrutura do pacote e as funções que criam e manipulam os pacotes.
```C
    //Cabeçalho do pacote
    struct pacote_t {
        uint8_t marcador;       //8 bits para o marcador de início
        uint8_t tam: 7;         //7 bits para o tamanho da mensagem
        uint8_t seq: 5;         //5 bits para o campo de sequência
        uint8_t tipo: 4;        //4 bits para o tipo da mensagem
        uint8_t checksum;       //8 bits para o checksum 

        uint8_t dados[TAM_MAX]  //vetor de bytes para os dados (127 posições)
    } 
```
Considerações importantantes: As mensagens com tipos iguais a 3 e 14 estavam livres para a escolha de suas respectivas funcionalidades. Para o número 3 foi escolhido o tipo SYN que é utilizado na conexão inicial. 
Ele é responsável pela sincroniação, ou seja, subir primeiro o cliente e depois o servidor não faz diferença por conta dele. O tipo número 14 é auxiliar e representa a ocorrência do timeout.  
Além disso, um novo código de erro foi definido: `movimento inválido`. Duas macros foram definidas para o caso do OK: OK `normal` e OK `tesouro`. No primeiro caso, a posição atual do jogador não contém nenhum tesouro e, por consequência, o OK Tesouro significa o oposto.

As funções de enviar e receber pacotes sofreram um versinonamento interessante. No início da implementação do trabalho, ambas as funções alocavam um ponteiro pacote para cada mensagem que deveria ser recebida, ou enviada. Além de muito estranho de manipular e visualizar, era muito fácil e perigoso liberar o ponteiro em um momento errado e acabar perdendo a informação apontada por ele. Como solução do problema, atualmente, ambas as funções mentém apenas um ponteiro pacote que é sobrescrito sempre que necessário. A função de enviar pacotes não tem retorno, enquanto a função que os recebe retorna o tipo do pacote recebido.

Um último detalhe é um tópico do checksum. Na função que calcula o cheksum existe uma manipulação (`checksum = checksum & 0xFF`) que garante que o valor final contenha sempre 8 bits, pois realiza um AND  bit a bit com 8 bits 1.

3. `sock.h`: É a biblioteca responsável por lidar com socket `Raw Sockets` e manipular os dados dos arquivos - com manipulação entende-se obter extensão, exibir e enviar e receber dados. 

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