#include "types.h"
#include "move_generation.h"
#include "uci.h"

#include <iostream>
#include <chrono>

bool test_position(std::string fen, const uint64_t* results, int depth = 5) {
	PositionInfo info = {0};
	Position pos = Position(&info, fen);

	for (int i = 0; i < depth; i++) {
		auto t1 = std::chrono::high_resolution_clock::now();
		uint64_t nodes = pos.perft(i + 1);
		auto t2 = std::chrono::high_resolution_clock::now();
		auto sec = std::chrono::duration<double>(t2 - t1).count();
		std::cout << "    Depth " << i + 1 << ": " << nodes << "    (" << sec << " sec)" << std::endl;
		if (nodes != results[i]) {
			std::cout << "        error: expected " << results[i] << std::endl;
			return false;
		}
	}

	return true;
}

bool default_position() {
	const uint64_t perft_results[] = {20, 400, 8902, 197281, 4865609, 119060324, 3195901860};
	std::cout << "Default position: " << std::endl;
	return test_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", perft_results);
}

bool kiwipete() {
	const uint64_t perft_results[] = {48, 2039, 97862, 4085603, 193690690, 8031647685};
	std::cout << "Kiwipete: " << std::endl;
	return test_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", perft_results);
}

bool en_passant() {
	const uint64_t perft_results[] = {14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};
	std::cout << "En Passant: " << std::endl;
	return test_position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", perft_results);
}

bool weird() {
	const uint64_t perft_results[] = {6, 264, 9467, 422333, 15833292, 706045033};
	std::cout << "Weird: " << std::endl;
	return test_position("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", perft_results);
}

bool talkchess() {
	const uint64_t perft_results[] = {44, 1486, 62379, 2103487, 89941194};
	std::cout << "Talkchess: " << std::endl;
	return test_position("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", perft_results);
}

bool alternative() {
	const uint64_t perft_results[] = {46, 2079, 89890, 3894594, 164075551, 6923051137};
	std::cout << "Alternative: " << std::endl;
	return test_position("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", perft_results);
}

void testing() {
	PositionInfo info = {0};
	Position pos = Position(&info); // , "rnbqkbnr/pppppppp/8/8/2P5/8/PP1PPPPP/RNBQKBNR b KQkq - 0 1"
	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << pos << std::endl;

	// for (int i = 0; i < 1000000; i++) {
		PositionInfo info2 = {0};
		pos.play_move(move_init(E2, E4), &info2);
		// pos.unplay_move(move_init(E2, E4));
		// pos.info_init();
		// pos.generate_moves();
	// }

	// std::cout << pos << std::endl;
	// std::cout << bb_string(pos.state->pinned) << std::endl;

	std::cout << pos.divide(1) << std::endl;

	auto t2 = std::chrono::high_resolution_clock::now();
	auto sec = std::chrono::duration<double>(t2 - t1).count();

	std::cout << sec << " seconds" << std::endl;

}

void test_movepicker() {
	SearchThread thread;
	std::cout << thread.mp_perft(3) << std::endl;
}

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();

	// testing();

	test_movepicker();

	// if (!default_position()) return 1;
	// if (!kiwipete()) return 1;
	// if (!en_passant()) return 1;
	// if (!weird()) return 1;
	// if (!talkchess()) return 1;
	// if (!alternative()) return 1;

	return 0;
}