#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTES 2
#define MAX_MSG 1024

/* Estrutura principal do servidor */
typedef struct {
    int sock_fd;              // Socket do servidor
    int porta;                // Porta em que o servidor escuta
    struct sockaddr_in addr;  // Endereço do servidor
} Servidor;

/* Funções principais */
int servidor_init(Servidor *srv, int porta);
int servidor_executar(Servidor *srv);
void servidor_fechar(Servidor *srv);

/* Funções auxiliares internas */
void* servidor_tratar_cliente(void *arg);
void adicionar_cliente(int sock);
void remover_cliente(int sock);
void broadcast(int remetente, const char *msg, int len);

#endif // SERVIDOR_H
