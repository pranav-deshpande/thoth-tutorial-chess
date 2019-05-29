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

#define BLANK 0
#define WHITE 1
#define BLACK 2

enum {
    BL = 0,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};


int init_pos[8][8] = { 
    {BL, BL, BL, BL, BQ, BL, BL, BL},
    {BL, BL, BL, WP, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL}
};

std::string enum_to_piece[13] = {"--", "WP", "WN", "WB", "WR", "WQ", "WK", "BP", "BN", "BB", "BR", "BQ", "BK"};

class move {
public:
    std::pair<int, int> init_pos;
    std::pair<int, int> final_pos;
    int captured_piece;
    int promoted_piece;
    
     move(std::pair<int, int> init_pos, std::pair<int, int> final_pos, int captured_piece, int promoted_piece) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = captured_piece;
        this->promoted_piece = promoted_piece;
    }
    
    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos, int captured_piece) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = captured_piece;
        this->promoted_piece = BL;
    }
    
    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = BL;
        this->promoted_piece = BL;
    }
};

class chessboard {
private:
    int board[8][8];
    int side_to_play;
    
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
        side_to_play = WHITE;
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
    
    auto generate_all_moves() {
        std::vector <move> movelist;
        
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
        
        return movelist;
    }
    
    void make_move(move m) {
        board[m.final_pos.first][m.final_pos.second] = board[m.init_pos.first][m.init_pos.second];          
        board[m.init_pos.first][m.init_pos.second] = BLANK;
        if(m.promoted_piece != BL) {
            board[m.final_pos.first][m.final_pos.second] = m.promoted_piece;
        }
        side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
    }
    
    void undo_move(move m) {
        board[m.init_pos.first][m.init_pos.second] = board[m.final_pos.first][m.final_pos.second];
        board[m.final_pos.first][m.final_pos.second] = m.captured_piece;
        if(m.promoted_piece != BL) {
            board[m.init_pos.first][m.init_pos.second] = side_to_play == WHITE ? BP : WP;
        }
        side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
    }
};

int main() {
    chessboard board;
    
    board.init();
    board.print();
    std::vector<move> movelist = board.generate_all_moves();
    std::cout << "Total possible moves: " << movelist.size() << "\n\n";
    
    for(auto m : movelist) {
        board.make_move(m);
        board.print();
        board.undo_move(m);
    }

    board.print();
}