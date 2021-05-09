#ifndef _BOARD_H_
#define _BOARD_H_

#include <stdint.h>

enum Rank : char {
	RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,
	NUM_RANKS
};

enum File : char {
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
	NUM_FILES
};

enum Square : char {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	NUM_SQUARES, NO_SQUARE
};

typedef uint8_t Castling;
enum CastlingTypes : char {
	NO_CASTLING,
	WHITE_KINGSIDE = 1,
	WHITE_QUEENSIDE = 2,
	BLACK_KINGSIDE = 4,
	BLACK_QUEENSIDE = 8,

	KINGSIDE = 5,
	QUEENSIDE = 10,

	WHITE_BOTH = WHITE_KINGSIDE | WHITE_QUEENSIDE,
	BLACK_BOTH = BLACK_KINGSIDE | BLACK_QUEENSIDE,

	ALL_CASTLING = WHITE_BOTH | BLACK_BOTH,
	NUM_CASTLING = 16
};

inline constexpr Rank rank(Square s) {
	return Rank(s >> 3);
}

inline constexpr File file(Square s) {
	return File(s & 7);
}

inline constexpr Square square(Rank r, File f) {
	return Square((r << 3) + f);
}

inline Rank& operator++(Rank& r, int) { return r = Rank(int(r) + 1); }
inline Rank& operator--(Rank& r, int) { return r = Rank(int(r) - 1); }
inline File& operator++(File& f, int) { return f = File(int(f) + 1); }
inline File& operator--(File& f, int) { return f = File(int(f) - 1); }
inline Square& operator++(Square& s, int) { return s = Square(int(s) + 1); }
inline Square& operator--(Square& s, int) { return s = Square(int(s) - 1); }
inline Square& operator+=(Square& s, int x) { return s = Square(int(s) + x); }
inline Square operator+(Square& s, int x) { return Square(int(s) + x); }

#include "direction.h"

inline Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
inline Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }

#endif