#ifndef _ZOBRIST_H_
#define _ZOBRIST_H_

#include "random.h"
#include "board.h"
#include "piece.h"
typedef uint64_t Key;

struct Zobrist {
    Key piece_square[NUM_PIECES][NUM_SQUARES];
    Key en_passant[NUM_FILES];
    Key castling[NUM_CASTLING];
    Key side, no_pawns;
    Zobrist() {
        Random r(69420);
        for (int p = WHITE_PAWN; p < BLACK_KING; p++)
            for (Square s = A1; s <= H8; s++)
                piece_square[p][s] = r.rand();

        for (File f = FILE_A; f <= FILE_H; f++)
            en_passant[f] = r.rand();

        for(int cr = NO_CASTLING; cr <= ALL_CASTLING; cr++)
            castling[cr] = r.rand();

        side = r.rand();
        no_pawns = r.rand();
    }
};

extern Zobrist zobrist;

#endif