#include "bitboard.h"
#include "direction.h"

Bitboard BB_EMPTY = 0;
Bitboard BB_UNIVERSE = -1;
Bitboard BB_RANKS[NUM_RANKS];
Bitboard BB_FILES[NUM_FILES];
Bitboard BB_SQUARES[NUM_SQUARES];

void bb_init() {
	for (Rank r = RANK_1; r <= RANK_8; r++)
		BB_RANKS[r] = 0xFFULL << (r * UP);

	for (File f = FILE_A; f <= FILE_H; f++)
		BB_FILES[f] = 0x0101010101010101ULL << (f * RIGHT);
		
	for (int i = A1; i <= H8; i++)
		BB_SQUARES[i] = 0x1ULL << (i * RIGHT);
}

std::string bb_string(Bitboard bb) {
	std::string str = "";
	for (Rank r = RANK_8; r >= RANK_1; r--) {
		for (File f = FILE_A; f <= FILE_H; f++)
			str += (bb & BB_SQUARES[square(r, f)] ? "1 " : ". ");
		str += '\n';
	}
	return str;
}