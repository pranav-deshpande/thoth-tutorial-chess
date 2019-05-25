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
    {BR, BN, BB, BQ, BK, BB, BN, BR},
    {BP, BP, BP, BP, BP, BP, BP, BP},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {WP, WP, WP, WP, WP, WP, WP, WP},
    {WR, WN, WB, WQ, WK, WB, WN, WR}
};

int modded_init_pos[8][8] = { 
    {BR, BN, BB, BQ, BK, BB, BN, BR},
    {BP, BP, BP, BP, BP, BP, BP, BP},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {BL, BL, BL, BL, BL, BL, BL, BL},
    {WP, WP, WP, BL, WP, WP, WP, WP},
    {WR, WN, WB, WQ, WK, WB, WN, WR}
};

std::string enum_to_piece[13] = {"--", "WP", "WN", "WB", "WR", "WQ", "WK", "BP", "BN", "BB", "BR", "BQ", "BK"};

class move {
public:
    std::pair<int, int> init_pos;
    std::pair<int, int> final_pos;
    int captured_piece;
    
    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos, int captured_piece) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = captured_piece;
    }
    
    move(std::pair<int, int> init_pos, std::pair<int, int> final_pos) {
        this->init_pos = init_pos;
        this->final_pos = final_pos;
        this->captured_piece = BL;
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
                board[i][j] = modded_init_pos[i][j];
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
                if(get_piece_side(curr_piece) == side_to_play) {
                    // Now slide the piece in all directions
                    int var_i;
                    int var_j;
                    int next_piece;

                    // Lots of diagonal moves now!! -> in all 4 directions
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
                    
                    // Straight moves in all 4 directions
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
                }
            }
        }
        
        return movelist;
    }
    
    void make_move(move m) {
        board[m.final_pos.first][m.final_pos.second] = board[m.init_pos.first][m.init_pos.second];          
        board[m.init_pos.first][m.init_pos.second] = BLANK;
        side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
    }
    
    void undo_move(move m) {
        board[m.init_pos.first][m.init_pos.second] = board[m.final_pos.first][m.final_pos.second];
        board[m.final_pos.first][m.final_pos.second] = m.captured_piece;
        side_to_play = (side_to_play == WHITE ? BLACK : WHITE);
    }
};

int main() {
    chessboard board;
    
    board.init();
    board.print();
    std::vector<move> movelist = board.generate_all_moves();
    std::cout << "Total possible moves: " << movelist.size() << std::endl;
    
    for(auto m : movelist) {
        board.make_move(m);
        board.print();
        board.undo_move(m);
    }

    board.print();
}
