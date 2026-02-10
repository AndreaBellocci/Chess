// Chicago, 7/04/2026
//	Completed on 8/04/2026
 
#include "../Types.h"
#include "../Pieces.h"
#include "../Chess.h"

void Queen_BuildPossibleMoves(Pieces queen_id, int queen_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks)
{
	Rook_BuildPossibleMoves	 (queen_id, queen_pos, the_game, my_king_id, out_moves, allow_checks);
	Bishop_BuildPossibleMoves(queen_id, queen_pos, the_game, my_king_id, out_moves, allow_checks);
} // Queen_BuildPossibleMoves


bool Queen_CanEatKingInSquare(int queen_pos, int enemy_king_pos, const board_t& board)
{
	return Rook_CanEatKingInSquare(queen_pos, enemy_king_pos, board) || 
		 Bishop_CanEatKingInSquare(queen_pos, enemy_king_pos, board);
} // Queen_CanEatKingInSquare


bool Queen_CanMove(int queen_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	return Rook_CanMove(queen_pos, the_game, my_king_id, my_king_pos, allow_checks) || 
		 Bishop_CanMove(queen_pos, the_game, my_king_id, my_king_pos, allow_checks);
} // Queen_CanMove