#ifndef CLIENTE_H
#define CLIENTE_H

#include <netinet/in.h>

/* Estrutura do cliente */
typedef struct {
    int sock;                 // descritor do socket
    char ip[16];              // endereço IP (ex: "127.0.0.1")
    int porta;                // porta de conexão
    struct sockaddr_in addr;  // informações do servidor
} Cliente;

/* Funções do cliente */
int cliente_init(Cliente *cli, const char *ip, int porta);
int cliente_conectar(Cliente *cli);
int cliente_enviar(Cliente *cli, const char *msg);
int cliente_receber(Cliente *cli, char *buffer, int tamanho);
void cliente_fechar(Cliente *cli);

#endif // CLIENTE_H
