#ifndef _SEARCH_TYPES_H_
#define _SEARCH_TYPES_H_

#include <cstdint>

typedef uint8_t Depth;
typedef int16_t Value;

enum Bound { NO_BOUND, UPPER_BOUND, LOWER_BOUND, EXACT_BOUND };

#endif