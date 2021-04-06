#include "position.h"
#include <iostream>
#include <string>

bool validateFEN(std::string fen) {
	let fields = fen.split(/\s+/)
	let positions = fields[0].split('/')
	// FEN consists of 6 fields
	if (fields.length !== 6){
		console.log("FEN Must have 6 fields.")
		return false;
	}
	//first field (positions field) must be 8 ranks long.
	if (positions.length !== 8) {
		console.log("Positions field (seperated by '/' must be of length 8.")
		return false;
	}
	//second field (move field) must be w or b
	if (!/^(w|b)$/.test(fields[1])) {
		console.log ("move field must be 'w' or 'b'")
		return false;
	}
	//third field (castling field) must be - or K/Q/k/q or any combination
	if (!/^(KQ?k?q?|Qk?q?|kq?|q|-)$/.test(fields[2])) {
		console.log("Castling field must be either - or a combination of K Q q k.")
		return false;
	}
	//fourth field (en passant) may be - or any coordinate
	if (!/^(-|[abcdefgh][36])$/.test(fields[3])) {
		console.log("En pessant field must be either - or a coordinate.")
		return false;
	}
	//en passant for the right turn?
	if ((fields[3][1] == '3' && fields[1] == 'w') || (fields[3][1] == '6' && fields[1] == 'b')) {
		console.log("En pessant field does not match the current turn")
		return false;
	}
	//fifth field (halfmove field) must be an integer > 0
	if (isNaN(fields[4]) || 0 > parseInt(fields[5], 10)) {
		console.log("Halfmove number must be an integer that is > 0.")
		return false;
	}
	
	//sixth field (fullmove field) must be an integer > 0
	if (isNaN(fields[5]) || 0 > parseInt(fields[4], 10)) {
		console.log("Move number must be an integer that is > 0.")
		return false;
	}
	let whiteKing = false
	let blackKing = false
	let kings = 0
	//positions are valid
	for (let i = 0; i < positions.length; i++) {
		//sum ranks and no 2 consecutive numbers
		let sumRank = 0
		let prev_num = false
		
		for (let j = 0; j < positions[i].length; j++) {
			if (!isNaN(positions[i][j])) {
				if (prev_num) {
					console.log("FEN positions may not have 2 consecutive numbers")
					return false
				}
				sumRank += parseInt(positions[i][j], 10)
				prev_num = true
			}
			else {
				if (!/^[prnbqkPRNBQK]$/.test(positions[i][j])) {
					console.log("FEN pieces must be represented using the letters prnbqk(or uppercase)")
					return false
				}
				if (positions[i][j] == 'k') {
					blackKing = true;
					kings++
				}
				if (positions[i][j] == 'K') {
					whiteKing = true;
					kings++
				}
				sumRank += 1
				prev_num = false
			}
			if (sumRank > 8 || sumRank < 0){
				console.log("FEN rank sums up to > 8 or < 0")
				return false
			}
		}
	}
	//must have kings
	if (!(whiteKing && blackKing && (kings == 2))) {
		console.log("FEN must contain exactly one white and one black king")
		return false
	}
	console.log("Valid fen!")
	return true;
}
// Clock
}


//  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
Position::Position(std::string fen) {
	std::string fields[6];
	int ls = 0, index = 0;
	for (unsigned int i = 0; i < fen.length() + 1; i++){
		if (fen[i] == ' ' || i == fen.length()){
			fields[index] = fen.substr(ls, i - ls);
			index++;
			ls = i + 1;
		} 
	}
    for (int i = 0; i < 6; i++){
        std::cout << fields[i] << fields[i].length() << std::endl;
    }
	turn = (fields[1] == "w") ? WHITE : BLACK;
	//castling
	if (fields[2].find('k') != std::string::npos) castling |= BLACK_KINGSIDE;
	if (fields[2].find('K') != std::string::npos) castling |= WHITE_KINGSIDE;
	if (fields[2].find('q') != std::string::npos) castling |= BLACK_QUEENSIDE;
	if (fields[2].find('Q') != std::string::npos) castling |= WHITE_QUEENSIDE;
	//en peasant xd
	if (fields[3] != "-")en_peasant = square(Rank(fields[3][1] - '1'), File(fields[3][0] - 'a'));
	else en_peasant = NO_SQUARE;
	fullMoves = std::stoi(fields[4]);
	halfMoves = std::stoi(fields[4]);

    //rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    Square s = A8;
	for (unsigned i = 0; i < fields[0].length(); i++) {
        std::cout << (int)s << " ";
        if (fields[0][i] == '/'){
            s += -16;
            continue;
        } 
        if (fields[0][i] > 65){
        Color color = (fields[0][i] > 97) ? BLACK : WHITE;
        PieceType type;
        switch (fields[0][i]){
            case 'b': case 'B':
                type = BISHOP;
                break;
            case 'k': case 'K':
                type = KING;
                break;
            case 'n': case 'N':
                type = KNIGHT;
                break;
            case 'p': case 'P':
                type = PAWN;
                break;
            case 'q': case 'Q':
                type = QUEEN;
                break;
            case 'r': case 'R':
                type = ROOK;
                break;
            default:
                std::cout << "nani";
                return;
        }
        board[s] = piece_init(type, color);
        Bitboard bitboard = 0 | BB_SQUARES[s];
        pieces[color][type] |= bitboard;
        s++;
        }
        else{
            for (int j = 0; j < fields[0][i] - '0'; j++){
                board[s + j] = NO_PIECE;
            }
			s += fields[0][i] - '0';
        }
    }	
    
	//set colors
	for (int i = 0; i < NUM_PIECE_TYPES; i++){
		colors[WHITE] |= pieces[WHITE][i];
		colors[BLACK] |= pieces[BLACK][i];
	}
	//set all
	all_pieces = colors[WHITE] | colors[BLACK];
}

Piece Position::piece_on(Square s) const {
	return board[s];
}

void Position::place_piece(Piece p, Square s) {
	board[s] = p;
	pieces[piece_color(p)][piece_type(p)] |= BB_SQUARES[s];
	colors[piece_color(p)] |= BB_SQUARES[s];
	all_pieces |= BB_SQUARES[s];
}

std::ostream& operator<<(std::ostream& os, const Position& p) {
	os << "\n +---+---+---+---+---+---+---+---+\n";
	for (Rank r = RANK_8; r >= RANK_1; r--) {
		for (File f = FILE_A; f <= FILE_H; f++)
			os << " | " << "PNBRQK  pnbrqk "[p.piece_on(square(r, f))];
		os << " | " << (1 + r) << "\n +---+---+---+---+---+---+---+---+\n";
	}
	os << "   a   b   c   d   e   f   g   h\n";
	return os;
}