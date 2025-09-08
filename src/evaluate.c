#include "../include/evaluate.h"
#include "../include/globals.h"

int material_score[12] = {
    100, 320, 330, 500, 900, 20000,      // P, N, B, R, Q, K
    -100, -320, -330, -500, -900, -20000 // p, n, b, r, q, k
};

// Definições das variáveis globais para máscaras de avaliação
u64 coluna_masks[64];
u64 linha_masks[64];
u64 peao_isolado_masks[64];
u64 peao_passado_branco_masks[64];
u64 peao_passado_preto_masks[64];

// =============================================================================
// TABELAS DE VALORES POSICIONAIS DAS PEÇAS
// =============================================================================
// Baseadas em valores clássicos de engines como Stockfish, Fruit e CPW-Engine
// Valores do ponto de vista das peças brancas (a1=0, h8=63)

// Tabela de valores posicionais para peões
int pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,         // 1ª fileira (a1-h1)
    5, 10, 10, -20, -20, 10, 10, 5, // 2ª fileira (a2-h2)
    5, -5, -10, 0, 0, -10, -5, 5,   // 3ª fileira (a3-h3)
    0, 0, 0, 20, 20, 0, 0, 0,       // 4ª fileira (a4-h4)
    5, 5, 10, 25, 25, 10, 5, 5,     // 5ª fileira (a5-h5)
    10, 10, 20, 30, 30, 20, 10, 10, // 6ª fileira (a6-h6)
    50, 50, 50, 50, 50, 50, 50, 50, // 7ª fileira (a7-h7)
    0, 0, 0, 0, 0, 0, 0, 0          // 8ª fileira (a8-h8)
};

// Tabela de valores posicionais para cavalos
int knight_table[64] = {
    -50, -40, -30, -30, -30, -30, -40, -50, // 1ª fileira (a1-h1)
    -40, -20, 0, 5, 5, 0, -20, -40,         // 2ª fileira (a2-h2)
    -30, 5, 10, 15, 15, 10, 5, -30,         // 3ª fileira (a3-h3)
    -30, 0, 15, 20, 20, 15, 0, -30,         // 4ª fileira (a4-h4)
    -30, 5, 15, 20, 20, 15, 5, -30,         // 5ª fileira (a5-h5)
    -30, 0, 10, 15, 15, 10, 0, -30,         // 6ª fileira (a6-h6)
    -40, -20, 0, 0, 0, 0, -20, -40,         // 7ª fileira (a7-h7)
    -50, -40, -30, -30, -30, -30, -40, -50  // 8ª fileira (a8-h8)
};

// Tabela de valores posicionais para bispos
int bishop_table[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, // 1ª fileira (a1-h1)
    -10, 5, 0, 0, 0, 0, 5, -10,             // 2ª fileira (a2-h2)
    -10, 10, 10, 10, 10, 10, 10, -10,       // 3ª fileira (a3-h3)
    -10, 0, 10, 10, 10, 10, 0, -10,         // 4ª fileira (a4-h4)
    -10, 5, 5, 10, 10, 5, 5, -10,           // 5ª fileira (a5-h5)
    -10, 0, 5, 10, 10, 5, 0, -10,           // 6ª fileira (a6-h6)
    -10, 0, 0, 0, 0, 0, 0, -10,             // 7ª fileira (a7-h7)
    -20, -10, -10, -10, -10, -10, -10, -20  // 8ª fileira (a8-h8)
};

// Tabela de valores posicionais para torres
int rook_table[64] = {
    0, 0, 0, 5, 5, 0, 0, 0,       // 1ª fileira (a1-h1)
    -5, 0, 0, 0, 0, 0, 0, -5,     // 2ª fileira (a2-h2)
    -5, 0, 0, 0, 0, 0, 0, -5,     // 3ª fileira (a3-h3)
    -5, 0, 0, 0, 0, 0, 0, -5,     // 4ª fileira (a4-h4)
    -5, 0, 0, 0, 0, 0, 0, -5,     // 5ª fileira (a5-h5)
    -5, 0, 0, 0, 0, 0, 0, -5,     // 6ª fileira (a6-h6)
    5, 10, 10, 10, 10, 10, 10, 5, // 7ª fileira (a7-h7)
    0, 0, 0, 0, 0, 0, 0, 0        // 8ª fileira (a8-h8)
};

