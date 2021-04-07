#include "uci.h"

#include <iostream>

void move_notation(Position pos, Move m) {
	std::cout << "PNBRQK  pnbrqk "[pos.piece_on(move_from(m))] << (pos.piece_on(move_to(m)) == NO_PIECE ? "" : "x") << "abcdefgh"[file(move_to(m))] << "12345678"[rank(move_to(m))] << std::endl;
}