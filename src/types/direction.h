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
//   a    b    c   d    e    f    g    h

enum Direction : char {
	DOWN_LEFT = -9,
	DOWN = -8,
	DOWN_RIGHT = -7,
	LEFT = -1,
	NO_DIRECTION = 0,
	RIGHT = 1,
	UP_LEFT = 7,
	UP = 8,
	UP_RIGHT = 9
};

#endif