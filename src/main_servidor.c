#include "servidor.h"
#include "libtslog.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int porta = 8080; // porta padrão
    if (argc > 1) {
        porta = atoi(argv[1]); // se passar porta via argumento
    }

    tslog_init("app.log"); // inicializa logging

    Servidor srv;
    if (servidor_init(&srv, porta) < 0) {
        fprintf(stderr, "Erro ao iniciar servidor na porta %d\n", porta);
        return 1;
    }

    // loop principal (aceita conexões e cria threads)
    servidor_executar(&srv);

    servidor_fechar(&srv);
    return 0;
}
