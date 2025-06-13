#include "bitboard.h"
#include "ataques.h"

int main()
{
    u64 mainBitboard = 0ULL;

    gerar_ataques_pecas();

    u64 block = 0ULL;
    setBit(block, a1);
    setBit(block, a7);
    setBit(block, g1);
    setBit(block, h8);
    printBitboard(block);

    printBitboard(gerar_ataque_bispo(d4));
    printBitboard(gerar_ataque_bispo_tempo_real(d4, block));

    return 0;
}
