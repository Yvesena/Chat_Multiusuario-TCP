CC = gcc
CFLAGS = -Wall -Iinclude -pthread

# Fontes
LIBTSLOG = libtslog/tslog.c
SERVIDOR_SRC = src/servidor.c src/main_servidor.c
CLIENTE_SRC = src/cliente.c src/main_cliente.c
TESTE_SRC   = testes/test_logging.c

# Executáveis
all: servidor cliente test_logging

servidor: $(SERVIDOR_SRC) $(LIBTSLOG)
	$(CC) $(CFLAGS) -o servidor $(SERVIDOR_SRC) $(LIBTSLOG)

cliente: $(CLIENTE_SRC) $(LIBTSLOG)
	$(CC) $(CFLAGS) -o cliente $(CLIENTE_SRC) $(LIBTSLOG)

test_logging: $(TESTE_SRC) $(LIBTSLOG)
	$(CC) $(CFLAGS) -o test_logging $(TESTE_SRC) $(LIBTSLOG)

clean:
	rm -f servidor cliente test_logging *.o app.log
