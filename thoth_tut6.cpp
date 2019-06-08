/*
Thoth Tutorial Chess - A Chess Engine created for teaching beginners the basics of chess programming.
Copyright (C) 2019 Pranav Deshpande
This file is a part of Thoth Tutorial Chess.

Thoth Tutorial Chess is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

Thoth Tutorial Chess is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Thoth Tutorial Chess. If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>

#define INVALID -1
#define BLANK 0
#define WHITE 1
#define BLACK 2

enum {
    BL = 0,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

enum {
    NO_CASTLE = 0, WHITE_QUEEN_SIDE_CASTLE, WHITE_KING_SIDE_CASTLE,
    BLACK_QUEEN_SIDE_CASTLE, BLACK_KING_SIDE_CASTLE
};

enum {
    NO_END_OF_GAME, CHECKMATE, STALEMATE, INSUFFICIENT_MATERIAL_DRAW, THREE_MOVE_DRAW, FIFTY_MOVE_DRAW
};

int init_pos[8][8] = {
        {BL, BL, BL, BL, BL, BL, BL, BL},
        {BL, BL, BL, BL, BL, BL, BL, BL},
        {BL, BL, BL, BL, BL, BL, BL, BL},
        {BL, BL, BL, BL, BL, BL, BL, BL},
        {BL, BL, BL, BL, BL, BL, BL, BL},
        {BQ, BK, BL, BL, BL, BL, BL, BL},
        {BL, BL, BL, BL, BL, BL, BL, BL},
        {BL, WK, BL, BL, BL, BL, BL, BL}
};

std::string enum_to_piece[13] = {"--", "WP", "WN", "WB", "WR", "WQ", "WK", "BP", "BN", "BB", "BR", "BQ", "BK"};

class move {
public:
    std::pair<int, int> init_pos;
    std::pair<int, int> final_pos;
    int captured_piece;
    int promoted_piece;
    int castle_code;
    bool do_enpassant;

    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos, int captured_piece, int promoted_piece) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = captured_piece;
        this->promoted_piece = promoted_piece;
        this->castle_code = NO_CASTLE;
        this->do_enpassant = false;
    }

    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos, int captured_piece) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = captured_piece;
        this->promoted_piece = BL;
        this->castle_code = NO_CASTLE;
        this->do_enpassant = false;
    }

    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = BL;
        this->promoted_piece = BL;
        this->castle_code = NO_CASTLE;
        this->do_enpassant = false;
    }

    move(int castle_code) {
        this->castle_code = castle_code;
        this->init_pos = {INVALID, INVALID};
        this->final_pos = {INVALID, INVALID};
        this->captured_piece = BL;
        this->promoted_piece = BL;
        this->do_enpassant = false;
    }

    move(bool do_enpassant, std::pair<int, int> init_pos, std::pair<int, int> final_pos, int captured_piece) {
        this->do_enpassant = true;
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = captured_piece;
        this->promoted_piece = BL;
        this->castle_code = NO_CASTLE;
    }
};

class chessboard {
private:
    int board[8][8];
    int side_to_play;

    bool whiteQcastle, prevWhiteQcastle;
    bool whiteKcastle, prevWhiteKcastle;
    bool blackQcastle, prevBlackQcastle;
    bool blackKcastle, prevBlackKcastle;

    bool is_en_passant_allowed, prev_is_en_passant_allowed;
    std::pair<int, int> en_passant_square, prev_en_passant_square;
    
    std::vector<int> fifty_move_history;

    int get_piece_side(int piece) {
        if(piece == BL) return BLANK;
        if(piece <= WK) return WHITE;
        return BLACK;
    }

    int opposite_side() {
        return side_to_play == WHITE ? BLACK : WHITE;
    }

    bool is_square_in_range(int row, int col) {
        return (row < 8 && row >= 0 && col < 8 && col >= 0);
    }

    bool is_slider(int piece) {
        return piece == WQ || piece == BQ || piece == WR || piece == BR || piece == WB || piece == BB;
    }

    bool is_diagonal_attacker(int piece) {
        return piece == WQ || piece == BQ || piece == WB || piece == BB;
    }

    bool is_straight_attacker(int piece) {
        return piece == WQ || piece == BQ || piece == WR || piece == BR;
    }
    
    bool is_knight(int piece) {
        return piece == WN || piece == BN;
    }
    
    bool is_pawn(int piece) {
        return piece == WP || piece == BP;
    }
    
    bool is_king(int piece) {
        return piece == WK || piece == BK;
    }
    
    bool is_bishop(int piece) {
        return piece == WB || piece == BB;
    }
    
    bool is_queen(int piece) {
        
    }
public:
    chessboard() {
        init();
    }

    void init() {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                board[i][j] = init_pos[i][j];
            }
        }
        whiteQcastle = whiteKcastle = false;
        blackQcastle = blackKcastle = false;
        is_en_passant_allowed = false;
        en_passant_square = {INVALID, INVALID};
        fifty_move_history.push_back(0);
        side_to_play = BLACK;
    }

    void print() {
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                std::cout << enum_to_piece[board[i][j]] << " ";
            }
            std::cout << std::endl;
        }

        std::cout << std::endl;
        std::cout << "Side to play: " << (side_to_play == WHITE ? "WHITE" : "BLACK") << "\n\n";
    }

    void make_move(move m) {
        int curr_piece = board[m.init_pos.first][m.init_pos.second];
        // Store the current castling permissions
        prevWhiteKcastle = whiteKcastle;
        prevWhiteQcastle = whiteQcastle;
        prevBlackKcastle = blackKcastle;
        prevBlackQcastle = blackQcastle;

        // Store the current en_passant permissions
        prev_en_passant_square = en_passant_square;
        prev_is_en_passant_allowed = is_en_passant_allowed;

        // Set the permissions for en_passant
        if((curr_piece == WP || curr_piece == BP) && abs(m.init_pos.first-m.final_pos.first) == 2) {
            is_en_passant_allowed = true;
            en_passant_square = {m.final_pos.first, m.final_pos.second};
        }

        // Set the permissions
        if(curr_piece == WK) {
            whiteKcastle = false;
            whiteQcastle = false;
        }
        if(curr_piece == BK) {
            blackKcastle = false;
            blackQcastle = false;
        }
        if(curr_piece == WR) {
            if(m.init_pos.second == 7) {
                whiteKcastle = false;
            } else if(m.init_pos.second == 0) {
                whiteQcastle = false;
            }
        }
        if(curr_piece == BR) {
            if(m.init_pos.second == 7) {
                blackKcastle = false;
            } else if(m.init_pos.second == 0) {
                blackQcastle = false;
            }
        }
        
        // Update the move history for the fifty move rule
        if(m.captured_piece != BLANK || is_pawn(curr_piece)) {
            fifty_move_history.push_back(0);
        } else {
            fifty_move_history.push_back(fifty_move_history[fifty_move_history.size()-1] + 1);
        }
        
        if(m.castle_code != NO_CASTLE) {
            switch(m.castle_code) {
                case WHITE_QUEEN_SIDE_CASTLE:
                    board[7][2] = WK;
                    board[7][3] = WR;
                    board[7][0] = BL;
                    board[7][4] = BL;
                    whiteQcastle = false;
                    break;
                case WHITE_KING_SIDE_CASTLE:
                    board[7][6] = WK;
                    board[7][5] = WR;
                    board[7][4] = BL;
                    board[7][7] = BL;
                    whiteKcastle = false;
                    break;
                case BLACK_QUEEN_SIDE_CASTLE:
                    board[0][2] = BK;
                    board[0][3] = BR;
                    board[0][0] = BL;
                    board[0][4] = BL;
                    blackQcastle = false;
                    break;
                case BLACK_KING_SIDE_CASTLE:
                    board[0][6] = BK;
                    board[0][5] = BR;
                    board[0][4] = BL;
                    board[0][7] = BL;
                    blackKcastle = false;
                    break;
            }
            side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
            return;
        }
        board[m.final_pos.first][m.final_pos.second] = board[m.init_pos.first][m.init_pos.second];
        board[m.init_pos.first][m.init_pos.second] = BLANK;
        if(m.do_enpassant) {
            board[en_passant_square.first][en_passant_square.second] = BL;
        }
        if(m.promoted_piece != BL) {
            board[m.final_pos.first][m.final_pos.second] = m.promoted_piece;
        }
        side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
    }

    void undo_move(move m) {
        // Unconditionally restore the castle permissions
        whiteQcastle = prevWhiteQcastle;
        whiteKcastle = prevWhiteKcastle;
        blackQcastle = prevBlackQcastle;
        blackKcastle = prevBlackKcastle;
        
        // Restore the en_passant permissions and square
        en_passant_square = prev_en_passant_square;
        is_en_passant_allowed = prev_is_en_passant_allowed;
        
        // Deal with the 50 moves rule history
        fifty_move_history.pop_back();
        
        if(m.castle_code != NO_CASTLE) {
            switch(m.castle_code) {
                case WHITE_QUEEN_SIDE_CASTLE:
                    board[7][2] = BL;
                    board[7][3] = BL;
                    board[7][0] = WR;
                    board[7][4] = WK;
                    break;
                case WHITE_KING_SIDE_CASTLE:
                    board[7][6] = BL;
                    board[7][5] = BL;
                    board[7][4] = WK;
                    board[7][7] = WR;
                    break;
                case BLACK_QUEEN_SIDE_CASTLE:
                    board[0][2] = BL;
                    board[0][3] = BL;
                    board[0][0] = BR;
                    board[0][4] = BK;
                    break;
                case BLACK_KING_SIDE_CASTLE:
                    board[0][6] = BL;
                    board[0][5] = BL;
                    board[0][4] = BK;
                    board[0][7] = BR;
                    break;
            }
            side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
            return;
        }

        board[m.init_pos.first][m.init_pos.second] = board[m.final_pos.first][m.final_pos.second];
        board[m.final_pos.first][m.final_pos.second] = m.captured_piece;
        if(m.do_enpassant) {
            board[m.final_pos.first][m.final_pos.second] = BL;
            board[prev_en_passant_square.first][prev_en_passant_square.second] = m.captured_piece;
        }
        if(m.promoted_piece != BL) {
            board[m.init_pos.first][m.init_pos.second] = side_to_play == WHITE ? BP : WP;
        }
        side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
    }

    bool is_square_attacked(std::pair<int, int> square, int side) {
        // Check if the square is attacked by any slider pieces

        int i = square.first;
        int j = square.second;
        int var_i, var_j, piece;

        // Check the diagonals for queen/bishop attacks
        var_i = i + 1;
        var_j = j + 1;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_diagonal_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_i++;
                var_j++;
            } else {
                break;
            }
        }

        var_i = i + 1;
        var_j = j - 1;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_diagonal_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_i++;
                var_j--;
            } else {
                break;
            }
        }

        var_i = i - 1;
        var_j = j - 1;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_diagonal_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_i--;
                var_j--;
            } else {
                break;
            }
        }

        var_i = i - 1;
        var_j = j + 1;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_diagonal_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_i--;
                var_j++;
            } else {
                break;
            }
        }

        // Check the diagonals for rook/queen attacks
        var_i = i + 1;
        var_j = j;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_straight_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_i++;
            } else {
                break;
            }
        }

        var_i = i;
        var_j = j + 1;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_straight_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_j++;
            } else {
                break;
            }
        }

        var_i = i - 1;
        var_j = j;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_straight_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_i--;
            } else {
                break;
            }
        }

        var_i = i;
        var_j = j - 1;
        while(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_straight_attacker(piece)) {
                return true;
            } else if(get_piece_side(piece) == BLANK) {
                var_j--;
            } else {
                break;
            }
        }

        // Check if the square is attacked by a night
        // The directions in which the knights can move
        std::pair<int, int> knight_moves[] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2},
                                              {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};

        for(auto next : knight_moves) {
            var_i = i + next.first;
            var_j = j + next.second;
            if(is_square_in_range(var_i, var_j)) {
                piece = board[var_i][var_j];
                if(get_piece_side(piece) == side && is_knight(piece)) {
                    return true;
                }
            }
        }
        
        // Let's check for pawn attacks
        int increment_sign = (side == WHITE ? 1 : -1);
        
        var_i = i + increment_sign;
        var_j = j + 1;
        if(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_pawn(piece)) {
                return true;
            }
        }

        var_i = i + increment_sign;
        var_j = j - 1;
        if(is_square_in_range(var_i, var_j)) {
            piece = board[var_i][var_j];
            if(get_piece_side(piece) == side && is_pawn(piece)) {
                return true;
            }
        }
        
        // Finally we check for king attacks
        // The directions in which a king can move
        std::pair<int, int> king_moves[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0},
                                        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

        for(auto next : king_moves) {
            var_i = i + next.first;
            var_j = j + next.second;
            if(is_square_in_range(var_i, var_j)) {
                piece = board[var_i][var_j];
                if(get_piece_side(piece) == side && is_king(piece)) {
                    return true;
                }
            }
        }
        
        // If the square is not attacked, return false
        return false;
    }
    
    auto get_filtered_moves(auto movelist) {
        std::pair<int, int> king_sq = {INVALID, INVALID};
        int king = INVALID;
        
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                if(is_king(board[i][j]) && get_piece_side(board[i][j]) == side_to_play) {
                    king_sq = {i, j};
                    king = board[i][j];
                }
            }
        }
        
        if(king == INVALID) {
            std::cout << "INVALID chess position, king not found on board\n\n";
            assert(false);
        }
        
        int attacking_side = opposite_side();
        
        std::vector<move> filtered_moves;
        for(move m: movelist) {
            make_move(m);
            if(board[m.final_pos.first][m.final_pos.second] == king) {
                king_sq = m.final_pos;
            }
            if(!is_square_attacked(king_sq, attacking_side)) {
                filtered_moves.push_back(m);
            }
            undo_move(m);
        }
        
        return filtered_moves;
        
    }
    
    auto generate_all_moves() {
        std::vector <move> movelist;

        // Castles
        if(side_to_play == WHITE && whiteQcastle == true
           && board[7][1] == BL && board[7][2] == BL
           && board[7][3] == BL && !is_square_attacked({7,2}, BLACK) && !is_square_attacked({7, 3}, BLACK)) {
            movelist.push_back(move(WHITE_QUEEN_SIDE_CASTLE));
        }
        if(side_to_play == WHITE && whiteKcastle == true
           && board[7][5] == BL && board[7][6] == BL && !is_square_attacked({7, 5}, BLACK) && !is_square_attacked({7, 6}, BLACK)) {
            movelist.push_back(move(WHITE_KING_SIDE_CASTLE));
        }
        if(side_to_play == BLACK && blackQcastle == true
           && board[0][1] == BL && board[0][2] == BL
           && board[0][3] == BL && !is_square_attacked({0, 2}, WHITE) && !is_square_attacked({0, 3}, WHITE)) {
            movelist.push_back(move(BLACK_QUEEN_SIDE_CASTLE));
        }
        if(side_to_play == BLACK && blackKcastle == true
           && board[0][5] == BL && board[0][6] == BL  && !is_square_attacked({0, 5}, WHITE) && !is_square_attacked({0, 6}, WHITE)) {
            movelist.push_back(move(BLACK_KING_SIDE_CASTLE));
        }

        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                int curr_piece = board[i][j];

                // The square where the piece will move
                int var_i;
                int var_j;
                int next_piece;

                if(get_piece_side(curr_piece) == side_to_play) {
                    // Now slide the piece in all directions

                    // Lots of diagonal moves now!! -> in all 4 directions -> Bishops and Queens
                    if(is_slider(curr_piece) && curr_piece != WR && curr_piece != BR) {
                        var_i = i + 1;
                        var_j = j + 1;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_i++;
                                var_j++;
                            } else {
                                break;
                            }
                        }

                        var_i = i - 1;
                        var_j = j - 1;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_i--;
                                var_j--;
                            } else {
                                break;
                            }
                        }
                        var_i = i + 1;
                        var_j = j - 1;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_i++;
                                var_j--;
                            } else {
                                break;
                            }
                        }

                        var_i = i - 1;
                        var_j = j + 1;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_i--;
                                var_j++;
                            } else {
                                break;
                            }
                        }
                    }

                    // Straight moves in all 4 directions -> Rooks and Queens
                    if(is_slider(curr_piece) && curr_piece != WB && curr_piece != BB) {
                        var_i = i;
                        var_j = j + 1;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_j++;
                            } else {
                                break;
                            }
                        }

                        var_i = i;
                        var_j = j - 1;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_j--;
                            } else {
                                break;
                            }
                        }
                        var_i = i + 1;
                        var_j = j;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_i++;
                            } else {
                                break;
                            }
                        }

                        var_i = i - 1;
                        var_j = j;
                        while(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                break;
                            } else if(get_piece_side(next_piece) == BLANK) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                                var_i--;
                            } else {
                                break;
                            }
                        }
                    }

                    if(curr_piece == WN || curr_piece == BN) {
                        // The directions in which the knights can move
                        std::pair<int, int> knight_moves[] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2},
                                                              {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};

                        // Generate all knight moves -> capture an opponent piece or just move somewhere.
                        for(auto next : knight_moves) {
                            var_i = i + next.first;
                            var_j = j + next.second;
                            if(is_square_in_range(var_i, var_j)) {
                                next_piece = board[var_i][var_j];
                                if(next_piece == BL) {
                                    movelist.push_back(move({i, j}, {var_i, var_j}));
                                } else if(get_piece_side(next_piece) == opposite_side()) {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                }
                            }
                        }
                    }

                    if(curr_piece == WP || curr_piece == BP) {
                        int increment_sign = (side_to_play == WHITE ? -1 : 1);

                        // Move the pawns 1 square ahead
                        // Note that we have to take care of pawn promotion as well
                        var_i = i + increment_sign;
                        var_j = j;
                        if(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(next_piece == BL) {
                                if(var_i != 7 && var_i != 0 ){
                                    movelist.push_back(move({i, j}, {var_i, var_j}));
                                } else {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, BL, side_to_play == WHITE ? WQ : BQ));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, BL, side_to_play == WHITE ? WR : BR));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, BL, side_to_play == WHITE ? WB : BB));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, BL, side_to_play == WHITE ? WN : BN));
                                }
                            }
                        }

                        // Move the pawns 2 squares ahead if the conditions are satisfied
                        var_i = i + increment_sign * 2;
                        var_j = j;
                        if((i == 6 && curr_piece == WP) || (i == 1 && curr_piece == BP)) {
                            next_piece = board[var_i][var_j];
                            if(next_piece == BL) {
                                movelist.push_back(move({i, j}, {var_i, var_j}));
                            }
                        }

                        // Diagonal captures
                        // Note that we have to take care of pawn promotions as well
                        var_i = i + increment_sign;
                        var_j = j + 1;
                        if(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                if(var_i != 7 && var_i != 0) {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                } else {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WQ : BQ));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WR : BR));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WB : BB));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WN : BN));
                                }
                            }
                        }

                        var_i = i + increment_sign;
                        var_j = j - 1;
                        if(is_square_in_range(var_i, var_j)) {
                            next_piece = board[var_i][var_j];
                            if(get_piece_side(next_piece) == opposite_side()) {
                                if(var_i != 7 && var_i != 0) {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                } else {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WQ : BQ));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WR : BR));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WB : BB));
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece, side_to_play == WHITE ? WN : BN));
                                }
                            }
                        }

                        // Enpassant
                        if(is_square_in_range(var_i, var_j)) {
                            var_i = i + increment_sign;
                            var_j = j + 1;
                            if(is_en_passant_allowed && en_passant_square.first == i && en_passant_square.second == var_j) {
                                movelist.push_back(move(true, {i, j}, {var_i, var_j}, board[i][var_j]));
                            }

                            var_i = i + increment_sign;
                            var_j = j - 1;
                            if(is_en_passant_allowed && en_passant_square.first == i && en_passant_square.second == var_j) {
                                movelist.push_back(move(true, {i, j}, {var_i, var_j}, board[i][var_j]));
                            }
                        }

                    }

                    if(curr_piece == WK || curr_piece == BK) {
                        // The directions in which a king can move
                        std::pair<int, int> king_moves[] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0},
                                                            {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

                        // Generate all king moves -> capture an opponent piece or just move somewhere.
                        for(auto next : king_moves) {
                            var_i = i + next.first;
                            var_j = j + next.second;
                            if(is_square_in_range(var_i, var_j)) {
                                next_piece = board[var_i][var_j];
                                if(next_piece == BL) {
                                    movelist.push_back(move({i, j}, {var_i, var_j}));
                                } else if(get_piece_side(next_piece) == opposite_side()) {
                                    movelist.push_back(move({i, j}, {var_i, var_j}, next_piece));
                                }
                            }
                        }

                    }

                }
            }
        }

        return get_filtered_moves(movelist);
    }
    
    int is_draw_by_insufficient_material() {
        // Draw by insufficient material:
        int knight_count = 0;
        int bishop_count = 0, bishops_on_white = 0, bishops_on_black = 0;
        int pawn_count = 0;
        int queen_rook_count = 0;
        
        static int const colours[][8] = {
            {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
            {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
            {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
            {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
            {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
            {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK},
            {BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE},
            {WHITE, BLACK, WHITE, BLACK, WHITE, BLACK, WHITE, BLACK}
        };

        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                int piece = board[i][j];
                if(is_knight(piece)) {
                    knight_count++;
                } else if(is_bishop(piece)) {
                    bishop_count++;
                    if(colours[i][j] == BLACK) {
                        bishops_on_black++;
                    } else {
                        bishops_on_white++;
                    }
                } else if(is_pawn(piece)) {
                    pawn_count++;
                } else if(is_straight_attacker(piece)) {
                    queen_rook_count++;
                }
            }
        }
        
        if(pawn_count + queen_rook_count > 0) {
            return NO_END_OF_GAME;
        }

        if(knight_count == 1 && bishop_count == 0) {
            return INSUFFICIENT_MATERIAL_DRAW;
        }
        
        if(knight_count == 0) {
           if(bishops_on_white == 0 || bishops_on_black == 0) {
               return INSUFFICIENT_MATERIAL_DRAW;
           }
        }
        
        // Default
        return NO_END_OF_GAME;
    }
    
    int is_end_of_game() {
        std::pair<int, int> king_sq = {INVALID, INVALID};
        int king = INVALID;
        
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                if(is_king(board[i][j]) && get_piece_side(board[i][j]) == side_to_play) {
                    king_sq = {i, j};
                    king = board[i][j];
                }
            }
        }
        
        if(king == INVALID) {
            std::cout << "INVALID chess position, king not found on board\n\n";
            assert(false);
        } 
        
        bool is_king_in_check = is_square_attacked(king_sq, opposite_side());
        std::vector<move> movelist = generate_all_moves();
        
        // See if it's a CHECKMATE or a STALEMATE    
        if(is_king_in_check && movelist.empty()){
            return CHECKMATE;
        } else if(movelist.empty()) {
            return STALEMATE;
        }
        
        // Is it a draw due to insufficient material?
        int draw_reason = is_draw_by_insufficient_material();
        
        if(draw_reason == INSUFFICIENT_MATERIAL_DRAW) {
            return INSUFFICIENT_MATERIAL_DRAW;
        }
        
        // 50 MOVE Rule
        if(fifty_move_history[fifty_move_history.size()-1] >= 100) {
            return FIFTY_MOVE_DRAW;
        }
        
        // default
        return NO_END_OF_GAME;
    }
};

int main() {
    chessboard board;

    board.init();
    board.print();

    std::vector<move> movelist = board.generate_all_moves();
    for(auto m : movelist) {
        board.make_move(m);
        board.print();
        int game_end_status = board.is_end_of_game();
        if(game_end_status != NO_END_OF_GAME) {
            std::cout << "Game has ended.\n\n";
            std::cout << game_end_status << std::endl;
            break;
        }
        board.undo_move(m);
        board.print();
    }
}