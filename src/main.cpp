#include "types.h"
#include "move_generation.h"
#include "uci.h"

#include <iostream>

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();


	// for (Square s = A1; s <= H8; s++)
	// 	std::cout << bb_string(BB_RAYS[s][E4]) << std::endl;

	// std::cout << bb_string(queen_moves(A1, 0)) << std::endl;

	// for (int i = 0; i < NUM_CASTLING; i++)
	// 	std::cout << bb_string(BB_CASTLING_ROOK[i]) << std::endl;
	// std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	// std::string fen1 = "2r2b2/8/8/2pP4/8/2K5/8/8 w KQkq - 0 1";

	PositionInfo info = {0};
	PositionInfo info2;
	Position pos = Position(&info);
	// std::cout << pos << std::endl;
	// pos.play_move(move_init(B7, B5), &info2);
	// std::cout << pos << std::endl;
	std::cout << pos.perft(7) << std::endl;
	
	// for (Color c : {WHITE, BLACK})
	// 	for (PieceType pt : {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING})
			
	// 		std::cout << bb_string(p.pieces[c][pt]) << std::endl;

	// for (Color c : {WHITE, BLACK})
	// 	std::cout << bb_string(p.colors[c]) << std::endl;

	// std::cout << bb_string(pos.blockers(H1, WHITE, BLACK)) << std::endl;

	// Moves moves = pos.legal_moves();
	// for (MoveCount i = 0; i < moves.size(); i++)
	// 	std::cout << move_notation(pos, moves.list[i]) << std::endl;

	// // std::cout << bb_string(pos.state->king_unsafe) << std::endl;
	// std::cout << "Number of moves: " << moves.size() << std::endl;

	// std::cout << bb_string(BLACK_LEFT_FIANCHETTO) << std::endl;
	// std::cout << bb_string(BLACK_RIGHT_FIANCHETTO) << std::endl;

	return 0;
}