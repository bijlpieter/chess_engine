#ifndef _DIRECTION_H_
#define _DIRECTION_H_

//
//  56   57   58   59   60   61   62   63     8
//
//  48   49   50   51   52   53   54   55     7
//
//  40   41   42   43   44   45   46   47     6
//
//  32   33   34   35   36   37   38   39     5
//
//  24   25   26   27   28   29   30   31     4
//
//  16   17   18   19   20   21   22   23     3
//
//  08   09   10   11   12   13   14   15     2
//
//  00   01   02   03   04   05   06   07     1
//
//   a    b    c    d    e    f    g    h

enum FlipDirection : char{
	VERTICALLY,
	HORIZONTALLY
};

enum Direction : char {
	DOWN = -8,
	LEFT = -1,
	NO_DIRECTION = 0,
	RIGHT = 1,
	UP = 8,
	
	DOWN_LEFT = DOWN + LEFT,
	DOWN_RIGHT = DOWN + RIGHT,
	UP_LEFT = UP + LEFT,
	UP_RIGHT = UP + RIGHT,

	UP_UP = UP + UP,
	DOWN_DOWN = DOWN + DOWN,

	DOWN_DOWN_LEFT = DOWN + DOWN + LEFT,
	DOWN_DOWN_RIGHT = DOWN + DOWN + RIGHT,
	DOWN_LEFT_LEFT = DOWN + LEFT + LEFT,
	DOWN_RIGHT_RIGHT = DOWN + RIGHT + RIGHT,
	UP_UP_LEFT = UP + UP + LEFT,
	UP_UP_RIGHT = UP + UP + RIGHT,
	UP_RIGHT_RIGHT = UP + RIGHT + RIGHT,
	UP_LEFT_LEFT = UP + LEFT + LEFT
};

const Direction KingDirections[] = {DOWN_LEFT, DOWN, DOWN_RIGHT, LEFT, RIGHT, UP_LEFT, UP, UP_RIGHT};
const Direction KnightDirections[] = {DOWN_DOWN_LEFT, DOWN_DOWN_RIGHT, DOWN_RIGHT_RIGHT, UP_RIGHT_RIGHT, UP_UP_RIGHT, UP_UP_LEFT, UP_LEFT_LEFT, DOWN_LEFT_LEFT};
const Direction BishopDirections[] = {UP_RIGHT, UP_LEFT, DOWN_RIGHT, DOWN_LEFT};
const Direction RookDirections[] = {UP, LEFT, DOWN, RIGHT};

#endif