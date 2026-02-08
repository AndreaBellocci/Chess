// Chicago, 7/04/2026
//	Completed on 8/04/2026

#include "../Chess.h"
#include "../Pieces.h"
#include "../piece_utilities.h"

#include <cmath>

void Bishop_BuildPossibleMoves(Pieces bishop_id, int bishop_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks)
{
	const int row = bishop_pos / BOARD_SIDE;
	const int col = bishop_pos % BOARD_SIDE;
	const auto& my_king_pos = the_game.m_Pieces[static_cast<int>(my_king_id)]->GetPiecePos();
	int i, j;

	// First diagonal - down 
	for (i = row + 1, j = col + 1; i < BOARD_SIDE && j < BOARD_SIDE; ++i, ++j)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = i * BOARD_SIDE + j;
		if (MoveHelper(dest, bishop_id, bishop_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}

	// First diagonal - up 
	for (i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = i * BOARD_SIDE + j;
		if (MoveHelper(dest, bishop_id, bishop_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}

	// Second diagonal - down
	for (i = row + 1, j = col - 1; i < BOARD_SIDE && j >= 0; ++i, --j)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = i * BOARD_SIDE + j;
		if (MoveHelper(dest, bishop_id, bishop_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}

	// Second diagonal - up 
	for (i = row - 1, j = col + 1; i >= 0 && j < BOARD_SIDE; --i, ++j)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = i * BOARD_SIDE + j;
		if (MoveHelper(dest, bishop_id, bishop_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}
} // Bishop_BuildPossibleMoves


bool Bishop_CanEatKingInSquare(int bishop_pos, int enemy_king_pos, const board_t& board)
{
	const long long row = bishop_pos / BOARD_SIDE;
	const long long col = bishop_pos % BOARD_SIDE;

	const long long king_row = enemy_king_pos / BOARD_SIDE;
	const long long king_col = enemy_king_pos % BOARD_SIDE;

	// Bishops can only move diagonally if no obstacle is found
	const int diff_row = king_row - row;
	const int diff_col = king_col - col;
	if (std::abs(diff_row) != std::abs(diff_col)) // Not in the same diagonal
		return false;

	// First diagonal
	if (diff_row == diff_col)
	{
		int i = INVALID;
		if (row < king_row) // Go down, diff_row is positive
		{
			i = 1;
			while (i < diff_row && board[(row + i) * BOARD_SIDE + (col + i)] == nullptr)
				++i;
		}
		else // Go up, diff_row is negative
		{
			i = -1;
			while (i > diff_row && board[(row + i) * BOARD_SIDE + (col + i)] == nullptr)
				--i;
		}

		if (i == diff_row) // King was reached
			return true;
	}
	// Second diagonal (i.e. diff_row == -diff_col)
	else
	{
		int i = INVALID;
		if (row < king_row) // Go down, diff_row is positive
		{
			i = 1;
			while (i < diff_row && board[(row + i) * BOARD_SIDE + (col - i)] == nullptr)
				++i;
		}
		else // Go up, diff_row is negative
		{
			i = -1;
			while (i > diff_row && board[(row + i) * BOARD_SIDE + (col - i)] == nullptr)
				--i;
		}

		if (i == diff_row) // King was reached
			return true;
	}
	
	return false;
} // Bishop_CanEatKingInSquare


bool Bishop_CanMove(int bishop_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	const size_t max = the_game.m_Board.size();
	const int dest1 = bishop_pos - BOARD_SIDE - 1; // Move by one up the first diagonal
	const int dest2 = bishop_pos - BOARD_SIDE + 1; // Move by one up the second diagonal
	const int dest3 = bishop_pos + BOARD_SIDE - 1; // Move by one down the second diagonal
	const int dest4 = bishop_pos + BOARD_SIDE + 1; // Move by one down the first diagonal
	return
		(0 <= dest1 && dest1 < max && CanPieceMoveTo(dest1, bishop_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest2 && dest2 < max && CanPieceMoveTo(dest2, bishop_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest3 && dest3 < max && CanPieceMoveTo(dest3, bishop_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest4 && dest4 < max && CanPieceMoveTo(dest4, bishop_pos, the_game, my_king_id, my_king_pos, allow_checks));
} // Bishop_CanMove