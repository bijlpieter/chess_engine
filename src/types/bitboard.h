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

inline int popcount(Bitboard b) {
	return _popcnt64(b);
}

inline Square lsb(Bitboard b) {
	return Square(_tzcnt_u64(b));
}

inline Square msb(Bitboard b) {
	return Square(_lzcnt_u64(b) ^ 63);
}

inline constexpr Bitboard flip(Bitboard bb, FlipDirection D) {
	switch (D) {
	case VERTICALLY:
		return _bswap64(bb);
	case HORIZONTALLY:
		return bb;
	}
	return bb;
}

inline constexpr Bitboard shift(Bitboard bb, Direction D) {
	switch(D) {
	case UP:
		return bb << UP;
	case UP_LEFT:
		return (bb << UP_LEFT) & ~BB_FILES[FILE_H];
	case UP_RIGHT:
		return (bb << UP_RIGHT) & ~BB_FILES[FILE_A];
	case RIGHT:
		return (bb << RIGHT) & ~BB_FILES[FILE_A];

	case NO_DIRECTION:
		return bb;

	case LEFT:
		return (bb >> -LEFT) & ~BB_FILES[FILE_H];
	case DOWN_RIGHT:
		return (bb >> -DOWN_RIGHT) & ~BB_FILES[FILE_A];
	case DOWN:
		return (bb >> -DOWN);
	case DOWN_LEFT:
		return (bb >> -DOWN_LEFT) & ~BB_FILES[FILE_H];

	case UP_UP:
		return bb << UP_UP;
	case DOWN_DOWN:
		return bb >> -DOWN_DOWN;

	case UP_UP_RIGHT:
		return (bb << UP_UP_RIGHT) & ~BB_FILES[FILE_A];
	case UP_UP_LEFT:
		return (bb << UP_UP_LEFT) & ~BB_FILES[FILE_H];
	case UP_RIGHT_RIGHT:
		return (bb << UP_RIGHT_RIGHT) & ~(BB_FILES[FILE_A] | BB_FILES[FILE_B]);
	case UP_LEFT_LEFT:
		return (bb << UP_LEFT_LEFT) & ~(BB_FILES[FILE_G] | BB_FILES[FILE_H]);

	case DOWN_DOWN_RIGHT:
		return (bb >> -DOWN_DOWN_RIGHT) & ~BB_FILES[FILE_A];
	case DOWN_DOWN_LEFT:
		return (bb >> -DOWN_DOWN_LEFT) & ~BB_FILES[FILE_H];
	case DOWN_RIGHT_RIGHT:
		return (bb >> -DOWN_RIGHT_RIGHT) & ~(BB_FILES[FILE_A] | BB_FILES[FILE_B]);
	case DOWN_LEFT_LEFT:
		return (bb >> -DOWN_LEFT_LEFT) & ~(BB_FILES[FILE_G] | BB_FILES[FILE_H]);
	}
	return bb;
}

void bb_init();
std::string bb_string(Bitboard bb);

#endif