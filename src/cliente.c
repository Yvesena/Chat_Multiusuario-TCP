#include "cliente.h"
#include "libtslog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Inicializa o cliente com IP e porta */
int cliente_init(Cliente *cli, const char *ip, int porta) {
    if (!cli) return -1;

    memset(cli, 0, sizeof(Cliente));
    strncpy(cli->ip, ip, sizeof(cli->ip)-1);
    cli->porta = porta;

    cli->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (cli->sock < 0) {
        perror("Erro ao criar socket");
        return -1;
    }

    cli->addr.sin_family = AF_INET;
    cli->addr.sin_port = htons(porta);
    cli->addr.sin_addr.s_addr = inet_addr(ip);

    return 0;
}

/* Conecta ao servidor */
int cliente_conectar(Cliente *cli) {
    if (!cli) return -1;

    if (connect(cli->sock, (struct sockaddr*)&cli->addr, sizeof(cli->addr)) < 0) {
        perror("Erro ao conectar");
        return -1;
    }

    tslog_write("INFO", "Conectado ao servidor %s:%d", cli->ip, cli->porta);
    return 0;
}

/* Envia mensagem ao servidor */
int cliente_enviar(Cliente *cli, const char *msg) {
    if (!cli || !msg) return -1;

    int enviado = send(cli->sock, msg, strlen(msg), 0);
    if (enviado > 0) {
        tslog_write("INFO", "Mensagem enviada: %s", msg);
    } else {
        tslog_write("ERROR", "Falha ao enviar mensagem");
    }
    return enviado;
}

/* Recebe mensagem do servidor */
int cliente_receber(Cliente *cli, char *buffer, int tamanho) {
    if (!cli || !buffer) return -1;

    int n = recv(cli->sock, buffer, tamanho - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        tslog_write("INFO", "Mensagem recebida: %s", buffer);
    } else if (n == 0) {
        tslog_write("WARN", "Conexão encerrada pelo servidor");
    } else {
        tslog_write("ERROR", "Erro ao receber dados");
    }
    return n;
}

/* Fecha a conexão */
void cliente_fechar(Cliente *cli) {
    if (!cli) return;
    close(cli->sock);
    tslog_write("INFO", "Conexão fechada");
}
