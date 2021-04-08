#ifndef _BITBOARD_H_
#define _BITBOARD_H_

#include <cstdint>
#include <string>
#include <x86intrin.h>

#include "board.h"
#include "direction.h"

typedef uint64_t Bitboard;

extern Bitboard BB_RANKS[NUM_RANKS];
extern Bitboard BB_FILES[NUM_FILES];
extern Bitboard BB_SQUARES[NUM_SQUARES];
extern Bitboard BB_CASTLING_KING[NUM_CASTLING];
extern Bitboard BB_CASTLING_ROOK[NUM_CASTLING];
extern Bitboard BB_RAYS[NUM_SQUARES][NUM_SQUARES];
extern Bitboard BB_LINES[NUM_SQUARES][NUM_SQUARES];

inline Bitboard bb_ray(Square from, Square to) {
	return BB_RAYS[from][to];
}

inline Bitboard bb_line(Square from, Square to) {
	return BB_LINES[from][to];
}

inline uint64_t popcount(Bitboard bb) {
	return _popcnt64(bb);
}

inline uint64_t pext(Bitboard occ, Bitboard mask) {
	return _pext_u64(occ, mask);
}

inline Square lsb(Bitboard bb) {
	return Square(__builtin_ctzll(bb));
}

inline Square pop_lsb(Bitboard& bb) {
	Square s = lsb(bb);
	bb &= (bb - 1);
	return s;
}

inline Square msb(Bitboard b) {
	return Square(__builtin_clzll(b) ^ 63);
}

inline constexpr Bitboard flip(Bitboard bb, FlipDirection D) {
	switch (D) {
	case VERTICALLY:
		return _bswap64(bb);
	//imp later
	case HORIZONTALLY:
		return bb;
	}
	return bb;
}

inline Bitboard operator|(Bitboard bb, Square s) { return bb | BB_SQUARES[s]; }
inline Bitboard operator|(Bitboard bb, Rank r) { return bb | BB_RANKS[r]; }
inline Bitboard operator|(Bitboard bb, File f) { return bb | BB_FILES[f]; }

inline Bitboard operator&(Bitboard bb, Square s) { return bb & BB_SQUARES[s]; }
inline Bitboard operator&(Bitboard bb, Rank r) { return bb & BB_RANKS[r]; }
inline Bitboard operator&(Bitboard bb, File f) { return bb & BB_FILES[f]; }

inline Bitboard operator|(Rank r1, Rank r2) { return BB_RANKS[r1] | BB_RANKS[r2]; }
inline Bitboard operator|(Rank r, File f) { return BB_RANKS[r] | BB_FILES[f]; }
inline Bitboard operator|(File f, Rank r) { return BB_RANKS[r] | BB_FILES[f]; }
inline Bitboard operator|(File f1, File f2) { return BB_FILES[f1] | BB_FILES[f2]; }

inline Bitboard operator~(Rank r) { return ~BB_RANKS[r]; }
inline Bitboard operator~(File f) { return ~BB_FILES[f]; }
inline Bitboard operator~(Square s) { return ~BB_SQUARES[s]; }

inline constexpr Bitboard shift(Bitboard bb, Direction D) {
	switch(D) {
	case UP:
		return bb << UP;
	case UP_LEFT:
		return (bb << UP_LEFT) & ~FILE_H;
	case UP_RIGHT:
		return (bb << UP_RIGHT) & ~FILE_A;
	case RIGHT:
		return (bb << RIGHT) & ~FILE_A;

	case NO_DIRECTION:
		return bb;

	case LEFT:
		return (bb >> -LEFT) & ~FILE_H;
	case DOWN_RIGHT:
		return (bb >> -DOWN_RIGHT) & ~FILE_A;
	case DOWN:
		return (bb >> -DOWN);
	case DOWN_LEFT:
		return (bb >> -DOWN_LEFT) & ~FILE_H;

	case UP_UP:
		return bb << UP_UP;
	case DOWN_DOWN:
		return bb >> -DOWN_DOWN;

	case UP_UP_RIGHT:
		return (bb << UP_UP_RIGHT) & ~FILE_A;
	case UP_UP_LEFT:
		return (bb << UP_UP_LEFT) & ~FILE_H;
	case UP_RIGHT_RIGHT:
		return (bb << UP_RIGHT_RIGHT) & ~(FILE_A | FILE_B);
	case UP_LEFT_LEFT:
		return (bb << UP_LEFT_LEFT) & ~(FILE_G | FILE_H);

	case DOWN_DOWN_RIGHT:
		return (bb >> -DOWN_DOWN_RIGHT) & ~FILE_A;
	case DOWN_DOWN_LEFT:
		return (bb >> -DOWN_DOWN_LEFT) & ~FILE_H;
	case DOWN_RIGHT_RIGHT:
		return (bb >> -DOWN_RIGHT_RIGHT) & ~(FILE_A | FILE_B);
	case DOWN_LEFT_LEFT:
		return (bb >> -DOWN_LEFT_LEFT) & ~(FILE_G | FILE_H);
	}
	return bb;
}

void bb_init();
void bb_rays_init();
std::string bb_string(Bitboard bb);

#endif