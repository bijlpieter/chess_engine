#ifndef _MOVE_H_
#define _MOVE_H_

#include <cstdint>
#include "board.h"

typedef uint16_t Move;

inline constexpr Square move_to(Move m) {
	return Square(m & 0x3F);            // 0000000000111111
}

inline constexpr Square move_from(Move m) {
	return Square(m & 0xFC0);           // 0000111111000000
}

enum PromotionPiece {
	PROMOTION_KNIGHT,
	PROMOTION_BISHOP,
	PROMOTION_ROOK,
	PROMOTION_QUEEN
};

inline constexpr PromotionPiece move_promo(Move m) {
	return PromotionPiece(m & 0x3000);  // 0011000000000000
}

enum MoveType {
	S_MOVE_NONE,
	S_MOVE_PROMOTION,
	S_MOVE_EN_PASSANT,
	S_MOVE_CASTLING
};

inline constexpr MoveType move_type(Move m) {
	return MoveType(m & 0xC000);        // 1100000000000000
}

#endif