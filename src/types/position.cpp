#include "position.h"
#include "score.h"
#include <iostream>
#include <string>
#include <regex>

bool validateFEN(std::string fen) {
	std::string fields[6];
	int ls = 0, index = 0;
	for (unsigned int i = 0; i < fen.length() + 1; i++){
		if (fen[i] == ' ' || i == fen.length()){
			fields[index] = fen.substr(ls, i - ls);
			index++;
			ls = i + 1;
		}
	}
    //total fields
    if (index != 6){
        std::cout << "FEN must have 6 fields separated by a single space."<< std::endl;
        return false;
    }
    //positions field
    int bs = 0;
    for (char c : fields[0]){
        if (c == '/') bs++;
    }
    if (bs != 7){
        std::cout << "Positions field must have 8 fields seperated by a /. Found: "<< bs <<  std::endl;
        return false;
    }
    // move field
    std::regex wb("^(w|b)$");
	if (!std::regex_match(fields[1], wb)) {
        std::cout << "Move field must be either 'w' or 'b'."<< std::endl;
        return false;
    }
    // castle rights field
    std::regex castle("^(KQ?k?q?|Qk?q?|kq?|q|-)$");
    if (!std::regex_match(fields[2], castle)) {
        std::cout << "Castling field must be either - or any of K Q q k in that specific order."<< std::endl;
        return false;
    }
    // en peasant field
    std::regex peasant("^(-|[abcdefgh][36])$");
    if (!std::regex_match(fields[3], peasant)) {
        std::cout << "En pessant field must be either - or a coordinate."<< std::endl;
        return false;
    }
    // en peasant matches current turn
	if ((fields[3][1] == '3' && fields[1] == "w") || (fields[3][1] == '6' && fields[1] == "b")) {
		std::cout << "En pessant field does not match the current turn"<< std::endl;
		return false;
	}
    // halfmove field
    for (char& c : fields[4]) {
        if (std::isdigit(c) == 0) {
            std::cout << "halfmove field may not contain characters."<< std::endl;
		    return false;
        }
    }
    if (stoi(fields[4]) < 0){
        std::cout << "halfmove field must be positive."<< std::endl;
		return false;
    }
    // fullmove field
	for (char& c : fields[5]){
        if (std::isdigit(c) == 0){
            std::cout << "fullmove field may not contain characters."<< std::endl;
		    return false;
        }
    }
    if (stoi(fields[5]) < 0){
        std::cout << "fullmove field must be positive"<< std::endl;
		return false;
    }
    bool white = false, black = false, num = false;
	int k = 0, sum = 0;
    std::regex pieces("^[prnbqkPRNBQK]$");
    for (char& c : fields[0]){
        if (c == '/' ){
            if (sum != 8){
                std::cout << "Positions field: the sum of a rank must be 8 squares."<< std::endl;
                return false;
            }
            sum = 0;
            num = false;
        }
        else if (std::isdigit(c) != 0){
            if (num){
                std::cout << "FEN positions may not have 2 consecutive numbers"<< std::endl;
                return false;
            }
            sum += c - '0';
            num = true;
        }
        else{
            std::string s(1,c);
            if (!std::regex_match(s,pieces)){
                std::cout << "FEN pieces must be represented using the letters prnbqkPRNBQK. Found: "<< s << std::endl;
                return false;
            }
            if (c == 'K'){
                white = true;
                k++;
            }
            if (c == 'k'){
                black = true;
                k++;
            }
            sum++;
            num = false;
        }
    }
    if (sum != 8){
        std::cout << "Positions field: the sum of a rank must be 8 squares.(final)"<< std::endl;
        return false;
    }
	if (!(white && black && (k == 2))) {
		std::cout << "FEN must contain exactly one white and one black king" << std::endl;
		return false;
	}
	//std::cout << "Valid FEN."<< std::endl;
	return true;
}

//  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
Position::Position(PositionInfo* info, std::string fen) {
    state = info;
    if (!validateFEN(fen)) {
        std::cout << "FEN was not valid, returning." << std::endl;
        return;
    }
	std::string fields[6];
	int ls = 0, index = 0;
	for (unsigned int i = 0; i < fen.length() + 1; i++){
		if (fen[i] == ' ' || i == fen.length()){
			fields[index] = fen.substr(ls, i - ls);
			index++;
			ls = i + 1;
		} 
	}
	turn = (fields[1] == "w") ? WHITE : BLACK;
	//castling

    for (Square s = A1; s <= H8; s++) {
        castlingMask[s] = 0;
    }

	if (fields[2].find('k') != std::string::npos) {
        state->castling |= BLACK_KINGSIDE;
        castlingMask[E8] |= BLACK_KINGSIDE;
        castlingMask[H8] |= BLACK_KINGSIDE;
    }
	if (fields[2].find('K') != std::string::npos) {
        state->castling |= WHITE_KINGSIDE;
        castlingMask[E1] |= WHITE_KINGSIDE;
        castlingMask[H1] |= WHITE_KINGSIDE;
    }
	if (fields[2].find('q') != std::string::npos) {
        state->castling |= BLACK_QUEENSIDE;
        castlingMask[E8] |= BLACK_QUEENSIDE;
        castlingMask[A8] |= BLACK_QUEENSIDE;
    }
	if (fields[2].find('Q') != std::string::npos) {
        state->castling |= WHITE_QUEENSIDE;
        castlingMask[E1] |= WHITE_QUEENSIDE;
        castlingMask[A1] |= WHITE_QUEENSIDE;
    }

	//en peasant xd
	if (fields[3] != "-")
        state->en_peasant = square(Rank(fields[3][1] - '1'), File(fields[3][0] - 'a'));
	else 
        state->en_peasant = NO_SQUARE;
	fullMoves = std::stoi(fields[4]);
	halfMoves = std::stoi(fields[4]);

    //rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
    Square s = A8;
	for (unsigned i = 0; i < fields[0].length(); i++) {
        if (fields[0][i] == '/') {
            s += -16;
            continue;
        } 

        if (fields[0][i] > 65) {
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
    score_init();
	info_init();
    key_init();
    // phase = calculate_phase();
}

Piece Position::piece_on(Square s) const {
	return board[s];
}

Square Position::square_of(PieceType p, Color c) const {
	return lsb(pieces[c][p]);
}

std::ostream& operator<<(std::ostream& os, const Position& p) {
	os << "\n +---+---+---+---+---+---+---+---+\n";
	for (Rank r = RANK_8; r >= RANK_1; r--) {
		for (File f = FILE_A; f <= FILE_H; f++)
			os << " | " << "PNBRQK  pnbrqk  "[p.piece_on(square(r, f))];
		os << " | " << (1 + r) << "\n +---+---+---+---+---+---+---+---+\n";
	}
	os << "   a   b   c   d   e   f   g   h\n";

	return os;
}