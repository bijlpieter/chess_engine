#include "types.h"
#include "move_generation.h"

#include <iostream>

bool default_position() {
	PositionInfo info = {0};
	Position pos = Position(&info);

	const uint64_t perft_results[] = {20, 400, 8902, 197281, 4865609, 119060324, 3195901860};

	std::cout << "Default position: " << std::endl;
	for (int i = 0; i < 7; i++) {
		uint64_t nodes = pos.perft(i + 1);
		std::cout << "    Depth " << i + 1 << ": " << nodes << std::endl;
		if (nodes != perft_results[i]) {
			std::cout << "        error: expected " << perft_results[i] << std::endl;
			return false;
		}
	}

	return true;
}

bool kiwipete() {
	PositionInfo info = {0};
	Position pos = Position(&info, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

	const uint64_t perft_results[] = {48, 2039, 97862, 4085603, 193690690, 8031647685};

	std::cout << "Kiwipete: " << std::endl;
	for (int i = 0; i < 6; i++) {
		uint64_t nodes = pos.perft(i + 1);
		std::cout << "    Depth " << i + 1 << ": " << nodes << std::endl;
		if (nodes != perft_results[i]) {
			std::cout << "        error: expected " << perft_results[i] << std::endl;
			return false;
		}
	}

	return true;
}

bool en_passant() {
	PositionInfo info = {0};
	Position pos = Position(&info, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");

	const uint64_t perft_results[] = {14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};

	std::cout << "En Passant: " << std::endl;
	for (int i = 0; i < 7; i++) {
		uint64_t nodes = pos.perft(i + 1);
		std::cout << "    Depth " << i + 1 << ": " << nodes << std::endl;
		if (nodes != perft_results[i]) {
			std::cout << "        error: expected " << perft_results[i] << std::endl;
			return false;
		}
	}

	return true;
}

bool weird() {
	PositionInfo info = {0};
	Position pos = Position(&info, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

	const uint64_t perft_results[] = {6, 264, 9467, 422333, 15833292, 706045033};

	std::cout << "Weird: " << std::endl;
	for (int i = 0; i < 6; i++) {
		uint64_t nodes = pos.perft(i + 1);
		std::cout << "    Depth " << i + 1 << ": " << nodes << std::endl;
		if (nodes != perft_results[i]) {
			std::cout << "        error: expected " << perft_results[i] << std::endl;
			return false;
		}
	}

	return true;
}

bool talkchess() {
	PositionInfo info = {0};
	Position pos = Position(&info, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");

	const uint64_t perft_results[] = {44, 1486, 62379, 2103487, 89941194};

	std::cout << "Talkchess: " << std::endl;
	for (int i = 0; i < 5; i++) {
		uint64_t nodes = pos.perft(i + 1);
		std::cout << "    Depth " << i + 1 << ": " << nodes << std::endl;
		if (nodes != perft_results[i]) {
			std::cout << "        error: expected " << perft_results[i] << std::endl;
			return false;
		}
	}

	return true;
}

bool alternative() {
	PositionInfo info = {0};
	Position pos = Position(&info, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

	const uint64_t perft_results[] = {46, 2079, 89890, 3894594, 164075551, 6923051137};

	std::cout << "Alternative: " << std::endl;
	for (int i = 0; i < 6; i++) {
		uint64_t nodes = pos.perft(i + 1);
		std::cout << "    Depth " << i + 1 << ": " << nodes << std::endl;
		if (nodes != perft_results[i]) {
			std::cout << "        error: expected " << perft_results[i] << std::endl;
			return false;
		}
	}

	return true;
}

int main() {
	bb_init();
	bb_moves_init();
	bb_rays_init();

	if (!default_position()) return 1;
	if (!kiwipete()) return 1;
	if (!en_passant()) return 1;
	if (!weird()) return 1;
	if (!talkchess()) return 1;
	if (!alternative()) return 1;

	return 0;
}