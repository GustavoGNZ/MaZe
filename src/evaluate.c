#include "../include/evaluate.h"
#include "../include/globals.h"
#include "../include/bitboard.h"
#include "../include/ataques.h"

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

int coluna_semilivre_bonus = 6;
int coluna_livre_bonus = 12;
int rei_coluna_livre_penalidade = 20;      // Penalidade por rei em coluna livre
int rei_coluna_semilivre_penalidade = 12;  // Penalidade por rei em coluna semi-livre

// Parâmetros de mobilidade (bônus por movimento legal)
int mobility_bonus_knight = 4;   // Bônus por movimento de cavalo
int mobility_bonus_bishop = 5;   // Bônus por movimento de bispo
int mobility_bonus_rook = 2;     // Bônus por movimento de torre
int mobility_bonus_queen = 1;    // Bônus por movimento de dama

// Parâmetros de segurança do rei
int king_safety_pawn_shield_bonus = 10;    // Bônus por peão protetor
int king_safety_open_file_penalty = 15;    // Penalidade por linha/coluna aberta perto do rei
int king_safety_attacked_square_penalty = 20;  // Penalidade por casa atacada ao redor do rei
int king_safety_king_attacked_penalty = 50;    // Penalidade por rei diretamente atacado
// Array para converter índice de casa (0-63) para número da fileira (0-7)
// ====================================================================
// Sistema de coordenadas do tabuleiro:
// - Casa 0 = a1 (fileira 0), Casa 7 = h1 (fileira 0)
// - Casa 8 = a2 (fileira 1), Casa 15 = h2 (fileira 1)
// - Casa 56 = a8 (fileira 7), Casa 63 = h8 (fileira 7)
//
// Para peões BRANCOS: fileira 0 = posição inicial, fileira 7 = quase promovendo
// Para peões PRETOS: fileira 7 = posição inicial, fileira 0 = quase promovendo
//
// Exemplos de uso:
// - Casa 12 (e2) → get_linha[12] = 1 (segunda fileira)
// - Casa 36 (e5) → get_linha[36] = 4 (quinta fileira)
int get_linha[64] = {
    0, 0, 0, 0, 0, 0, 0, 0, // Fileira 1: a1-h1 (fileira 0)
    1, 1, 1, 1, 1, 1, 1, 1, // Fileira 2: a2-h2 (fileira 1)
    2, 2, 2, 2, 2, 2, 2, 2, // Fileira 3: a3-h3 (fileira 2)
    3, 3, 3, 3, 3, 3, 3, 3, // Fileira 4: a4-h4 (fileira 3)
    4, 4, 4, 4, 4, 4, 4, 4, // Fileira 5: a5-h5 (fileira 4)
    5, 5, 5, 5, 5, 5, 5, 5, // Fileira 6: a6-h6 (fileira 5)
    6, 6, 6, 6, 6, 6, 6, 6, // Fileira 7: a7-h7 (fileira 6)
    7, 7, 7, 7, 7, 7, 7, 7  // Fileira 8: a8-h8 (fileira 7)
};

// Parâmetros de avaliação da estrutura de peões
// ================================================

// PENALIDADE POR PEÕES DOBRADOS (valor em centipawns)
// Aplicada quando há 2+ peões da mesma cor na mesma coluna
// Exemplo: 3 peões dobrados = 2 × 5 = -10 pontos de penalidade
// Valor reduzido: peões dobrados são ruins mas não justificam sacrificar material
int peao_dobrado_penalidade = 5;

// PENALIDADE POR PEÕES ISOLADOS (valor em centipawns)
// Aplicada quando um peão não tem peões amigos nas colunas adjacentes
// Peões isolados são vulneráveis mas não tão graves quanto se pensava
// Valor reduzido de 20 para 8
int peao_isolado_penalidade = 8;

