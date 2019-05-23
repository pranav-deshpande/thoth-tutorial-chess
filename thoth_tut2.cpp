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

using namespace std;

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

std::string enum_to_piece[13] = {"--", "WP", "WN", "WB", "WR", "WQ", "WK", "BP", "BN", "BB", "BR", "BQ", "BK"};

class chessboard {
private:
    int board[8][8];
    int side_to_play;
    
    int get_piece_side(int piece) {
        if(piece == BL) return BLANK;
        if(piece <= WK) return WHITE;
        return BLACK;
    }
    
    bool is_square_in_range(int row, int col) {
        return (row < 8 && row >= 0 && col < 8 && col >= 0);
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
    
    void generate_all_moves() {
        std::vector <std::pair<std::pair<int, int>, std::pair<int, int> > > movelist;
        
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                int curr_piece = board[i][j];
                if(get_piece_side(curr_piece) == side_to_play) {
                    for(int var = 0; var < 7; var++) {
                        int new_i = i + var;
                        int new_j = j + var;
                        int new_piece = board[new_i][new_j];
                        if(get_piece_side(new_piece) != side_to_play) {
                            movelist.push_back({{i, j}, {new_i, new_j}});
                        }
                    }
                }
            }
        }
    }
};

int main() {
	chessboard board;
	
	board.init();
	board.print();
}