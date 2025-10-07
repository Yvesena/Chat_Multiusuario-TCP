
### ESTRUTURA

'''

Projeto_Chat/
│
├── include/                      # Arquivos de cabeçalho (.h)
│   ├── cliente.h                 # Declarações das funções do cliente
│   ├── libtslog.h                # Interface da biblioteca de logs
│   └── servidor.h                # Declarações do servidor e threads
│
├── libtslog/                     # Biblioteca de logs thread-safe
│   ├── tslog.c                   # Implementação do sistema de logs
│   └── tslog.o                   # Objeto compilado (gerado pelo Make)
│
├── src/                          # Código-fonte principal do sistema
│   ├── cliente.c                 # Funções do cliente (envio e recepção de mensagens)
│   ├── main_cliente.c            # Função principal do cliente
│   ├── main_servidor.c           # Função principal do servidor
│   └── servidor.c                # Implementação do servidor multithread
│
├── testes/                       # Scripts e programas de teste
│   ├── test_chat.sh              # Script automatizado de teste com múltiplos clientes
│   ├── test_logging.c            # Teste do sistema de logs
│   └── test_logging.o            # Objeto compilado do teste
│
├── app.log                       # Log geral de execução (gerado automaticamente)
│
├── Makefile                      # Arquivo de automação de compilação
│
├── Estrutura.md                  # Documento com a descrição da estrutura do projeto
├── LEIA-ME.md                    # Guia do projeto e instruções de execução
└── relatorio.final.pdf           # Relatório final da Etapa 3

'''

