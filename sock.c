#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "sock.h"
 

// funcao de criacao de raw socket passada no enunciado do trabalho

int cria_raw_socket(char* nome_interface_rede) {
    // Cria arquivo para o socket sem qualquer protocolo
    int soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (soquete == -1) {
        fprintf(stderr, "Erro ao criar socket: Verifique se você é root!\n");
        exit(-1);
    }
 
    int ifindex = if_nametoindex(nome_interface_rede);
 
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ifindex;
    // Inicializa socket
    if (bind(soquete, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        fprintf(stderr, "Erro ao fazer bind no socket\n");
        exit(-1);
    }
 
    struct packet_mreq mr = {0};
    mr.mr_ifindex = ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    // Não joga fora o que identifica como lixo: Modo promíscuo
    if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        fprintf(stderr, "Erro ao fazer setsockopt: "
            "Verifique se a interface de rede foi especificada corretamente.\n");
        exit(-1);
    }
 
    return soquete;
}

// ------------------------------------------------------------------------------------------------

//Calcula checksum --------------------------------------------------------------------------------
//Campos: tamanho + sequência + tipo + dados
//OBS: Soma o campo dos dados somente até a quantidade indicada pelo tamanho
void calcula_checksum(struct pacote *pack)
{
    unsigned int checksum;

    checksum = pack->tam + pack->seq + pack->tipo;

    //Soma os dados escritos
    for (int i = 0; i < pack->tam; i++)
        checksum += pack->dados[i];

    //Garante que o valor vai ter 8 bits 
    //Pega os 8 bits menos significativos
    //checksum AND 11111111 (por isso mantém somente os 8 bits menos significativos ligados)
    checksum = checksum & 0xFF;

    //Escreve o valor no campo especificado
    pack->checksum = checksum;

}
//-------------------------------------------------------------------------------------------------

//Verifica o campo do checksum da mensagem recebida -----------------------------------------------
//*pack é a mensagem recebida
unsigned int verifica_checksum(struct pacote *pack)
{
    unsigned int checksum_original;

    //Armazena o valor inicial do checksum
    checksum_original = pack->checksum;

    //Zera temporariamente (para não interferir no cálculo)
    pack->checksum = 0;

    //Calcula o checksum da mensagem recebida
    calcula_checksum(pack);

    //Se não forem iguais, houve erro
    if (checksum_original != pack->checksum)
        return 0;
    
    printf("Checksum OK!\n");

    return 1;
}
//-------------------------------------------------------------------------------------------------

//Detecta o tipo dos dados ------------------------------------------------------------------------
void detecta_tipo(struct pacote *pack, char *caminho_arquivo)
{
    //Busca o ponto do nome do arquivo para encontrar a extensão
    //Retorna um ponteiro para o último caractere '.' na string
    char *extensao = strrchr(caminho_arquivo, '.');

    if (extensao == '.jpg')
        pack->tipo = 0000;
    else if (extensao == '.mp4')
        pack->tipo = 0001;
    else if (extensao == '.txt')
        pack->tipo = 0010;
}


//-------------------------------------------------------------------------------------------------

// prepara vetor de pacotes com os dados a serem enviados
// recebe caminho do arquivo

struct pacote **prepara_pacotes_dados(const char *caminho) {

    // usa estrutura stat para conseguir info do arquivo
    struct stat info;

    if(stat(caminho, &info) == -1) {
        perror("Erro ao obter informações do arquivo");
        return (NULL);
    }
    // obtem tamanho do arquivo
    ssize_t tamanho = info.st_size;

    // abre o arquivo
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        return (NULL);
    }
    fseek(arquivo, 0, SEEK_SET);    // apontamos para inicio do arquivo

    // descobre quantos pacotes serao necessarios para enviar todo o arquivo
    ssize_t num = (tamanho / TAM_MAX) + 1; // teto

    // cria vetor de pacotes
    struct pacote **packets = malloc(num * sizeof(struct pacote *));
    if (!packets) {
        perror("Erro ao criar vetor de pacotes");
        return (-2);
    }

    printf("tamanho arquivo: %d\n", tamanho);
    printf("num: %d\n", num);


    // cria pacotes com os pedaços de dados do arquivo
    // inicializa campos da estrutura
    for (ssize_t i = 0; i < num; i++) {

        printf("i: %d\n", i);

        packets[i] = malloc(sizeof(struct pacote));
        if (!packets[i]) {
            perror("Erro ao criar pacote de transmissao de dados");
            return (-3);
        }

        packets[i]->marcador = MARC;
        packets[i]->seq = i % 32;
        packets[i]->tipo = DADOS;

        ssize_t bytes_lidos = fread(packets[i]->dados, 1, TAM_MAX, arquivo);
        printf("leu tantos bytes: %d\n", bytes_lidos);
        printf("leu: %s\n", packets[i]->dados);

        packets[i]->tam = bytes_lidos; 

        calcula_checksum(packets[i]);
    }

    printf("saiu laco\n");

    fclose(arquivo);

    return (packets); 
}


//-------------------------------------------------------------------------------------------------

// recebe um vetor de pacotes contendo dados separados sequencialmente
// escreve o arquivo no caminho passado 
// retorna 0 em caso de sucesso e valores negativos em caso de erro

uint8_t interpreta_pacotes_dados(struct pacote **packets, uint8_t tam, const char *caminho) {

    // cria o arquivo para copiar os dados do pacote
    FILE *arquivo = fopen(caminho, "w");
    if (!arquivo) {
        perror("Erro ao criar arquivo");
        return (NULL);
    }
    fseek(arquivo, 0, SEEK_SET);    // apontamos para inicio do arquivo

    if (!packets) {
        perror("Erro: vetor de pacotes inválido");
        return (-1);
    }

    // vamos percorres os pacotes escrever os dados no arquivo
    for (uint8_t i = 0; i < tam; i++) {
        fputs(packets[i]->dados, arquivo);
    }

    fclose(arquivo);

    return (0);
}