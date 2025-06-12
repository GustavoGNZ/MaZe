# ♟️ MaZe – Chess Engine


![Language](https://img.shields.io/badge/language-C-blue?style=flat-square)


**MaZe** é uma engine de xadrez desenvolvida em C como parte do Trabalho de Conclusão de Curso (TCC), com foco na construção de uma base modular e eficiente para representação de estados e geração de jogadas usando bitboards.O nome surgiu da junção dos nomes das duas pessoas mais importantes da minha vida: minha mãe Marta e meu pai José, que sempre me apoiaram e me deram a base para ser quem eu sou hoje.


---

## 📚 Sobre o Projeto

Este projeto tem como objetivo estudar e implementar os princípios fundamentais de uma engine de xadrez, com destaque para:

- Representação de peças e tabuleiro com **bitboards**.
- Geração de ataques previamente computados.
- Uso de técnicas de otimização e possibilidade de análise de algoritmos como **Minimax**, **Alpha-Beta Pruning**.

---

## ⚙️ Compilação

### 🔧 Requisitos

- Compilador C (`gcc`)
- `make` instalado

### 🚀 Comandos

```bash
# Compilar a versão otimizada
make

# Compilar com suporte a debug
make debug
```


## 🧠 Funcionalidades Atuais

- Representação do tabuleiro via bitboard
- Geração de ataques de peões e cavalos.

## 📈 Planejamento Futuro

- Geração de jogadas legais e movimentação
- Implementação de algoritmos de busca:
  - Minimax
  - Poda Alpha-Beta
- Avaliação de posições e heurísticas simples
- Suporte ao protocolo UCI

## 💻 Instalação

### Linux
```bash
git clone https://github.com/GustavoGNZ/MaZe.git
cd maze
make
./maze
```

### Windows

Em breve.
