#pragma once
#include "Chess.h"

bool isEnemy(Pieces other_id, Pieces my_piece_id);

bool IsKingUnderCheck(const piece_list& pieces, const board_t& board, Pieces king_id, int king_pos);
bool CanCastle(const Chess& the_game, Pieces king_id, int king_src, int king_dest, Pieces rook_id, int rook_dest);
bool CanPieceMoveTo(int dest, int src, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);
bool MoveHelper(int dest, Pieces piece_id, int src, const Chess& the_game, Pieces my_king_id, int my_king_pos, PossibleMoves& out_moves, bool allow_checks);