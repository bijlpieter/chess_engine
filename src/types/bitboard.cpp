#include "bitboard.h"
#include "direction.h"

Bitboard BB_RANKS[NUM_RANKS];
Bitboard BB_FILES[NUM_FILES];
Bitboard BB_SQUARES[NUM_SQUARES];
Bitboard BB_CASTLING[NUM_CASTLING] = {0};

void bb_init() {
	for (Rank r = RANK_1; r <= RANK_8; r++)
		BB_RANKS[r] = 0xFFULL << (r * UP);

	for (File f = FILE_A; f <= FILE_H; f++)
		BB_FILES[f] = 0x0101010101010101ULL << (f * RIGHT);
		
	for (Square s = A1; s <= H8; s++)
		BB_SQUARES[s] = 0x1ULL << (s * RIGHT);

	Bitboard white_kingside = 0x70ULL;
	Bitboard white_queenside = 0x1CULL;
	Bitboard black_kingside = 0x70ULL << 56;
	Bitboard black_queenside = 0x1CULL << 56;

	for (int i = 0; i < NUM_CASTLING; i++) {
		if (i & WHITE_KINGSIDE)
			BB_CASTLING[i] |= white_kingside;
		if (i & WHITE_QUEENSIDE)
			BB_CASTLING[i] |= white_queenside;
		if (i & BLACK_KINGSIDE)
			BB_CASTLING[i] |= black_kingside;
		if (i & BLACK_QUEENSIDE)
			BB_CASTLING[i] |= black_queenside;
	}
}

std::string bb_string(Bitboard bb) {
	std::string str = "";
	for (Rank r = RANK_8; r >= RANK_1; r--) {
		for (File f = FILE_A; f <= FILE_H; f++)
			str += (bb & square(r, f) ? "1 " : ". ");
		str += '\n';
	}
	return str;
}