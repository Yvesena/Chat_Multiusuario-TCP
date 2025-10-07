#include "servidor.h"
#include "libtslog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTES 4
#define MAX_MSG 1024
#define NAME_LEN 32 // novo
// Lista global de clientes conectados
typedef struct {
    int sock;
    char name[NAME_LEN];
} ClientInfo;

ClientInfo clientes[MAX_CLIENTES];
int num_clientes = 0;
pthread_mutex_t mutex_clientes = PTHREAD_MUTEX_INITIALIZER;

int contador_clientes = 0;
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
void adicionar_cliente(int sock, const char *nome_base) {
    pthread_mutex_lock(&mutex_clientes);
    if (num_clientes < MAX_CLIENTES) {
        snprintf(clientes[num_clientes].name, NAME_LEN, "Cliente_%d", ++contador_clientes);
        clientes[num_clientes].sock = sock;
        num_clientes++;
        tslog_write("INFO", "Novo cliente adicionado: %s (socket %d)", clientes[num_clientes - 1].name, sock);
    }
    pthread_mutex_unlock(&mutex_clientes);
}

/* Remove cliente da lista */
void remover_cliente(int sock) {//novo
    pthread_mutex_lock(&mutex_clientes);
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].sock == sock) {
            tslog_write("INFO", "removendo cliente %s (socket %d)", clientes[i].name, sock);
            clientes[i] = clientes[num_clientes - 1];
            num_clientes--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_clientes);
}

/* Envia mensagem a todos os clientes (menos o remetente) */
void broadcast(int remetente_sock, const char *msg, int len) {

    pthread_mutex_lock(&mutex_clientes);
    int enviados = 0;
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].sock != remetente_sock) {
            send(clientes[i].sock, msg, len, 0);
            enviados++;
        }
    }
    tslog_write("INFO", "Mensagem enviada para %d cliente(s) (remetente sock %d)", enviados, remetente_sock);
    pthread_mutex_unlock(&mutex_clientes);
}

/* Função de thread para tratar cliente */
void* servidor_tratar_cliente(void *arg) {
    int sock = *(int*)arg;
    free(arg);

    char buffer[MAX_MSG];
    char nome_cliente[NAME_LEN] = "";

    // 🔹 Identificação automática do cliente
    pthread_mutex_lock(&mutex_clientes);
    if (num_clientes < MAX_CLIENTES) {
        snprintf(clientes[num_clientes].name, NAME_LEN, "Cliente_%d", ++contador_clientes);
        clientes[num_clientes].sock = sock;
        strcpy(nome_cliente, clientes[num_clientes].name);
        num_clientes++;
        tslog_write("INFO", "Novo cliente conectado: %s (socket %d)", nome_cliente, sock);
    } else {
        tslog_write("WARN", "Conexão rejeitada: máximo de %d clientes atingido", MAX_CLIENTES);
        pthread_mutex_unlock(&mutex_clientes);
        close(sock);
        return NULL;
    }
    pthread_mutex_unlock(&mutex_clientes);

    // 🔹 Envia mensagem de boas-vindas para o cliente
    char msg_boas_vindas[64];
    snprintf(msg_boas_vindas, sizeof(msg_boas_vindas), "Bem-vindo, %s!\n", nome_cliente);
    send(sock, msg_boas_vindas, strlen(msg_boas_vindas), 0);

    // 🔹 Notifica o servidor
    printf("[Servidor]: %s entrou no chat.\n", nome_cliente);
    tslog_write("INFO", "%s entrou no chat.", nome_cliente);

    // 🔹 Loop principal de recepção de mensagens
    while (1) {
        int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';

        // Ignora mensagens vazias
        if (strlen(buffer) == 0) continue;

        // 🔹 Monta mensagem final com identificação
        char mensagem_final[MAX_MSG + NAME_LEN + 8];
        snprintf(mensagem_final, sizeof(mensagem_final), "[%s]: %s", nome_cliente, buffer);

        tslog_write("INFO", "Recebido de %s (socket %d): %s", nome_cliente, sock, buffer);

        // 🔹 Envia para os outros clientes
        broadcast(sock, mensagem_final, strlen(mensagem_final));
    }

    // 🔹 Cliente saiu
    close(sock);
    pthread_mutex_lock(&mutex_clientes);
    for (int i = 0; i < num_clientes; i++) {
        if (clientes[i].sock == sock) {
            tslog_write("INFO", "Cliente desconectado: %s (socket %d)", clientes[i].name, sock);
            printf("[Servidor]: %s saiu do chat.\n", clientes[i].name);
            clientes[i] = clientes[num_clientes - 1];
            num_clientes--;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_clientes);

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
            printf("[SERVIDOR] Thread criada para cliente %d\n", num_clientes);
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
