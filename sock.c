#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <unistd.h>         //Para reproduzir imagens e vídeos
#include <sys/wait.h>       //Para reproduzir imagens e vídeos
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

//Detecta a extensão dos dados --------------------------------------------------------------------
char* devolve_extensao(char *caminho_arquivo)
{
    //Abre o arquivo em modo binário
    FILE *arquivo = fopen(caminho_arquivo, "rb");

    //Se não conseguiu abrir o arquivo
    if (!arquivo)
    {
        //Assume que algo foi lido pelo terminal
        printf("Não foi possível abrir o arquivo!\n");
        return NULL;
    }

    //Buffer para armazenar o cabeçalho que será lido
    unsigned char cabecalho[8];

    //Lê o cabeçalho e retorna a quantidade de itens lidos com sucesso
    size_t lidos = fread(cabecalho, 1, 8, arquivo);

    //Fecha o arquivo
    fclose(arquivo);

    if (lidos < 8)
    {
        printf("Erro ou Arquivo muito pequeno\n");
        return NULL;
    }
    else
    {
        if (cabecalho[0] == 0xFF && cabecalho[1] == 0xD8)
            return(".jpg");
        else if (cabecalho[4] == 0x66 && cabecalho[5] == 0x74 && cabecalho[6] == 0x79 && cabecalho[7] == 0x70)
            return(".mp4");
        else
            return(".txt");
    }

}
//------------------------------------------------------------------------------------------------------------------

//Exibe o arquivo (Tesouro) na tela com um processo filho
//OBS: Usar o comando which nome_do_aplicativo para saber se está instalado no computador
void exibe_arquivo(const char *caminho_arquivo)
{
    //Descobre o formato do arquivo
    const char *extensao = devolve_extensao(caminho_arquivo);

    printf("Extensão = %s\n", extensao);
    if (!extensao)
    {
        printf("Erro ao obter a extensão do arquivo\n");
        return;
    }

    //Cria um processo filho
    pid_t pid = fork();

    //Com base na extensão, preenche os campos dos argumentos
    char *args[3];
    if (strcmp(extensao, ".jpg") == 0)
        args[0] = "/usr/bin/feh";       //local do aplicativo
    else if (strcmp(extensao, ".mp4") == 0)
        args[0] = "/usr/bin/ffplay";    //local do aplicativo
    else
        args[0] = "/usr/bin/gedit";     //local do aplicativo

    args[1] = caminho_arquivo;
    args[2] = NULL;

    //Executa o processo filho
    if (pid == 0)
    {
        execv(args[0], args);
        perror("execv() falhou!\n");
    }
    else if (pid > 0)
    {
        //Processo Pai espera
        //wait(NULL);
        printf("Arquivo visualizado!\n");
    }
    else
        perror("fork() falhou :(\n");
}
//----------------------------------------------------------------------------------------------

//Atribui o tipo de ack ao pacote e devolve um pacote com a informação
struct pacote* ack_format_arq(struct pacote *pack)
{
    //Cria um pacote que vai servir como mensagem ACK
    struct pacote *ack = malloc(sizeof (struct pacote));

    //Remove o possível \n do final do arquivo
    unsigned int tamanho_string = strlen(pack->dados);
    if (pack->dados[tamanho_string - 1]== '\n')
        pack->dados[tamanho_string - 1] = '\0';

    //Pega o formato do aqruivo
    printf("dados = '%s'\n", pack->dados);
    char *extensao = devolve_extensao(pack->dados);

    printf("extensão = %s\n", extensao);

    //Com base no formato do arquivo, atribui o tipo do ACK
    if (strcmp(extensao, ".jpg") == 0)
        ack->tipo = IMG;
    else if (strcmp(extensao, '.mp4') == 0)
        ack->tipo = VIDEO;
    else
        ack->tipo = TEXT;

    //Coloca o nome do arquivo dentro do campo de dados do ack
    for (int i = 0; i < pack->tam; i++)
        ack->dados[i] = pack->dados[i];
    
    return ack;

}

//Verifica o pacote e econtra erros
//OBS: Talvez não precisemos dessa função
struct pacote* verifica_pacote(struct pacote *pack)
{
    struct pacote *mensagem = malloc(sizeof(struct pacote));

    //Verifica checksum do pacote
    unsigned int checksum = verifica_checksum(pack);
    if (checksum == 0)
    {
        //Mensagem não chegou
        mensagem->tipo = NACK;
    }
    else if (pack->marcador != 0x7e)
    {
        //Não achou o início
        mensagem->tipo = ERRO;
        mensagem->dados[0] = 2;         //código do erro
    }

    return mensagem;
}