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




### Servidor 
./servidor 8080

### Clinte
./cliente 127.0.0.1 8080



### Teste para o .sh
antes disso :chmod +x testes/test_chat.sh
./testes/test_chat.sh

### Precisa de
sudo apt update
sudo apt install gnome-terminal -y




####  Etapa 1 – Biblioteca libtslog: Logging Concorrente em C

Nesta fase, foi desenvolvida uma biblioteca simples e eficiente para registro de logs em ambiente multithread, chamada libtslog. A implementação foi feita em C, utilizando pthread mutex para garantir que apenas uma thread por vez possa escrever no arquivo de log, evitando assim condições de corrida (race conditions).

A biblioteca possibilita que múltiplas threads gravem mensagens simultaneamente de forma segura e sincronizada.
Para validar a robustez da solução, foi criado um programa de teste (log_test.c), que simula diversas threads concorrentes gerando e escrevendo mensagens no arquivo de log.

#### Etapa 2 – Servidor de Chat TCP com Teste Automatizado

Na segunda etapa, foi implementado um sistema completo de chat em rede, baseado no protocolo TCP, com foco em comunicação em tempo real e suporte a múltiplos usuários conectados simultaneamente.

As principais funcionalidades incluem:

Servidor multithreaded: gerencia diversas conexões de clientes de forma simultânea, atribuindo uma thread dedicada para cada usuário conectado.

Broadcast de mensagens: todas as mensagens enviadas por um cliente são automaticamente retransmitidas para os demais usuários ativos.

Identificação por apelidos: cada usuário pode se identificar com um nome personalizado, facilitando a comunicação.

Registro de eventos com libtslog: todas as ações relevantes (como conexões, desconexões e mensagens trocadas) são registradas no log por meio da biblioteca libtslog, garantindo integridade mesmo em ambiente concorrente.

Comunicação bidirecional: o sistema mantém canais distintos para envio e recebimento de mensagens, permitindo uma troca contínua e fluida entre cliente e servidor. 