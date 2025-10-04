#!/bin/bash

echo "=== Teste de Múltiplos Clientes ==="
echo "Iniciando servidor em background..."

# Limpa o app.log antigo antes de rodar
> app.log

# Inicia o servidor em background, redirecionando para app.log
./servidor >> app.log 2>&1 &
SERVER_PID=$!

# Aguarda o servidor inicializar
sleep 3

echo "Servidor iniciado (PID: $SERVER_PID)"
echo "Iniciando múltiplos clientes..."

# Função para executar cliente em background
start_client() {
    local client_id=$1
    ./cliente 127.0.0.1 8080 2>&1 | while IFS= read -r line
    do
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] [Cliente $client_id] $line"
    done >> app.log &
    echo "Cliente $client_id iniciado (PID $!)"
}

# Inicia múltiplos clientes
for i in {1..5}; do
    start_client $i
    sleep 1
done

echo ""
echo "✅ Todos os clientes foram iniciados"
echo "   - Servidor: localhost:8080"
echo "   - Clientes: 2 conectados"
echo ""
echo "Analise o arquivo app.log para detalhes das conexões e mensagens."

# Finalizar servidor
echo "Finalizando servidor..."
kill $SERVER_PID 2>/dev/null

echo ""
echo "=== Teste concluído ==="
echo "Verifique os logs em: app.log"
