#include "servidor.h"
#include "libtslog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTES 2
#define MAX_MSG 1024

// Lista global de clientes conectados
int clientes[MAX_CLIENTES];
int num_clientes = 0;
pthread_mutex_t mutex_clientes = PTHREAD_MUTEX_INITIALIZER;

/* Inicializa o servidor */
int servidor_init(Servidor *srv, int porta) {
    if (!srv) return -1;

    srv->porta = porta;
    srv->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (srv->sock_fd < 0) {
        perror("Erro ao criar socket");
        return -1;
    }

    int opt = 1;
    setsockopt(srv->sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    srv->addr.sin_family = AF_INET;
    srv->addr.sin_addr.s_addr = INADDR_ANY;
    srv->addr.sin_port = htons(porta);

    if (bind(srv->sock_fd, (struct sockaddr*)&srv->addr, sizeof(srv->addr)) < 0) {
        perror("Erro no bind");
        close(srv->sock_fd);
        return -1;
    }

    if (listen(srv->sock_fd, MAX_CLIENTES) < 0) {
        perror("Erro no listen");
        close(srv->sock_fd);
        return -1;
    }

    tslog_write("INFO", "Servidor iniciado na porta %d", porta);
    printf("Servidor ouvindo na porta %d...\n", porta);
    return 0;
}

/* Adiciona cliente à lista */
void adicionar_cliente(int sock) {
    pthread_mutex_lock(&mutex_clientes);
    if (num_clientes < MAX_CLIENTES) {
        clientes[num_clientes++] = sock;
        tslog_write("INFO", "Cliente %d adicionado à lista", sock);
    } else {
        tslog_write("WARN", "Número máximo de clientes atingido");
        close(sock);
    }
    pthread_mutex_unlock(&mutex_clientes);
}

/* Remove cliente da lista */
void remover_cliente(int sock) {
    pthread_mutex_lock(&mutex_clientes);
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i] == sock) {
            clientes[i] = clientes[num_clientes - 1]; // substitui pelo último
            num_clientes--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_clientes);
    tslog_write("INFO", "Cliente %d removido da lista", sock);
}

/* Envia mensagem a todos os clientes (menos o remetente) */
void broadcast(int remetente, const char *msg, int len) {
    pthread_mutex_lock(&mutex_clientes);
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i] != remetente) {
            send(clientes[i], msg, len, 0);
        }
    }
    pthread_mutex_unlock(&mutex_clientes);
}

/* Função de thread para tratar cliente */
void* servidor_tratar_cliente(void *arg) {
    int sock = *(int*)arg;
    free(arg);

    char buffer[MAX_MSG];
    tslog_write("INFO", "Novo cliente conectado (socket %d)", sock);

    adicionar_cliente(sock);

    while (1) {
        int n = recv(sock, buffer, sizeof(buffer)-1, 0);
        if (n <= 0) break;

        buffer[n] = '\0';
        tslog_write("INFO", "Recebido do cliente %d: %s", sock, buffer);

        // Broadcast para todos os outros clientes
        broadcast(sock, buffer, n);
    }

    close(sock);
    remover_cliente(sock);
    tslog_write("INFO", "Cliente desconectado (socket %d)", sock);
    return NULL;
}

/* Loop principal: aceita conexões e cria threads */
int servidor_executar(Servidor *srv) {
    if (!srv) return -1;

    while (1) {
        int *novo_sock = malloc(sizeof(int));
        if (!novo_sock) {
            tslog_write("ERROR", "Falha ao alocar memória para novo cliente");
            continue;
        }

        *novo_sock = accept(srv->sock_fd, NULL, NULL);
        if (*novo_sock < 0) {
            perror("Erro no accept");
            free(novo_sock);
            continue;
        }

        // 🔒 Verifica limite ANTES de criar thread
        pthread_mutex_lock(&mutex_clientes);
        if (num_clientes >= MAX_CLIENTES) {
            pthread_mutex_unlock(&mutex_clientes);

            tslog_write("WARN", "Conexão rejeitada: limite de %d clientes atingido", MAX_CLIENTES);
            close(*novo_sock);
            free(novo_sock);
            continue; // volta para esperar nova conexão
        }
        pthread_mutex_unlock(&mutex_clientes);

        // Se ainda há vagas, cria uma thread para o cliente
        pthread_t th;
        if (pthread_create(&th, NULL, servidor_tratar_cliente, novo_sock) == 0) {
            pthread_detach(th);
        } else {
            perror("Erro ao criar thread");
            close(*novo_sock);
            free(novo_sock);
        }
    }

    return 0;
}

/* Fecha o servidor */
void servidor_fechar(Servidor *srv) {
    if (!srv) return;
    close(srv->sock_fd);
    tslog_write("INFO", "Servidor encerrado");
}
