#ifndef _MOVE_H_
#define _MOVE_H_

#include <cstdint>
#include "board.h"

#define NULL_MOVE 0

typedef uint16_t Move;

inline constexpr Square move_to(Move m) {
	return Square(m & 0x3F);            // 0000000000111111
}

inline constexpr Square move_from(Move m) {
	return Square((m & 0xFC0) >> 6);    // 0000111111000000
}

enum PromotionPiece {
	PROMOTION_KNIGHT,
	PROMOTION_BISHOP,
	PROMOTION_ROOK,
	PROMOTION_QUEEN
};

inline constexpr PromotionPiece move_promo(Move m) {
	return PromotionPiece((m & 0x3000) >> 12);  // 0011000000000000
}

enum MoveType {
	S_MOVE_NONE = 0,
	S_MOVE_PROMOTION = 1 << 14,
	S_MOVE_EN_PASSANT = 2 << 14,
	S_MOVE_CASTLING = 3 << 14
};

inline constexpr MoveType move_type(Move m) {
	return MoveType(m & 0xC000);        // 1100000000000000
}

inline constexpr Move move_init(Square from, Square to) {
	return to | (from << 6);
}

#define MAX_MOVES 256
typedef uint16_t MoveCount;

struct Moves {
	Move list[MAX_MOVES];
	Move* end = list;
	MoveCount size() { return end - list; }
	Move& operator[](int i) {
		return list[i];
	}
};

#endif