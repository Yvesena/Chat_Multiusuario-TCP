CC = gcc
CFLAGS = -Wall -pthread -Iinclude

OBJ_LIB = libtslog/tslog.o
OBJ_TEST = testes/test_logging.o

all: test_logging

# Biblioteca de logging
$(OBJ_LIB): libtslog/tslog.c include/libtslog.h
	$(CC) $(CFLAGS) -c libtslog/tslog.c -o $(OBJ_LIB)

# Teste de logging
$(OBJ_TEST): testes/test_logging.c include/libtslog.h
	$(CC) $(CFLAGS) -c testes/test_logging.c -o $(OBJ_TEST)

test_logging: $(OBJ_LIB) $(OBJ_TEST)
	$(CC) $(CFLAGS) $(OBJ_LIB) $(OBJ_TEST) -o test_logging

clean:
	rm -f libtslog/*.o testes/*.o test_logging app.log