#include "types.h"
#include <cstring>

SearchThread::SearchThread() : pawn_hash_table(131072) {
	info = new PositionInfo();
	pos = new Position(info); // , "rnbqkbnr/pppppppp/8/8/8/5N2/PPPPPPPP/RNBQKB1R b KQkq - 0 1"
	
	memset(pvTable, 0, sizeof(pvTable));
	memset(killers, 0, sizeof(killers));
	memset(history, 0, sizeof(history));
	memset(follow, 0, sizeof(follow));
	memset(stack, 0, sizeof(stack));
}

SearchThread::~SearchThread() {
	delete pos;
	delete info;
}