// BÔNUS POR PEÕES PASSADOS (valor em centipawns por fileira)
// Valores reduzidos para não supervalorizar peões passados distantes
// peao_passado_bonus[0] = 0   (1ª fileira - posição inicial)
// peao_passado_bonus[1] = 5   (2ª fileira) - antes era 10
// peao_passado_bonus[2] = 10  (3ª fileira) - antes era 20
// peao_passado_bonus[3] = 15  (4ª fileira) - antes era 30
// peao_passado_bonus[4] = 25  (5ª fileira) - antes era 50
// peao_passado_bonus[5] = 40  (6ª fileira) - antes era 70
// peao_passado_bonus[6] = 60  (7ª fileira) - antes era 90
// peao_passado_bonus[7] = 0   (8ª fileira - não há peões aqui)
int peao_passado_bonus[8] = {0, 5, 10, 15, 25, 40, 60, 0};

// =============================================================================
// TABELAS DE VALORES POSICIONAIS DAS PEÇAS
// =============================================================================
// Baseadas em valores clássicos de engines como Stockfish, Fruit e CPW-Engine
// Valores do ponto de vista das peças brancas (a1=0, h8=63)

// Tabela de valores posicionais para peões (valores reduzidos)
int pawn_table[64] = {
    0, 0, 0, 0, 0, 0, 0, 0,         // 1ª fileira (a1-h1)
    5, 5, 5, -10, -10, 5, 5, 5,     // 2ª fileira (a2-h2) - reduzido de -20 para -10
    2, -2, -5, 0, 0, -5, -2, 2,     // 3ª fileira (a3-h3) - reduzido pela metade
    0, 0, 0, 10, 10, 0, 0, 0,       // 4ª fileira (a4-h4) - reduzido de 20 para 10
    2, 2, 5, 12, 12, 5, 2, 2,       // 5ª fileira (a5-h5) - reduzido pela metade
    5, 5, 10, 15, 15, 10, 5, 5,     // 6ª fileira (a6-h6) - reduzido pela metade
    25, 25, 25, 25, 25, 25, 25, 25, // 7ª fileira (a7-h7) - reduzido de 50 para 25
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
                    setBit(mask, casa);
                }
            }
            if (rank != -1)
            {
                if (rank == i)
                {
                    setBit(mask, casa);
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
            setBit(mask, r * 8 + (file - 1));
        }

        // Coluna do próprio peão
        setBit(mask, r * 8 + file);

        // Coluna da direita (se existir)
        if (file < 7)
        {
            setBit(mask, r * 8 + (file + 1));
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
            setBit(mask, r * 8 + (file - 1));
        }

        // Coluna do próprio peão
        setBit(mask, r * 8 + file);

        // Coluna da direita (se existir)
        if (file < 7)
        {
            setBit(mask, r * 8 + (file + 1));
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
 * @brief Avalia a estrutura completa de peões
 *
 * Analisa peões dobrados, isolados e passados em uma única passada pelos bitboards.
 * Isso é mais eficiente que fazer múltiplas avaliações separadas.
 *
 * SISTEMA DE PONTUAÇÃO:
 * - Valores POSITIVOS favorecem as brancas
 * - Valores NEGATIVOS favorecem as pretas
 * - Penalidades são subtraídas do score das brancas (ou somadas para pretas)
 * - Bônus são adicionados ao score das brancas (ou subtraídos para pretas)
 *
 * @return Score da estrutura de peões (positivo favorece brancas)
 */
int evaluate_pawn_structure()
{
    int score = 0;

    // Obter bitboards dos peões de ambas as cores
    u64 peoes_brancos = bitboards[P];
    u64 peoes_pretos = bitboards[p];

    // =========================================================================
    // PARTE 1: ANÁLISE POR COLUNA (Peões dobrados e isolados)
    // =========================================================================

    for (int coluna = 0; coluna < 8; coluna++)
    {
        // Máscara da coluna atual (todas as casas da coluna a-h)
        u64 mascara_coluna = coluna_masks[coluna * 8];

        // Filtrar apenas os peões que estão nesta coluna
        u64 peoes_brancos_coluna = peoes_brancos & mascara_coluna;
        u64 peoes_pretos_coluna = peoes_pretos & mascara_coluna;

        // Contar quantos peões de cada cor há nesta coluna
        // Usando bit manipulation: temp &= temp - 1 remove o bit menos significativo
        int contador_brancos = 0;
        u64 temp_brancos = peoes_brancos_coluna;
        while (temp_brancos)
        {
            contador_brancos++;
            temp_brancos &= temp_brancos - 1; // Remove 1 bit por vez
        }

        int contador_pretos = 0;
        u64 temp_pretos = peoes_pretos_coluna;
        while (temp_pretos)
        {
            contador_pretos++;
            temp_pretos &= temp_pretos - 1; // Remove 1 bit por vez
        }

        // ---------------------------------------------------------------------
        // PEÕES DOBRADOS - Penalidade por ter múltiplos peões na mesma coluna
        // ---------------------------------------------------------------------
        // Lógica: Se há N peões na coluna, aplicar penalidade para (N-1) peões extras
        // Exemplo: 3 peões na coluna = penalidade de 2 × 10 = -20 pontos

        if (contador_brancos > 1)
        {
            // Penalidade para brancas (valor negativo diminui score das brancas)
            score -= peao_dobrado_penalidade * (contador_brancos - 1);
            // Ex: 3 peões brancos dobrados = score -= 10 * (3-1) = score -= 20
        }
        if (contador_pretos > 1)
        {
            // Penalidade para pretas = bônus para brancas (valor positivo)
            score += peao_dobrado_penalidade * (contador_pretos - 1);
            // Ex: 2 peões pretos dobrados = score += 10 * (2-1) = score += 10
        }

        // ---------------------------------------------------------------------
        // PEÕES ISOLADOS - Penalidade por não ter peões amigos nas colunas adjacentes
        // ---------------------------------------------------------------------
        // Lógica: Se há peões nesta coluna MAS não há peões nas colunas vizinhas,
        // então estes peões são "isolados" e vulneráveis

        u64 mascara_isolado = peao_isolado_masks[coluna * 8]; // colunas adjacentes (coluna-1 e coluna+1)

        if (contador_brancos > 0 && !(peoes_brancos & mascara_isolado))
        {
            // Há peões brancos nesta coluna E não há peões brancos nas colunas vizinhas
            // Aplicar penalidade para TODOS os peões isolados desta coluna
            score -= peao_isolado_penalidade * contador_brancos;
            // Ex: 2 peões brancos isolados = score -= 20 * 2 = score -= 40
        }
        if (contador_pretos > 0 && !(peoes_pretos & mascara_isolado))
        {
            // Mesma lógica para pretos (penalidade para pretos = bônus para brancas)
            score += peao_isolado_penalidade * contador_pretos;
            // Ex: 1 peão preto isolado = score += 20 * 1 = score += 20
        }
    }

    // =========================================================================
    // PARTE 2: ANÁLISE INDIVIDUAL (Peões passados)
    // =========================================================================
    // Agora analisamos cada peão individualmente para ver se é "passado"
    // Um peão passado é aquele que não tem peões inimigos bloqueando seu caminho à promoção

    // ---------------------------------------------------------------------
    // PEÕES PASSADOS BRANCOS
    // ---------------------------------------------------------------------
    u64 temp = peoes_brancos;
    while (temp)
    {
        int casa = getLeastBitIndex(temp);
        int linha = get_linha[casa]; // Linha do peão (0=primeira fileira, 7=oitava fileira)

        // Verificar se é peão passado usando a máscara pré-calculada
        // A máscara contém todas as casas que um peão inimigo precisaria ocupar para bloquear este peão
        if (!(peoes_pretos & peao_passado_branco_masks[casa]))
        {
            // Não há peões pretos bloqueando = peão passado!
            // Bônus aumenta conforme o peão se aproxima da promoção (linha 7)
            score += peao_passado_bonus[linha];
            // Ex: Peão branco na 6ª fileira (linha=5) = score += peao_passado_bonus[5] = score += 70
        }

        clearBit(temp, casa); // Remove este peão e continua para o próximo
    }

    // ---------------------------------------------------------------------
    // PEÕES PASSADOS PRETOS
    // ---------------------------------------------------------------------
    temp = peoes_pretos;
    while (temp)
    {
        int casa = getLeastBitIndex(temp);
        int linha = get_linha[casa]; // Linha do peão

        // Verificar se é peão passado
        if (!(peoes_brancos & peao_passado_preto_masks[casa]))
        {
            // Para peões pretos, a "progressão" é invertida: linha 0 é mais próxima da promoção
            // Por isso usamos (7 - linha) para inverter o índice do bônus
            score -= peao_passado_bonus[7 - linha];
            // Ex: Peão preto na 3ª fileira (linha=2) = score -= peao_passado_bonus[7-2] = score -= peao_passado_bonus[5] = score -= 70
        }

        clearBit(temp, casa);
    }

    return score;
}

/**
 * @brief Avalia colunas livres e semi-livres
 *
 * DEFINIÇÕES:
 * - Coluna livre: Sem peões de ambas as cores
 * - Coluna semi-livre (para lado): Sem peões do próprio lado, mas tem peões inimigos
 *
 * BONIFICAÇÕES (baseadas nas variáveis globais):
 * - Torres em colunas livres: +coluna_livre_bonus pontos (12)
 * - Torres em colunas semi-livres: +coluna_semilivre_bonus pontos (6)
 * - Damas em colunas livres: +coluna_livre_bonus/2 pontos (6)
 * - Damas em colunas semi-livres: +coluna_semilivre_bonus/2 pontos (3)
 *
 * @return Score das colunas livres/semi-livres (positivo favorece brancas)
 */
int evaluate_open_files()
{
    int score = 0;

    // Obter bitboards dos peões
    u64 peoes_brancos = bitboards[P];
    u64 peoes_pretos = bitboards[p];
    
    // AVALIAR TORRES BRANCAS
    u64 torres_brancas = bitboards[R];
    while (torres_brancas)
    {
        int casa = getLeastBitIndex(torres_brancas);
        int coluna = casa % 8; // Extrair coluna da casa (0-7)
        u64 mascara_coluna = coluna_masks[casa];
        
        // Verificar se há peões nesta coluna
        int tem_peoes_brancos = (peoes_brancos & mascara_coluna) != 0;
        int tem_peoes_pretos = (peoes_pretos & mascara_coluna) != 0;
        
        if (!tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Coluna livre
            score += coluna_livre_bonus;
            printf("Coluna livre para brancas na coluna %d\n", coluna);
        }
        else if (!tem_peoes_brancos && tem_peoes_pretos)
        {
            // Coluna semi-livre para brancas
            score += coluna_semilivre_bonus;
            printf("Coluna semi-livre para brancas na coluna %d\n", coluna);
        }
        
        clearBit(torres_brancas, casa);
    }
    
    // AVALIAR TORRES PRETAS
    u64 torres_pretas = bitboards[r];
    while (torres_pretas)
    {
        int casa = getLeastBitIndex(torres_pretas);
        int coluna = casa % 8; // Extrair coluna da casa (0-7)
        u64 mascara_coluna = coluna_masks[casa];
        
        // Verificar se há peões nesta coluna
        int tem_peoes_brancos = (peoes_pretos & mascara_coluna) != 0;
        int tem_peoes_pretos = (peoes_pretos & mascara_coluna) != 0;
        
        if (!tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Coluna livre
            score -= coluna_livre_bonus;
            printf("Coluna livre para pretas na coluna %d\n", coluna);
        }
        else if (tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Coluna semi-livre para pretas
            score -= coluna_semilivre_bonus;
            printf("Coluna semi-livre para pretas na coluna %d\n", coluna);
        }
        
        clearBit(torres_pretas, casa);
    }
    
    // AVALIAR DAMAS BRANCAS
    u64 damas_brancas = bitboards[Q];
    while (damas_brancas)
    {
        int casa = getLeastBitIndex(damas_brancas);
        int coluna = casa % 8; // Extrair coluna da casa (0-7)
        u64 mascara_coluna = coluna_masks[casa];
        
        // Verificar se há peões nesta coluna
        int tem_peoes_brancos = (peoes_brancos & mascara_coluna) != 0;
        int tem_peoes_pretos = (peoes_pretos & mascara_coluna) != 0;
        
        if (!tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Coluna livre (50% do bônus das torres)
            score += coluna_livre_bonus / 2;

        }
        else if (!tem_peoes_brancos && tem_peoes_pretos)
        {
            // Coluna semi-livre para brancas (50% do bônus das torres)
            score += coluna_semilivre_bonus / 2;
        }
        
        clearBit(damas_brancas, casa);
    }
    
    // AVALIAR DAMAS PRETAS
    u64 damas_pretas = bitboards[q];
    while (damas_pretas)
    {
        int casa = getLeastBitIndex(damas_pretas);
        int coluna = casa % 8; // Extrair coluna da casa (0-7)
        u64 mascara_coluna = coluna_masks[casa];
        
        // Verificar se há peões nesta coluna
        int tem_peoes_brancos = (peoes_pretos & mascara_coluna) != 0;
        int tem_peoes_pretos = (peoes_pretos & mascara_coluna) != 0;
        
        if (!tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Coluna livre (50% do bônus das torres)
            score -= coluna_livre_bonus / 2;
        }
        else if (tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Coluna semi-livre para pretas (50% do bônus das torres)
            score -= coluna_semilivre_bonus / 2;
        }
        
        clearBit(damas_pretas, casa);
    }
    
    // AVALIAR REIS (PENALIDADES POR ESTAR EM COLUNAS VULNERÁVEIS)
    // ============================================================
    // Reis em colunas livres ou semi-livres são vulneráveis a ataques de torres/damas
    
    // AVALIAR REI BRANCO
    u64 rei_branco = bitboards[K];
    if (rei_branco)
    {
        int casa = getLeastBitIndex(rei_branco);
        int coluna = casa % 8;
        u64 mascara_coluna = coluna_masks[casa];
        
        // Verificar se há peões nesta coluna
        int tem_peoes_brancos = (peoes_brancos & mascara_coluna) != 0;
        int tem_peoes_pretos = (peoes_pretos & mascara_coluna) != 0;
        
        if (!tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Rei em coluna livre = muito perigoso
            score -= rei_coluna_livre_penalidade;
            printf("Rei branco vulnerável em coluna livre %d\n", coluna);
        }
        else if (!tem_peoes_brancos && tem_peoes_pretos)
        {
            // Rei em coluna semi-livre = perigoso
            score -= rei_coluna_semilivre_penalidade;
            printf("Rei branco vulnerável em coluna semi-livre %d\n", coluna);
        }
    }
    
    // AVALIAR REI PRETO
    u64 rei_preto = bitboards[k];
    if (rei_preto)
    {
        int casa = getLeastBitIndex(rei_preto);
        int coluna = casa % 8;
        u64 mascara_coluna = coluna_masks[casa];
        
        // Verificar se há peões nesta coluna
        int tem_peoes_brancos = (peoes_brancos & mascara_coluna) != 0;
        int tem_peoes_pretos = (peoes_pretos & mascara_coluna) != 0;
        
        if (!tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Rei em coluna livre = muito perigoso para pretas = bônus para brancas
            score += rei_coluna_livre_penalidade;
            printf("Rei preto vulnerável em coluna livre %d\n", coluna);
        }
        else if (tem_peoes_brancos && !tem_peoes_pretos)
        {
            // Rei em coluna semi-livre = perigoso para pretas = bônus para brancas
            score += rei_coluna_semilivre_penalidade;
            printf("Rei preto vulnerável em coluna semi-livre %d\n", coluna);
        }
    }
    
    return score;
}

int evaluate_king_safety()
{
    int score = 0;
    
    // Obter bitboards dos peões
    u64 peoes_brancos = bitboards[P];
    u64 peoes_pretos = bitboards[p];

    // =========================================================================
    // SEGURANÇA DO REI BRANCO
    // =========================================================================
    
    u64 rei_branco = bitboards[K];
    if (rei_branco)
    {
        int casa_rei = getLeastBitIndex(rei_branco);
        int linha_rei = casa_rei / 8;
        int coluna_rei = casa_rei % 8;
        
        // AVALIAR ESCUDO DE PEÕES
        // Verificar peões nas casas ao redor do rei (principalmente à frente)
        int peoes_protetores = 0;
        
        // Casas à frente do rei (linha + 1)
        if (linha_rei < 7)
        {
            for (int col_offset = -1; col_offset <= 1; col_offset++)
            {
                int col_check = coluna_rei + col_offset;
                if (col_check >= 0 && col_check < 8)
                {
                    int casa_check = (linha_rei + 1) * 8 + col_check;
                    if (getBit(peoes_brancos, casa_check))
                    {
                        peoes_protetores++;
                    }
                }
            }
        }
        
        score += peoes_protetores * king_safety_pawn_shield_bonus;
        
        // AVALIAR ATAQUES AO REI E CASAS ADJACENTES
        // Verificar se o rei está sendo atacado
        if (casaEstaAtacada(casa_rei, preto))
        {
            score -= king_safety_king_attacked_penalty;
        }
        
        // Verificar casas ao redor do rei (zona de segurança)
        int casas_atacadas = 0;
        for (int linha_offset = -1; linha_offset <= 1; linha_offset++)
        {
            for (int col_offset = -1; col_offset <= 1; col_offset++)
            {
                if (linha_offset == 0 && col_offset == 0) continue; // Pular a casa do próprio rei
                
                int linha_check = linha_rei + linha_offset;
                int col_check = coluna_rei + col_offset;
                
                if (linha_check >= 0 && linha_check < 8 && col_check >= 0 && col_check < 8)
                {
                    int casa_check = linha_check * 8 + col_check;
                    if (casaEstaAtacada(casa_check, preto))
                    {
                        casas_atacadas++;
                    }
                }
            }
        }
        score -= casas_atacadas * king_safety_attacked_square_penalty;
        
        // AVALIAR EXPOSIÇÃO A COLUNAS/LINHAS ABERTAS
        u64 mascara_coluna_rei = coluna_masks[casa_rei];
        
        // Penalidade se o rei está em coluna sem peões próprios
        if (!(peoes_brancos & mascara_coluna_rei))
        {
            score -= king_safety_open_file_penalty;
        }
        
        // Verificar colunas adjacentes também
        if (coluna_rei > 0)
        {
            u64 mascara_coluna_esquerda = coluna_masks[casa_rei - 1];
            if (!(peoes_brancos & mascara_coluna_esquerda))
            {
                score -= king_safety_open_file_penalty / 2;
            }
        }
        
        if (coluna_rei < 7)
        {
            u64 mascara_coluna_direita = coluna_masks[casa_rei + 1];
            if (!(peoes_brancos & mascara_coluna_direita))
            {
                score -= king_safety_open_file_penalty / 2;
            }
        }
    }

    // =========================================================================
    // SEGURANÇA DO REI PRETO
    // =========================================================================
    
    u64 rei_preto = bitboards[k];
    if (rei_preto)
    {
        int casa_rei = getLeastBitIndex(rei_preto);
        int linha_rei = casa_rei / 8;
        int coluna_rei = casa_rei % 8;
        
        // AVALIAR ESCUDO DE PEÕES
        // Para pretas, verificar peões nas casas à frente (linha - 1)
        int peoes_protetores = 0;
        
        if (linha_rei > 0)
        {
            for (int col_offset = -1; col_offset <= 1; col_offset++)
            {
                int col_check = coluna_rei + col_offset;
                if (col_check >= 0 && col_check < 8)
                {
                    int casa_check = (linha_rei - 1) * 8 + col_check;
                    if (getBit(peoes_pretos, casa_check))
                    {
                        peoes_protetores++;
                    }
                }
            }
        }
        
        score -= peoes_protetores * king_safety_pawn_shield_bonus;
        
        // AVALIAR ATAQUES AO REI E CASAS ADJACENTES
        // Verificar se o rei está sendo atacado
        if (casaEstaAtacada(casa_rei, branco))
        {
            score += king_safety_king_attacked_penalty;
        }
        
        // Verificar casas ao redor do rei (zona de segurança)
        int casas_atacadas = 0;
        for (int linha_offset = -1; linha_offset <= 1; linha_offset++)
        {
            for (int col_offset = -1; col_offset <= 1; col_offset++)
            {
                if (linha_offset == 0 && col_offset == 0) continue; // Pular a casa do próprio rei
                
                int linha_check = linha_rei + linha_offset;
                int col_check = coluna_rei + col_offset;
                
                if (linha_check >= 0 && linha_check < 8 && col_check >= 0 && col_check < 8)
                {
                    int casa_check = linha_check * 8 + col_check;
                    if (casaEstaAtacada(casa_check, branco))
                    {
                        casas_atacadas++;
                    }
                }
            }
        }
        score += casas_atacadas * king_safety_attacked_square_penalty;
        
        // AVALIAR EXPOSIÇÃO A COLUNAS/LINHAS ABERTAS
        u64 mascara_coluna_rei = coluna_masks[casa_rei];
        
        // Penalidade se o rei está em coluna sem peões próprios
        if (!(peoes_pretos & mascara_coluna_rei))
        {
            score += king_safety_open_file_penalty;
        }
        
        // Verificar colunas adjacentes também
        if (coluna_rei > 0)
        {
            u64 mascara_coluna_esquerda = coluna_masks[casa_rei - 1];
            if (!(peoes_pretos & mascara_coluna_esquerda))
            {
                score += king_safety_open_file_penalty / 2;
            }
        }
        
        if (coluna_rei < 7)
        {
            u64 mascara_coluna_direita = coluna_masks[casa_rei + 1];
            if (!(peoes_pretos & mascara_coluna_direita))
            {
                score += king_safety_open_file_penalty / 2;
            }
        }
    }

    return score;
}

int evaluate_mobility()
{
    int score = 0;
    u64 ocupacao_total = 0ULL;

    // Calcular bitboard de ocupação total (todas as peças)
    for (int peca = P; peca <= k; peca++)
    {
        ocupacao_total |= bitboards[peca];
    }

    // =========================================================================
    // MOBILIDADE DOS CAVALOS
    // =========================================================================
    
    // Cavalos brancos
    u64 cavalos_brancos = bitboards[N];
    while (cavalos_brancos)
    {
        int casa = getLeastBitIndex(cavalos_brancos);
        u64 ataques = gerarAtaquesCavalo(casa);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_brancas = bitboards[P] | bitboards[N] | bitboards[B] | bitboards[R] | bitboards[Q] | bitboards[K];
        ataques &= ~pecas_brancas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score += movimentos * mobility_bonus_knight;
        
        clearBit(cavalos_brancos, casa);
    }

    // Cavalos pretos
    u64 cavalos_pretos = bitboards[n];
    while (cavalos_pretos)
    {
        int casa = getLeastBitIndex(cavalos_pretos);
        u64 ataques = gerarAtaquesCavalo(casa);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_pretas = bitboards[p] | bitboards[n] | bitboards[b] | bitboards[r] | bitboards[q] | bitboards[k];
        ataques &= ~pecas_pretas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score -= movimentos * mobility_bonus_knight;
        
        clearBit(cavalos_pretos, casa);
    }

    // =========================================================================
    // MOBILIDADE DOS BISPOS
    // =========================================================================
    
    // Bispos brancos
    u64 bispos_brancos = bitboards[B];
    while (bispos_brancos)
    {
        int casa = getLeastBitIndex(bispos_brancos);
        u64 ataques = obterAtaquesBispo(casa, ocupacao_total);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_brancas = bitboards[P] | bitboards[N] | bitboards[B] | bitboards[R] | bitboards[Q] | bitboards[K];
        ataques &= ~pecas_brancas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score += movimentos * mobility_bonus_bishop;
        
        clearBit(bispos_brancos, casa);
    }

    // Bispos pretos
    u64 bispos_pretos = bitboards[b];
    while (bispos_pretos)
    {
        int casa = getLeastBitIndex(bispos_pretos);
        u64 ataques = obterAtaquesBispo(casa, ocupacao_total);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_pretas = bitboards[p] | bitboards[n] | bitboards[b] | bitboards[r] | bitboards[q] | bitboards[k];
        ataques &= ~pecas_pretas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score -= movimentos * mobility_bonus_bishop;
        
        clearBit(bispos_pretos, casa);
    }

    // =========================================================================
    // MOBILIDADE DAS TORRES
    // =========================================================================
    
    // Torres brancas
    u64 torres_brancas = bitboards[R];
    while (torres_brancas)
    {
        int casa = getLeastBitIndex(torres_brancas);
        u64 ataques = obterAtaquesTorre(casa, ocupacao_total);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_brancas = bitboards[P] | bitboards[N] | bitboards[B] | bitboards[R] | bitboards[Q] | bitboards[K];
        ataques &= ~pecas_brancas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score += movimentos * mobility_bonus_rook;
        
        clearBit(torres_brancas, casa);
    }

    // Torres pretas
    u64 torres_pretas = bitboards[r];
    while (torres_pretas)
    {
        int casa = getLeastBitIndex(torres_pretas);
        u64 ataques = obterAtaquesTorre(casa, ocupacao_total);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_pretas = bitboards[p] | bitboards[n] | bitboards[b] | bitboards[r] | bitboards[q] | bitboards[k];
        ataques &= ~pecas_pretas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score -= movimentos * mobility_bonus_rook;
        
        clearBit(torres_pretas, casa);
    }

    // =========================================================================
    // MOBILIDADE DAS DAMAS
    // =========================================================================
    
    // Damas brancas
    u64 damas_brancas = bitboards[Q];
    while (damas_brancas)
    {
        int casa = getLeastBitIndex(damas_brancas);
        u64 ataques = obterAtaquesDama(casa, ocupacao_total);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_brancas = bitboards[P] | bitboards[N] | bitboards[B] | bitboards[R] | bitboards[Q] | bitboards[K];
        ataques &= ~pecas_brancas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score += movimentos * mobility_bonus_queen;
        
        clearBit(damas_brancas, casa);
    }

    // Damas pretas
    u64 damas_pretas = bitboards[q];
    while (damas_pretas)
    {
        int casa = getLeastBitIndex(damas_pretas);
        u64 ataques = obterAtaquesDama(casa, ocupacao_total);
        
        // Remover casas ocupadas por peças próprias
        u64 pecas_pretas = bitboards[p] | bitboards[n] | bitboards[b] | bitboards[r] | bitboards[q] | bitboards[k];
        ataques &= ~pecas_pretas;
        
        // Contar movimentos disponíveis
        int movimentos = __builtin_popcountll(ataques);
        score -= movimentos * mobility_bonus_queen;
        
        clearBit(damas_pretas, casa);
    }

    return score;
}



/**
 * @brief Função principal de avaliação da posição
 *
 * ESTRATÉGIA DE AVALIAÇÃO:
 * ========================
 * A função combina múltiplas heurísticas para avaliar uma posição de xadrez:
 *
 * 1. MATERIAL: Soma dos valores das peças (Peão=100, Cavalo=320, etc.)
 * 2. POSICIONAL: Bônus/penalidades baseados na posição das peças no tabuleiro
 * 3. ESTRUTURA DE PEÕES: Analisa fraquezas e forças na formação de peões
 *
 * SISTEMA DE PONTUAÇÃO:
 * =====================
 * - Valores são dados em "centipawns" (1 peão = 100 centipawns)
 * - Valores POSITIVOS favorecem as BRANCAS
 * - Valores NEGATIVOS favorecem as PRETAS
 * - O resultado final é ajustado pelo lado que está jogando
 *
 * EXEMPLOS DE VALORES TÍPICOS:
 * =============================
 * - Posição inicial: ≈ 0 pontos (equilibrada)
 * - Vantagem de 1 peão: ≈ +100 pontos
 * - Vantagem de 1 cavalo: ≈ +320 pontos
 * - Mate iminente: ≈ ±9999 pontos
 *
 * @return Valor total da avaliação do ponto de vista do jogador atual
 */
int evaluate()
{
    int score = 0;
    int component_score;

    // Avaliação material
    component_score = evaluate_material();
    score += component_score;

    // Avaliação posicional
    component_score = evaluate_positional();
    score += component_score;

    // Avaliação de estrutura de peões (dobrados, isolados, passados)
    component_score = evaluate_pawn_structure();
    score += component_score;

    // Avaliação de colunas livres e semi-livres
    component_score = evaluate_open_files();
    score += component_score;

    // Avaliação de mobilidade das peças
    component_score = evaluate_mobility();
    score += component_score;

    // Avaliação de segurança do rei
    component_score = evaluate_king_safety();
    score += component_score;

    

    // Retorna a avaliação do ponto de vista do lado a jogar
    return (lado_a_jogar == branco) ? score : -score;
}
