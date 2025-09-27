# 🗨️ Servidor de Chat Multiusuário (TCP)

Projeto final da disciplina **LPII – Programação Concorrente (C/C++)**.  
Implementa um **servidor de chat em rede** com múltiplos clientes simultâneos, utilizando **threads**, **monitores** e a biblioteca de **logging thread-safe libtslog**.

---

## 📋 Funcionalidades

- Servidor TCP concorrente que aceita múltiplos clientes ao mesmo tempo.
- Cada cliente é atendido por **uma thread dedicada**.
- **Broadcast**: toda mensagem enviada por um cliente é retransmitida a todos os demais conectados.
- **Logging concorrente** com `libtslog`: eventos (entrada, saída e mensagens) são gravados em arquivo de log sem race conditions.
- Proteção de estruturas compartilhadas (lista de clientes) via **monitor** (mutex + condition variables).
- Cliente CLI (linha de comando) para conectar e conversar em tempo real.



# Servidor_de_Chat_Multiusuario — Etapa 1

## Etapa 1: Biblioteca de Logging Concorrente

Esta etapa implementa uma biblioteca de logging thread-safe em C, capaz de registrar mensagens de múltiplas threads de forma segura em um arquivo de log.

### Como compilar

Abra o terminal na pasta `Projeto_Chat` e execute: 
make

### Como executar o teste

Após compilar, execute o teste com:
./test_logging


O programa irá criar várias threads que escrevem mensagens simultaneamente no arquivo `app.log`.

### Verificando o resultado

Após a execução, confira o arquivo `app.log` na pasta do projeto.  
Você verá as mensagens escritas por diferentes threads, demonstrando que a biblioteca de logging funciona corretamente em ambiente concorrente.




Relatório Técnico - Etapa 1
Objetivo da Etapa 1:

Implementar uma biblioteca de logging thread-safe (libtslog), com API clara, capaz de registrar
mensagens de múltiplas threads de forma concorrente e segura, garantindo exclusão mútua e
formatação com timestamp.
Além disso, deve-se demonstrar a arquitetura inicial do projeto, um programa de teste concorrente
e a organização do repositório.
Implementação:
A biblioteca libtslog foi desenvolvida em C, com três funções principais:
- tslog_init(path): inicializa o arquivo de log no caminho especificado.
- tslog_write(level, fmt, ...): escreve mensagens formatadas com nível e timestamp.
- tslog_close(): fecha o arquivo de log.
O acesso concorrente ao arquivo é protegido com pthread_mutex_t, garantindo exclusão mútua e
evitando race conditions. Cada linha é flushada imediatamente, garantindo consistência em
execuções paralelas.
Diagrama de Arquitetura:
As threads da aplicação chamam a API da libtslog, que encapsula a lógica de logging.
Internamente, um mutex garante que apenas uma thread escreva no arquivo por vez, inserindo
timestamp e nível de log antes da mensagem.
Fluxo simplificado:
Thread[i] ® tslog_write() ® Mutex ® Arquivo app.log
Teste CLI:
Foi implementado um programa test_logging que cria múltiplas threads, cada uma registrando
várias mensagens. O resultado no arquivo app.log mostra mensagens intercaladas corretamente,
com timestamps e sem corrupção de linhas.
Exemplo de saída no log:
[2025-09-23 20:10:01] [INFO] Thread 0 - mensagem 0
[2025-09-23 20:10:01] [INFO] Thread 1 - mensagem 0
[2025-09-23 20:10:01] [INFO] Thread 2 - mensagem 0
Conclusão:
A Etapa 1 foi concluída com sucesso. A biblioteca libtslog oferece logging thread-safe, validado por
testes com múltiplas threads. O projeto encontra-se organizado com headers, código-fonte, testes
e diagramas, pronto para evoluir para a Etapa 2 (protótipo cliente/servidor).