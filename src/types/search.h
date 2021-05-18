#ifndef _SEARCH_TYPES_H_
#define _SEARCH_TYPES_H_

#include <cstdint>

#define MAX_DEPTH 255

typedef uint8_t Depth;
typedef uint8_t Generation;

enum Bound { NO_BOUND, UPPER_BOUND, LOWER_BOUND, EXACT_BOUND };

#endif