// // Tabela de valores posicionais para dama
// int queen_table[64] = {
//    -20,-10,-10, -5, -5,-10,-10,-20,  // 1ª fileira (a1-h1)
//    -10,  0,  5,  0,  0,  0,  0,-10,  // 2ª fileira (a2-h2)
//    -10,  5,  5,  5,  5,  5,  0,-10,  // 3ª fileira (a3-h3)
//      0,  0,  5,  5,  5,  5,  0, -5,  // 4ª fileira (a4-h4)
//     -5,  0,  5,  5,  5,  5,  0, -5,  // 5ª fileira (a5-h5)
//    -10,  0,  5,  5,  5,  5,  0,-10,  // 6ª fileira (a6-h6)
//    -10,  0,  0,  0,  0,  0,  0,-10,  // 7ª fileira (a7-h7)
//    -20,-10,-10, -5, -5,-10,-10,-20   // 8ª fileira (a8-h8)
// };

// Tabela de valores posicionais para rei (meio-jogo)
int king_table_mg[64] = {
    20, 30, 10, 0, 0, 10, 30, 20,           // 1ª fileira (a1-h1)
    20, 20, 0, 0, 0, 0, 20, 20,             // 2ª fileira (a2-h2)
    -10, -20, -20, -20, -20, -20, -20, -10, // 3ª fileira (a3-h3)
    -20, -30, -30, -40, -40, -30, -30, -20, // 4ª fileira (a4-h4)
    -30, -40, -40, -50, -50, -40, -40, -30, // 5ª fileira (a5-h5)
    -30, -40, -40, -50, -50, -40, -40, -30, // 6ª fileira (a6-h6)
    -30, -40, -40, -50, -50, -40, -40, -30, // 7ª fileira (a7-h7)
    -30, -40, -40, -50, -50, -40, -40, -30  // 8ª fileira (a8-h8)
};

// Tabela de valores posicionais para rei (final de jogo)
int king_table_eg[64] = {
    -50, -30, -30, -30, -30, -30, -30, -50, // 1ª fileira (a1-h1)
    -30, -30, 0, 0, 0, 0, -30, -30,         // 2ª fileira (a2-h2)
    -30, -10, 20, 30, 30, 20, -10, -30,     // 3ª fileira (a3-h3)
    -30, -10, 30, 40, 40, 30, -10, -30,     // 4ª fileira (a4-h4)
    -30, -10, 30, 40, 40, 30, -10, -30,     // 5ª fileira (a5-h5)
    -30, -10, 20, 30, 30, 20, -10, -30,     // 6ª fileira (a6-h6)
    -30, -20, -10, 0, 0, -10, -20, -30,     // 7ª fileira (a7-h7)
    -50, -40, -30, -20, -20, -30, -40, -50  // 8ª fileira (a8-h8)
};

u64 set_file_rank_masks(int file, int rank)
{

    u64 mask = 0ULL;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int casa = i * 8 + j;
            if (file != -1)
            {
                if (file == j)
                {
                    mask |= setBit(mask, casa);
                }
            }
            if (rank != -1)
            {
                if (rank == i)
                {
                    mask |= setBit(mask, casa);
                }
            }
        }
    }
    return mask;
}

/**
 * @brief Gera máscara para peão passado branco
 * @param file Coluna do peão (0-7)
 * @param rank Fileira do peão (0-7)
 * @return Máscara com as casas à frente do peão nas colunas adjacentes
 */
u64 set_passed_pawn_mask_white(int file, int rank)
{
    u64 mask = 0ULL;
    
    // Para peões brancos, verificar fileiras à frente (rank + 1 até 7)
    for (int r = rank + 1; r < 8; r++)
    {
        // Coluna da esquerda (se existir)
        if (file > 0)
        {
            mask |= setBit(mask, r * 8 + (file - 1));
        }
        
        // Coluna do próprio peão
        mask |= setBit(mask, r * 8 + file);
        
        // Coluna da direita (se existir)
        if (file < 7)
        {
            mask |= setBit(mask, r * 8 + (file + 1));
        }
    }
    
    return mask;
}

