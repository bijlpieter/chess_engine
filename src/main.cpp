#include "./types/types.h"

#include <iostream>

int main() {
    bb_init();
    std::cout << bb_string(BB_SQUARES[G4]);
    return 0;
}