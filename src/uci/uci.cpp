#include "uci.h"

#include <iostream>

void move_notation(Position pos, Move m) {
	if (move_type(m) != S_MOVE_CASTLING)
		std::cout << " NBRQK   NBRQK "[pos.piece_on(move_from(m))] << (pos.piece_on(move_to(m)) == NO_PIECE ? "" : "x") << "abcdefgh"[file(move_to(m))] << "12345678"[rank(move_to(m))];
	else {
		if (file(move_to(m)) == FILE_G)
			std::cout << "O-O";
		else
			std::cout << "O-O-O";
	}
	if (move_type(m) == S_MOVE_PROMOTION)
		std::cout << "=" << "NBRQ"[move_promo(m) >> 12];
	std::cout << std::endl;
}