# ♟️ MaZe – Chess Engine


![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)


**MaZe** é uma engine de xadrez desenvolvida em C como parte do Trabalho de Conclusão de Curso (TCC), com foco na construção de uma base modular e eficiente para representação de estados e geração de jogadas usando bitboards.

O nome surgiu da junção dos nomes das duas pessoas mais importantes da minha vida: minha mãe Marta e meu pai José, que sempre me apoiaram e me deram a base para ser quem eu sou hoje.


---

## 📚 Sobre o Projeto

Este projeto tem como objetivo estudar e implementar os princípios fundamentais de uma engine de xadrez, com destaque para:

- Representação de peças e tabuleiro com **bitboards**.
- Geração de ataques previamente computados.
- Uso de técnicas de otimização e possibilidade de análise de algoritmos como **Minimax**, **Alpha-Beta Pruning**.
- Metaheurísticas.

---

## 🧠 Funcionalidades Atuais

- **Representação do tabuleiro via bitboard**: Sistema eficiente de representação usando 64-bit integers
- **Geração dos ataques de todas as peças**: Ataques pré-computados para otimização
- **Geração de jogadas legais**: Inclui direitos de Roque, En Passant e verificação de xeque
- **Parser de strings em notação FEN**: Conversão de posições FEN para representação bitboard
- **Sistema de backup**: Restauração de estado do tabuleiro para desfazer lances ilegais
- **Protocolo UCI básico**: Parsing de comandos UCI incluindo position e moves
- **Documentação completa**: Código totalmente documentado com comentários em português

## 📁 Estrutura do Projeto

```
MaZe/
├── src/
│   ├── maze.c          # Função principal e testes
│   ├── bitboard.c      # Operações com bitboards
│   ├── ataques.c       # Geração de ataques das peças
│   ├── globals.c       # Variáveis globais e estado do jogo
│   └── uci.c           # Implementação do protocolo UCI
├── include/
│   ├── bitboard.h      # Definições de bitboard
│   ├── ataques.h       # Declarações de funções de ataque
│   ├── globals.h       # Variáveis globais e estruturas
│   └── uci.h           # Funções do protocolo UCI
├── makefile            # Script de compilação
└── README.md           # Este arquivo
```

## 📈 Planejamento Futuro

- Implementação de algoritmos de busca:
  - Minimax
  - Poda Alpha-Beta
- Avaliação de posições e heurísticas simples
- Protocolo UCI completo (go, stop, etc.)
- Interface gráfica ou integração com GUIs populares

## ⚙️ Compilação e Uso

### 🔧 Requisitos
- Compilador C (`gcc`)
- `make` instalado (opcional)

### 🚀 Comandos

```bash
# Compilar a versão otimizada
make

# Ou compilar manualmente
gcc -Ofast -Iinclude -o maze src/*.c

# Executar o engine
./maze

# Testar com comando UCI
echo "uci" | ./maze
```

### 🎮 Exemplo de Uso

```bash
# Executar uma sequência de movimentos
./maze
# No engine, use comandos como:
# position startpos moves e2e4 e7e5 g1f3
```

## 💻 Instalação

### Linux
```bash
git clone https://github.com/GustavoGNZ/MaZe.git
cd maze
make
./maze
```

