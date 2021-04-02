#ifndef _PIECE_H_
#define _PIECE_H_

enum PieceType : char {
	PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
	NUM_PIECE_TYPES
};

enum Color : char {
	WHITE, BLACK,
	NUM_COLORS
};

enum Piece : char {
	NO_PIECE = -1,
	WHITE_PAWN = 0, WHITE_KNIGHT = 1, WHITE_BISHOP = 2, WHITE_ROOK = 3, WHITE_QUEEN = 4, WHITE_KING = 5,
	BLACK_PAWN = 8, BLACK_KNIGHT = 9, BLACK_BISHOP = 10, BLACK_ROOK = 11, BLACK_QUEEN = 12, BLACK_KING = 13,
	NUM_PIECES = 12
};

inline Color piece_color(Piece p) {
	return Color((p & 8) >> 3);
}

inline PieceType piece_type(Piece p) {
	return PieceType(p & 7);
}

inline Piece piece_init(PieceType p, Color c) {
	return Piece(p & (c << 3));
}

#endif