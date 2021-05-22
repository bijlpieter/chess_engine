#include "uci.h"

#include <sstream>

std::string move_notation(const Position& pos, Move m) {
	std::stringstream ss("");
	if (move_type(m) != S_MOVE_CASTLING)
		ss << " NBRQK   NBRQK "[pos.piece_on(move_from(m))] << (pos.piece_on(move_to(m)) == NO_PIECE ? "" : "x") << "abcdefgh"[file(move_to(m))] << "12345678"[rank(move_to(m))];
	else {
		if (file(move_to(m)) == FILE_H)
			ss << "O-O";
		else
			ss << "O-O-O";
	}
	if (move_type(m) == S_MOVE_PROMOTION)
		ss << "=" << "NBRQ"[move_promo(m)];
	return ss.str();
}

std::string sq_notation(Square s) {
	std::stringstream ss("");
	ss << "abcdefgh"[file(s)] << "12345678"[rank(s)];
	return ss.str();
}