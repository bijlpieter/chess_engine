#ifndef _UCI_H_
#define _UCI_H_

#include "types.h"

#include <string>

std::string move_notation(const Position& pos, Move m);
std::string sq_notation(Square s);

#endif