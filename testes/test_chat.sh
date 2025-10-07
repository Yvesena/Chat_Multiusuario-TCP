#!/bin/bash
echo "=== Teste de Concorrência: Servidor e Múltiplos Clientes ==="

# Quantos clientes você quer testar
NUM_CLIENTES=5
LOG_FILE="app.log"

# Limpa logs anteriores
> "$LOG_FILE"

# Marca o tempo inicial
START_TIME=$(date +%s.%N)

# Inicia o servidor em background e registra PID
echo "Iniciando servidor em background..."
./servidor >> "$LOG_FILE" 2>&1 &
SERVER_PID=$!
sleep 3
echo "Servidor iniciado (PID: $SERVER_PID)"
echo "Iniciando $NUM_CLIENTES clientes em paralelo..."
echo ""

# Função que simula um cliente automático
start_client() {
    local client_id=$1
    { 
        sleep 1 
        echo "olá, eu sou o cliente $client_id"
        sleep $((RANDOM % 2 + 1))
        echo "tudo bem com o servidor?"
        sleep $((RANDOM % 2 + 1))
        echo "encerrando minha conexão..."
        sleep 1
    } | ./cliente 127.0.0.1 8080 "Cliente $client_id" 2>&1 | while IFS= read -r line
    do
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] [Cliente $client_id] $line"
    done >> "$LOG_FILE" &
    echo "Cliente $client_id iniciado (PID $!)"
}

# Inicia múltiplos clientes com pequenos intervalos
for i in $(seq 1 $NUM_CLIENTES); do
    start_client $i
    sleep 1
done

echo ""
echo "✅ Todos os clientes foram iniciados"
echo " - Servidor: localhost:8080"
echo " - Clientes: $NUM_CLIENTES (alguns podem ser rejeitados se o limite for menor)"
echo ""

# Aguarda interações
echo "⏳ Aguardando interações entre os clientes..."
sleep 10

# Finaliza o servidor
echo ""
echo "Finalizando servidor e clientes..."
kill $SERVER_PID 2>/dev/null

# Aguarda todos os clientes terminarem
wait 2>/dev/null
echo ""

# Marca o tempo final
END_TIME=$(date +%s.%N)
TOTAL_TIME=$(echo "$END_TIME - $START_TIME" | bc)

# === RELATÓRIO FINAL AUTOMÁTICO ===
echo "=== 🧩 Teste concluído ==="
echo ""
echo "📜 Relatório gerado:"
echo " - Arquivo de log: $LOG_FILE"
echo " - Total de clientes tentados: $NUM_CLIENTES"
echo " - Limite configurado no servidor: consulte 'MAX_CLIENTES' em servidor.c"
echo ""

# Gera o relatório final

ERROS_SYNC=$(grep -iE "mutex|erro|deadlock" "$LOG_FILE" | wc -l)

printf "Tempo total: %.2fs\n" "$TOTAL_TIME"

if [ "$ERROS_SYNC" -eq 0 ]; then
    echo "Nenhum erro de sincronização detectado."
else
    echo "⚠️ $ERROS_SYNC possíveis erros de sincronização detectados. Verifique o log."
fi

echo ""
echo "=== ✅ Fim do teste automático ==="
