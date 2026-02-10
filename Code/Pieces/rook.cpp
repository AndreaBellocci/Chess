// Chicago, 7/04/2026
//	Completed on 8/04/2026

#include "../Types.h"
#include "../Pieces.h"
#include "../Chess.h"
#include "piece_utilities.h"

void Rook_BuildPossibleMoves(Pieces rook_id, int rook_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks)
{
	const int row = rook_pos / BOARD_SIDE;
	const int col = rook_pos % BOARD_SIDE;
	const auto& my_king_pos = the_game.m_Pieces[static_cast<int>(my_king_id)]->GetPiecePos();

	// Horizontal backward
	for (int j = col - 1; j >= 0; --j)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = row * BOARD_SIDE + j;
		if (MoveHelper(dest, rook_id, rook_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}

	// Horizontal forward
	for (int j = col + 1; j < BOARD_SIDE; ++j)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = row * BOARD_SIDE + j;
		if (MoveHelper(dest, rook_id, rook_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}

	// Vertical up
	for (int i = row - 1; i >= 0; --i)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = i * BOARD_SIDE + col;
		if (MoveHelper(dest, rook_id, rook_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}

	// Vertical down
	for (int i = row + 1; i < BOARD_SIDE; ++i)
	{
		// Move helper will return true only if we should stop searching in this direction
		const int dest = i * BOARD_SIDE + col;
		if (MoveHelper(dest, rook_id, rook_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks))
			break;
	}
} // Rook_BuildPossibleMoves


bool Rook_CanEatKingInSquare(int rook_pos, int enemy_king_pos, const board_t& board)
{
	const int row = rook_pos / BOARD_SIDE;
	const int col = rook_pos % BOARD_SIDE;

	const int king_row = enemy_king_pos / BOARD_SIDE;
	const int king_col = enemy_king_pos % BOARD_SIDE;

	// Rooks can only move by rows or columns if no obstacle is found

	// Same col
	if (col == king_col)
	{
		int i = INVALID;
		if (row < king_row) // Go down
		{
			// Loop until we either reach the king or find an obstacle in between
			i = row + 1;
			while (i < king_row && board[i * BOARD_SIDE + col] == nullptr)
				++i;
		}
		else // Go up - can't be in the same cell as the king
		{
			// Loop until we either reach the king or find an obstacle in between
			i = row - 1;
			while (i > king_row && board[i * BOARD_SIDE + col] == nullptr)
				--i;
		}

		if (i == king_row) // King was reached
			return true;
	}

	// Same row
	else if (row == king_row)
	{
		int j = INVALID;
		if (col < king_col) // Go right
		{
			// Loop until we either reach the king or find an obstacle in between
			j = col + 1;
			while (j < king_col && board[row * BOARD_SIDE + j] == nullptr)
				++j;
		}
		else // Go left - can't be in the same cell as the king
		{
			// Loop until we either reach the king or find an obstacle in between
			j = col - 1;
			while (j > king_col && board[row * BOARD_SIDE + j] == nullptr)
				++j;
		}

		if (j == king_col) // King was reached
			return true;
	}

	return false;
} // Rook_CanEatKingInSquare


bool Rook_CanMove(int rook_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	const auto max = the_game.m_Board.size();
	const int dest1 = rook_pos - 1; // Move left
	const int dest2 = rook_pos + 1; // Move right
	const int dest3 = rook_pos - BOARD_SIDE; // Move up
	const int dest4 = rook_pos + BOARD_SIDE; // Move down
	return
		(0 <= dest1 && dest1 < max && CanPieceMoveTo(dest1, rook_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest2 && dest2 < max && CanPieceMoveTo(dest2, rook_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest3 && dest3 < max && CanPieceMoveTo(dest3, rook_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest4 && dest4 < max && CanPieceMoveTo(dest4, rook_pos, the_game, my_king_id, my_king_pos, allow_checks));
} // Rook_CanMove