CC = gcc
CFLAGS = -Wall -pthread -Iinclude

OBJ_LIB = libtslog/tslog.o
OBJ_TEST = testes/test_logging.o

all: test_logging servidor cliente

# Biblioteca de logging
$(OBJ_LIB): libtslog/tslog.c include/libtslog.h
	$(CC) $(CFLAGS) -c libtslog/tslog.c -o $(OBJ_LIB)

# Teste de logging
$(OBJ_TEST): testes/test_logging.c include/libtslog.h
	$(CC) $(CFLAGS) -c testes/test_logging.c -o $(OBJ_TEST)

test_logging: $(OBJ_LIB) $(OBJ_TEST)
	$(CC) $(CFLAGS) $(OBJ_LIB) $(OBJ_TEST) -o test_logging

# Servidor concorrente (futuro)
servidor: src/servidor.c $(OBJ_LIB)
	$(CC) $(CFLAGS) src/servidor.c $(OBJ_LIB) -o servidor

# Cliente simples (futuro)
cliente: src/cliente.c
	$(CC) $(CFLAGS) src/cliente.c -o cliente

clean:
	rm -f libtslog/*.o testes/*.o test_logging servidor cliente app.log
