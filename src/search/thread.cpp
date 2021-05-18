#include "types.h"
#include <cstring>

SearchThread::SearchThread() {
	info = new PositionInfo();
	pos = new Position(info);

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