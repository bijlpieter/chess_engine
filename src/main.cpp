#include "types.h"
#include "move_generation.h"
#include "uci.h"

#include <iostream>

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();
	//for (Square s = A1; s <= H8; s++)
	// 	std::cout << bb_string(KING_RING[s]) << std::endl;

	// std::cout << bb_string(queen_moves(A1, 0)) << std::endl;

	// for (int i = 0; i < NUM_CASTLING; i++)
	// 	std::cout << bb_string(BB_CASTLING_ROOK[i]) << std::endl;
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	std::string fen1 = "2r2b2/8/8/2pP4/8/2K5/8/8 w KQkq - 0 1";
	std::string fen2 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";

	PositionInfo info = {0};
	// PositionInfo info2 = {0};
	Position pos = Position(&info, fen);
	std::cout << pos << std::endl;
	Score test = pos.calculate_score(WHITE);
	std::cout << test;
	// pos.play_move(move_init(H1, F1), &info2);
	// std::cout << pos << std::endl;
	// std::cout << "Castling: " << int(pos.state->castling) << std::endl;
	// std::cout << pos.perft(5) << std::endl;
	
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
	return 0;
}