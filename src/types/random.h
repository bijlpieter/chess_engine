#ifndef _RANDOM_H_
#define _RANDOM_H_
struct Random{
    uint64_t r;
    uint64_t rand() {
        r ^= r >> 12, r ^= r << 25, r ^= r >> 27;
        return r * 2685821657736338717LL;
    }
    Random(uint64_t seed){
        r = seed;
    }
};

#endif