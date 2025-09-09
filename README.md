# ♟️ MaZe – Chess Engine

![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)
![Documentation](https://img.shields.io/badge/docs-Doxygen-green?style=flat-square)
![Status](https://img.shields.io/badge/status-Active-brightgreen?style=flat-square)

**MaZe** é uma engine de xadrez desenvolvida em C como parte do Trabalho de Conclusão de Curso (TCC), implementando algoritmos de busca avançados, avaliação de posições e protocolo UCI completo usando bitboards para máxima eficiência.

O nome surgiu da junção dos nomes das duas pessoas mais importantes da minha vida: minha mãe **Ma**rta e meu pai José (**Ze**), que sempre me apoiaram e me deram a base para ser quem eu sou hoje.

---

## 📚 Sobre o Projeto

Este projeto implementa uma engine de xadrez completa e funcional, com foco em:

- **Bitboards** para representação eficiente do tabuleiro
- **Magic Bitboards** para geração rápida de ataques de peças deslizantes
- **Algoritmo Negamax** com poda alfa-beta para busca de lances
- **Quiescence Search** para estabilização de posições táticas
- **Protocolo UCI** completo para integração com GUIs
- **Sistema de avaliação** baseado em material e posição
- **Detecção de repetição** e controle de tempo

---

## 🧠 Funcionalidades Implementadas

### ✅ Sistema de Representação
- **Bitboards de 64 bits**: Representação ultra-eficiente do tabuleiro
- **Magic Bitboards**: Geração instantânea de ataques para bispos e torres
- **Tabelas pré-computadas**: Ataques de peões, cavalos e reis
- **Codificação de lances**: Sistema compacto para armazenar movimentos

### ✅ Geração de Lances
- **Lances de todas as peças**: Incluindo regras especiais
- **Roque**: Implementação completa dos direitos de roque
- **En Passant**: Captura especial de peões
- **Promoção**: Transformação de peões em outras peças
- **Verificação de legalidade**: Detecção de xeque e lance legal

### ✅ Sistema de Busca
- **Algoritmo Negamax**: Busca minimax otimizada
- **Poda Alfa-Beta**: Redução do espaço de busca
- **Quiescence Search**: Busca de capturas para estabilizar posições
- **Variação Principal (PV)**: Armazenamento da melhor linha
- **Killer Moves**: Heurística de ordenação de lances
- **History Heuristic**: Aprendizado de bons lances
- **MVV-LVA**: Ordenação de capturas (Most Valuable Victim - Least Valuable Attacker)

### ✅ Avaliação de Posições
- **Avaliação de material**: Valores das peças balanceados
- **Fatores posicionais**: Controle do centro, mobilidade
- **Segurança do rei**: Penalidades por exposição
- **Estrutura de peões**: Avaliação de fraquezas e forças

### ✅ Protocolo UCI
- **Comandos básicos**: `uci`, `isready`, `position`, `go`
- **Parser de posições**: FEN e sequências de lances
- **Controle de tempo**: Gestão de tempo por lance e partida
- **Modo de análise**: Busca contínua para análise
- **Comunicação padrão**: Interface compatível com GUIs populares

### ✅ Sistemas Auxiliares
- **Detecção de repetição**: Prevenção de loops infinitos
- **Backup de estado**: Sistema de desfazer lances
- **Livro de aberturas**: Integração básica com teoria
- **Logging de partidas**: Histórico de lances jogados
- **Documentação Doxygen**: Código completamente documentado

## 📁 Estrutura do Projeto

```
MaZe/
├── src/                          # Código fonte (.c)
│   ├── maze.c                    # Função principal e posições de teste
│   ├── bitboard.c                # Sistema de bitboards e operações básicas
│   ├── ataques.c                 # Geração de ataques e magic bitboards
│   ├── search.c                  # Algoritmos de busca e ordenação
│   ├── evaluate.c                # Sistema de avaliação de posições
│   ├── uci.c                     # Protocolo UCI e comunicação
│   └── globals.c                 # Variáveis globais e detecção de repetição
├── include/                      # Headers (.h) - API documentada
│   ├── bitboard.h               # Macros, estruturas e bitboard core
│   ├── ataques.h                # Declarações de geração de ataques
│   ├── search.h                 # Interface dos algoritmos de busca
│   ├── evaluate.h               # Interface do sistema de avaliação
│   ├── uci.h                    # Interface do protocolo UCI
│   └── globals.h                # Variáveis globais e auxiliares
├── partidas/                    # Partidas jogadas e logs
│   └── *.pgn                   # Arquivos PGN de partidas
├── aberturas.txt               # Base de conhecimento de aberturas
├── makefile                    # Script de compilação otimizada
├── maze.exe                    # Executável da engine (Windows)
├── LICENSE                     # Licença do projeto
└── README.md                   # Documentação do projeto
```

### 🏗️ Arquitetura Modular

- **`bitboard.h/.c`**: Core do sistema - macros, estruturas e operações básicas
- **`ataques.h/.c`**: Magic bitboards e geração de ataques otimizada
- **`search.h/.c`**: Negamax, poda alfa-beta, quiescence e ordenação
- **`evaluate.h/.c`**: Avaliação material, posicional e segurança do rei
- **`uci.h/.c`**: Protocolo padrão para comunicação com GUIs
- **`globals.h/.c`**: Estado global, repetição e dados compartilhados
---

## ⚙️ Compilação e Uso

### 🔧 Requisitos
- **Compilador**: GCC 7.0+ ou Clang 6.0+
- **Sistema**: Windows/Linux/macOS
- **RAM**: Mínimo 64MB (recomendado 1GB+)
- **CPU**: Qualquer arquitetura x64

### 🚀 Compilação Rápida

```bash
# Clone o repositório
git clone https://github.com/GustavoGNZ/MaZe.git
cd MaZe

# Compilação otimizada (recomendada)
make

# Ou compilação manual com otimizações máximas
gcc -Ofast -Iinclude -o maze src/maze.c src/bitboard.c src/ataques.c src/globals.c src/uci.c src/evaluate.c src/search.c src/aberturas.c
```

### 🎮 Uso Básico

```bash
# Executar engine standalone
./maze

# Testar comunicação UCI
echo "uci" | ./maze

# Analisar posição específica
echo -e "position startpos moves e2e4 e7e5\ngo depth 8" | ./maze

# Integrar com GUI (exemplo Arena)
# 1. Abra Arena Chess GUI
# 2. Engines → Install Engine
# 3. Selecione o executável 'maze'
# 4. Configure como UCI engine
```

### 🔧 Comandos UCI Principais

```uci
uci                                    # Identificação da engine
isready                               # Verificar se está pronta
position startpos                     # Posição inicial
position fen <FEN_STRING>             # Carregar posição FEN
position startpos moves e2e4 e7e5     # Posição com lances
go depth 8                            # Buscar até profundidade 8
go movetime 5000                      # Buscar por 5 segundos
go infinite                           # Busca contínua (análise)
---
```

### 📜 Licença

Veja o arquivo LICENSE para detalhes.