/**
 * @brief Gera máscara para peão passado preto
 * @param file Coluna do peão (0-7)
 * @param rank Fileira do peão (0-7)
 * @return Máscara com as casas à frente do peão nas colunas adjacentes
 */
u64 set_passed_pawn_mask_black(int file, int rank)
{
    u64 mask = 0ULL;
    
    // Para peões pretos, verificar fileiras à frente (rank - 1 até 0)
    for (int r = rank - 1; r >= 0; r--)
    {
        // Coluna da esquerda (se existir)
        if (file > 0)
        {
            mask |= setBit(mask, r * 8 + (file - 1));
        }
        
        // Coluna do próprio peão
        mask |= setBit(mask, r * 8 + file);
        
        // Coluna da direita (se existir)
        if (file < 7)
        {
            mask |= setBit(mask, r * 8 + (file + 1));
        }
    }
    
    return mask;
}

void init_evaluation_masks()
{
    for (int linha = 0; linha < 8; linha++)
    {
        for (int coluna = 0; coluna < 8; coluna++)
        {
            int casa = linha * 8 + coluna;
            linha_masks[casa] = set_file_rank_masks(-1, linha);
            coluna_masks[casa] = set_file_rank_masks(coluna, -1);
            peao_isolado_masks[casa] = set_file_rank_masks(coluna - 1, -1) | set_file_rank_masks(coluna + 1, -1);
            
            peao_passado_branco_masks[casa] = set_passed_pawn_mask_white(coluna, linha);
            peao_passado_preto_masks[casa] = set_passed_pawn_mask_black(coluna, linha);
        }
    }

}

/**
 * @brief Avalia o material na posição atual
 *
 * Soma os valores de todas as peças no tabuleiro baseado
 * na tabela material_score.
 *
 * @return Valor total do material (positivo para brancas, negativo para pretas)
 */
int evaluate_material()
{
    int score = 0;

    for (int i = P; i <= k; i++)
    {
        u64 bitboard = bitboards[i];
        while (bitboard)
        {
            int casa = getLeastBitIndex(bitboard);
            score += material_score[i];
            clearBit(bitboard, casa);
        }
    }

    return score;
}

/**
 * @brief Avalia a posição das peças no tabuleiro
 *
 * Usa tabelas de valores posicionais para cada tipo de peça
 * para avaliar quão bem posicionadas estão.
 *
 * @return Valor total da avaliação posicional
 */
int evaluate_positional()
{
    int score = 0;

    for (int i = P; i <= k; i++)
    {
        u64 bitboard = bitboards[i];
        while (bitboard)
        {
            int peca = i;
            int casa = getLeastBitIndex(bitboard);

            // Avaliação posicional baseada no tipo da peça
            switch (peca)
            {
            case P:
                score += pawn_table[casa];
                break;
            case N:
                score += knight_table[casa];
                break;
            case B:
                score += bishop_table[casa];
                break;
            case R:
                score += rook_table[casa];
                break;
            case K:
                score += king_table_mg[casa]; // Usando tabela de meio-jogo
                break;
            case p:
                score -= pawn_table[ESPELHAR_CASA(casa)];
                break;
            case n:
                score -= knight_table[ESPELHAR_CASA(casa)];
                break;
            case b:
                score -= bishop_table[ESPELHAR_CASA(casa)];
                break;
            case r:
                score -= rook_table[ESPELHAR_CASA(casa)];
                break;
            case k:
                score -= king_table_mg[ESPELHAR_CASA(casa)]; // Usando tabela de meio-jogo
                break;
            }

            clearBit(bitboard, casa);
        }
    }

    return score;
}

/**
 * @brief Função principal de avaliação da posição
 *
 * Combina diferentes heurísticas para avaliar a posição atual.
 * Centraliza todas as avaliações específicas (material, posicional, etc.)
 *
 * @return Valor total da avaliação do ponto de vista do jogador atual
 */
int evaluate()
{
    int score = 0;

    // Avaliação material
    score += evaluate_material();

    // Avaliação posicional
    score += evaluate_positional();

    // Retorna a avaliação do ponto de vista do lado a jogar
    return (lado_a_jogar == branco) ? score : -score;
}
