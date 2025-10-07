#include "cliente.h"
#include "libtslog.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // para close()

#define BUFFER_SIZE 1024

Cliente cli;

// Thread para ficar recebendo mensagens do servidor
void *thread_receber(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int n = cliente_receber(&cli, buffer, sizeof(buffer));
        if (n <= 0) {
            printf("\n⚠️  Conexão com o servidor foi encerrada.\n");
            break; // servidor desconectou ou rejeitou
        }
        printf("\n[Servidor]: %s\n> ", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <IP> <PORTA>\n", argv[0]);
        return 1;
    }

    tslog_init("app.log");

    if (cliente_init(&cli, argv[1], atoi(argv[2])) < 0) exit(1);
    if (cliente_conectar(&cli) < 0) exit(1);
    
    if (argc >= 4) {
    char idmsg[128];
    snprintf(idmsg, sizeof(idmsg), "ID:%s\n", argv[3]); // mensagem de handshake
    cliente_enviar(&cli, idmsg); // envia identificação ao servidor
}
    // Testa se o servidor fechou imediatamente (limite atingido)
    char teste[1];
    int n = recv(cli.sock, teste, sizeof(teste), MSG_DONTWAIT);
    if (n == 0) {
        printf("❌ Servidor cheio. Tente novamente mais tarde.\n");
        tslog_write("WARN", "Conexão rejeitada pelo servidor (lotado)");
        cliente_fechar(&cli);
        return 1;
    }

    // thread para ficar ouvindo mensagens
    pthread_t tid;
    pthread_create(&tid, NULL, thread_receber, NULL);

    // loop para enviar mensagens digitadas
    char msg[BUFFER_SIZE];
    printf("Digite mensagens (CTRL+C para sair):\n> ");
    while (fgets(msg, sizeof(msg), stdin)) {
        msg[strcspn(msg, "\n")] = '\0'; // remove \n
        if (strlen(msg) > 0) {
            if (cliente_enviar(&cli, msg) < 0) {
                printf("⚠️  Não foi possível enviar (servidor pode ter caído).\n");
                break;
            }
        }
        printf("> ");
    }

    cliente_fechar(&cli);
    return 0;
}
