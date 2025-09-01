#include "../include/evaluate.h"

int material_score[12] = {
    100, 320, 330, 500, 900, 20000, // P, N, B, R, Q, K
    -100, -320, -330, -500, -900, -20000 // p, n, b, r, q, k
};

// =============================================================================
// TABELAS DE VALORES POSICIONAIS DAS PEÇAS
// =============================================================================
// Baseadas em valores clássicos de engines como Stockfish, Fruit e CPW-Engine
// Valores do ponto de vista das peças brancas (a1=0, h8=63)

// Tabela de valores posicionais para peões
int pawn_table[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

// Tabela de valores posicionais para cavalos
int knight_table[64] = {
   -50,-40,-30,-30,-30,-30,-40,-50,
   -40,-20,  0,  0,  0,  0,-20,-40,
   -30,  0, 10, 15, 15, 10,  0,-30,
   -30,  5, 15, 20, 20, 15,  5,-30,
   -30,  0, 15, 20, 20, 15,  0,-30,
   -30,  5, 10, 15, 15, 10,  5,-30,
   -40,-20,  0,  5,  5,  0,-20,-40,
   -50,-40,-30,-30,-30,-30,-40,-50
};

// Tabela de valores posicionais para bispos
int bishop_table[64] = {
   -20,-10,-10,-10,-10,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5, 10, 10,  5,  0,-10,
   -10,  5,  5, 10, 10,  5,  5,-10,
   -10,  0, 10, 10, 10, 10,  0,-10,
   -10, 10, 10, 10, 10, 10, 10,-10,
   -10,  5,  0,  0,  0,  0,  5,-10,
   -20,-10,-10,-10,-10,-10,-10,-20
};

// Tabela de valores posicionais para torres
int rook_table[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

// Tabela de valores posicionais para dama
int queen_table[64] = {
   -20,-10,-10, -5, -5,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
   -10,  5,  5,  5,  5,  5,  0,-10,
   -10,  0,  5,  0,  0,  0,  0,-10,
   -20,-10,-10, -5, -5,-10,-10,-20
};

// Tabela de valores posicionais para rei (meio-jogo)
int king_table_mg[64] = {
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -20,-30,-30,-40,-40,-30,-30,-20,
   -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

// Tabela de valores posicionais para rei (final de jogo)
int king_table_eg[64] = {
   -50,-40,-30,-20,-20,-30,-40,-50,
   -30,-20,-10,  0,  0,-10,-20,-30,
   -30,-10, 20, 30, 30, 20,-10,-30,
   -30,-10, 30, 40, 40, 30,-10,-30,
   -30,-10, 30, 40, 40, 30,-10,-30,
   -30,-10, 20, 30, 30, 20,-10,-30,
   -30,-30,  0,  0,  0,  0,-30,-30,
   -50,-30,-30,-30,-30,-30,-30,-50
};

/**
 * @brief Avalia apenas o material da posição
 * 
 * Calcula a diferença de material entre os dois lados
 * baseada nos valores das peças.
 * 
 * @return Valor do material (positivo favorece brancas)
 */
int evaluate_material() {
    int score = 0;

    for (int i = P; i <= k; i++) {
        int piece_count = __builtin_popcountll(bitboards[i]);
        score += piece_count * material_score[i];
    }

    return score;
}

/**
 * @brief Função principal de avaliação da posição
 * 
 * Combina diferentes heurísticas para avaliar a posição atual.
 * Centraliza todas as avaliações específicas (material, mobilidade, etc.)
 * 
 * @return Valor total da avaliação
 */
int evaluate() {
    int total_score = 0;

    // Avaliação de material
    total_score += evaluate_material();
    
    // Avaliação posicional das peças
    total_score += evaluate_piece_position();

    // Futuras heurísticas serão adicionadas aqui:
    // total_score += evaluate_mobility();
    // total_score += evaluate_pawn_structure();
    // total_score += evaluate_king_safety();
    // total_score += evaluate_piece_coordination();

    return (lado_a_jogar == branco) ? total_score : -total_score;
}

/**
 * @brief Avalia posicionamento das peças usando tabelas posicionais
 * 
 * Utiliza tabelas de valores posicionais para avaliar quão bem
 * posicionadas estão as peças no tabuleiro.
 * 
 * @return Valor posicional (positivo favorece brancas)
 */
int evaluate_piece_position() {
    int score = 0;
    
    for (int square = 0; square < 64; square++) {
        int flipped_square = square ^ 56; // Inverte para peças pretas
        
        // Verifica cada tipo de peça
        if (getBit(bitboards[P], square)) {
            score += pawn_table[square];
        }
        if (getBit(bitboards[p], square)) {
            score -= pawn_table[flipped_square];
        }
        if (getBit(bitboards[N], square)) {
            score += knight_table[square];
        }
        if (getBit(bitboards[n], square)) {
            score -= knight_table[flipped_square];
        }
        if (getBit(bitboards[B], square)) {
            score += bishop_table[square];
        }
        if (getBit(bitboards[b], square)) {
            score -= bishop_table[flipped_square];
        }
        if (getBit(bitboards[R], square)) {
            score += rook_table[square];
        }
        if (getBit(bitboards[r], square)) {
            score -= rook_table[flipped_square];
        }
        if (getBit(bitboards[Q], square)) {
            score += queen_table[square];
        }
        if (getBit(bitboards[q], square)) {
            score -= queen_table[flipped_square];
        }
        if (getBit(bitboards[K], square)) {
            score += king_table_mg[square]; // Usar meio-jogo por enquanto
        }
        if (getBit(bitboards[k], square)) {
            score -= king_table_mg[flipped_square];
        }
    }

    
    return score;
}