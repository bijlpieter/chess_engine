#ifndef _HISTORY_H_
#define _HISTORY_H_

#include "types.h"

class History {
  History() = delete;
  ~History() = delete;

  public:
    struct Heuristics {
      Heuristics() : h(0), ch(0), fh(0) {}
      int h, ch, fh;
    };

  public:
    static void updateHistory(SearchThread* search, Move* quiets, int nrQuiets, int ply, int bonus);
    static void getHistory(SearchThread* search, Move move, int ply, Heuristics &H);
    static void updateHist(Value& hist, Value score);
  private:
    static constexpr int histMax = 400;
    static constexpr int histMult = 32;
    static constexpr int histDiv = 512;
};

constexpr int History::histMax;
constexpr int History::histMult;
constexpr int History::histDiv;


void History::updateHist(Value& hist, Value score) {
	hist += score * histMult - hist * abs(score) / histDiv;
}

void History :: updateHistory(SearchThread* search, Move* quiets, int nrQuiets, int ply, int bonus) {
	if(ply < 2 || !nrQuiets) /// we can't update if we don't have a follow move or no quiets
		return;

	Move counterMove = search->stack[ply - 1].move, followMove = search->stack[ply - 2].move;
	Piece counterPiece = search->stack[ply - 1].piece, followPiece = search->stack[ply - 2].piece;
	Square counterTo = move_to(counterMove), followTo = move_to(followMove);

	Move best = quiets[nrQuiets - 1];
	Color turn = search->pos->turn;

	if (counterMove)
		search->cmTable[~turn][counterPiece][counterTo] = best; /// update counter move table

	bonus = std::min(bonus, histMax);

	for (int i = 0; i < nrQuiets; i++) {
		Move move = quiets[i];
		Value score = (move == best ? bonus : -bonus);
		Square from = move_from(move), to = move_to(move);
		Piece piece = search->pos->piece_on(from);

		updateHist(search->history[turn][from][to], score);

		if(counterMove)
			updateHist(search->follow[0][counterPiece][counterTo][piece][to], score);

		if(followMove)
			updateHist(search->follow[1][followPiece][followTo][piece][to], score);
	}
}

void History :: getHistory(SearchThread* search, Move move, int ply, Heuristics &H) {
	Square from = move_from(move), to = move_to(move);
	Piece piece = search->pos->piece_on(from);

	H.h = search->history[search->pos->turn][from][to];

	Move counterMove = search->stack[ply - 1].move, followMove = (ply >= 2 ? search->stack[ply - 2].move : NULL_MOVE);
	Piece counterPiece = search->stack[ply - 1].piece, followPiece = (ply >= 2 ? search->stack[ply - 2].piece : NO_PIECE);
	Square counterTo = move_to(counterMove), followTo = move_to(followMove);

	H.ch = search->follow[0][counterPiece][counterTo][piece][to];

	H.fh = search->follow[1][followPiece][followTo][piece][to];
}

#endif