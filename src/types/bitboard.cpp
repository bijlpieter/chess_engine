#include "bitboard.h"
#include "direction.h"
#include "move_generation.h"

Bitboard BB_RANKS[NUM_RANKS];
Bitboard BB_FILES[NUM_FILES];
Bitboard BB_SQUARES[NUM_SQUARES];
Bitboard BB_CASTLING_KING[NUM_CASTLING] = {0};
Bitboard BB_CASTLING_ROOK[NUM_CASTLING] = {0};
Bitboard BB_RAYS[NUM_SQUARES][NUM_SQUARES];
Bitboard BB_LINES[NUM_SQUARES][NUM_SQUARES];
Bitboard OUTPOSTS[NUM_COLORS];
Bitboard LIGHT_SQUARES;
Bitboard DARK_SQUARES;
Bitboard WHITE_LEFT_FIANCHETTO;
Bitboard WHITE_RIGHT_FIANCHETTO;
Bitboard BLACK_LEFT_FIANCHETTO;
Bitboard BLACK_RIGHT_FIANCHETTO;


void bb_init() {
	for (Rank r = RANK_1; r <= RANK_8; r++)
		BB_RANKS[r] = 0xFFULL << (r * UP);

	for (File f = FILE_A; f <= FILE_H; f++)
		BB_FILES[f] = 0x0101010101010101ULL << (f * RIGHT);
		
	for (Square s = A1; s <= H8; s++)
		BB_SQUARES[s] = 0x1ULL << (s * RIGHT);
	WHITE_LEFT_FIANCHETTO = 0x2020500ULL;
	WHITE_RIGHT_FIANCHETTO = 0x4040A000ULL;
	BLACK_LEFT_FIANCHETTO = flip(WHITE_LEFT_FIANCHETTO, VERTICALLY);
	BLACK_RIGHT_FIANCHETTO = flip(WHITE_RIGHT_FIANCHETTO, VERTICALLY);

	LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
	DARK_SQUARES = 0xAA55AA55AA55AA55ULL;
	OUTPOSTS[WHITE] = 0x00ffffffff000000ULL;
	OUTPOSTS[BLACK] = 0x000000ffffffff00ULL;
	Bitboard white_kingside_king = 0x70ULL;
	Bitboard white_queenside_king = 0x1CULL;
	Bitboard black_kingside_king = 0x70ULL << 56;
	Bitboard black_queenside_king = 0x1CULL << 56;

	Bitboard white_kingside_rook = 0x60ULL;
	Bitboard white_queenside_rook = 0xEULL;
	Bitboard black_kingside_rook = 0x60ULL << 56;
	Bitboard black_queenside_rook = 0xEULL << 56;

	for (int i = 0; i < NUM_CASTLING; i++) {
		if (i & WHITE_KINGSIDE) {
			BB_CASTLING_KING[i] |= white_kingside_king;
			BB_CASTLING_ROOK[i] |= white_kingside_rook;
		}
		if (i & WHITE_QUEENSIDE) {
			BB_CASTLING_KING[i] |= white_queenside_king;
			BB_CASTLING_ROOK[i] |= white_queenside_rook;
		}
		if (i & BLACK_KINGSIDE) {
			BB_CASTLING_KING[i] |= black_kingside_king;
			BB_CASTLING_ROOK[i] |= black_kingside_rook;
		}
		if (i & BLACK_QUEENSIDE) {
			BB_CASTLING_KING[i] |= black_queenside_king;
			BB_CASTLING_ROOK[i] |= black_queenside_rook;
		}
	}
}

void bb_rays_init() {
	for (Square s1 = A1; s1 <= H8; s1++) for (Square s2 = A1; s2 <= H8; s2++) {
		if (rook_moves(s1, 0) & s2) {
			BB_RAYS[s1][s2] = (rook_moves(s1, BB_SQUARES[s2]) & rook_moves(s2, BB_SQUARES[s1])) | s2;
			BB_LINES[s1][s2] = (rook_moves(s1, 0) & rook_moves(s2, 0)) | s1 | s2;
		}
		if (bishop_moves(s1, 0) & s2) {
			BB_RAYS[s1][s2] = (bishop_moves(s1, BB_SQUARES[s2]) & bishop_moves(s2, BB_SQUARES[s1])) | s2;
			BB_LINES[s1][s2] = (bishop_moves(s1, 0) & bishop_moves(s2, 0)) | s1 | s2;
		}
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