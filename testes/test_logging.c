#include "libtslog.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_THREADS 5 
#define MSGS_PER_THREAD 10

void* worker(void *arg) {
    long id = (long)arg;
    for (int i = 0; i < MSGS_PER_THREAD; i++) {
        tslog_write("INFO", "Thread %ld - mensagem %d", id, i);
        usleep(10000); // 10 ms para simular concorrência
    }
    return NULL;
}

int main(void) {
    if (tslog_init("app.log") != 0) {
        perror("Erro ao abrir app.log");
        return 1;
    }

    pthread_t th[N_THREADS];
    for (long i = 0; i < N_THREADS; i++) {
        pthread_create(&th[i], NULL, worker, (void*)i);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(th[i], NULL);
    }

    tslog_close();
    printf("Teste concluído. Veja app.log\n");
    return 0;
}
