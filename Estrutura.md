## 🏗️ Arquitetura

chat-multiuser/
│
├── src/
│ ├── main_server.c # Ponto de entrada do servidor
│ ├── client.c # Cliente CLI oficial
│ ├── server.c # Inicialização do socket e conexões
│ ├── client_handler.c # Lógica da thread que atende cada cliente
│ ├── monitor.c # Lista de clientes protegida (monitor)
│ └── logging.c # Wrapper para libtslog
│
├── include/
│ ├── server.h
│ ├── client_handler.h
│ ├── monitor.h
│ ├── logging.h
│ ├── common.h
│ └── config.h
│
├── logs/
│ └── chat.log # Gerado automaticamente pelo libtslog
│
├── Makefile
└── README.md

