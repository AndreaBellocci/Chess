// Chicago, 7/04/2026
//	Completed on 8/04/2026

#include "../Chess.h"
#include "../Pieces.h"
#include "../piece_utilities.h"

#include <cmath>

constexpr static void RotateBy90(int vec[])
{
	// Multiply by rotation matrix, which according to linear algebra is 
	// | 0 -1 |
	// | 1	0 |
	// Given the rule for matrix multiplication, the product simplifies to:
	int hold = vec[0];
	vec[0] = vec[1] * (-1);
	vec[1] = hold;
} // RotateBy90


void Knight_BuildPossibleMoves(Pieces knight_id, int knight_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks)
{
	const int row = knight_pos / BOARD_SIDE;
	const int col = knight_pos % BOARD_SIDE;
	const auto& my_king_pos = the_game.m_Pieces[static_cast<int>(my_king_id)]->GetPiecePos();

	// Eight possible positions, but only two deltas, which are rotated by 90 degrees
	int delta[2] = { 1,  2 };
	int dest1, dest2;

	// Check all 8 possible moves (4 rotations of 2 deltas)
	for (int i = 0; i < 4; ++i)
	{
		// One horizontal and two vertical
		dest1 = row + delta[1];
		dest2 = col + delta[0];

		// Skip out-of board moves
		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			const int dest = dest1 * BOARD_SIDE + dest2;
			MoveHelper(dest, knight_id, knight_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks); // Return value is not needed
		}

		// Two horizontal and one vertical
		dest1 = row + delta[0];
		dest2 = col + delta[1];

		// Skip out-of board moves
		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			const int dest = dest1 * BOARD_SIDE + dest2;
			MoveHelper(dest, knight_id, knight_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks); // Return value is not needed
		}

		// Rotate by 90 degrees
		RotateBy90(delta);
	}
} // Knight_BuildPossibleMoves


bool Knight_CanEatKingInSquare(int knight_pos, int enemy_king_pos, const board_t& board)
{
	const int row = knight_pos / BOARD_SIDE;
	const int col = knight_pos % BOARD_SIDE;

	const int king_row = enemy_king_pos / BOARD_SIDE;
	const int king_col = enemy_king_pos % BOARD_SIDE;

	const int row_diff = abs(row - king_row);
	const int col_diff = abs(col - king_col);
	return ((row_diff == 1 && col_diff == 2) || (row_diff == 2 && col_diff == 1));
} // Knight_CanEatKingInSquare


bool Knight_CanMove(int knight_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	const int row = knight_pos / BOARD_SIDE;
	const int col = knight_pos % BOARD_SIDE;

	// Eight possible positions
	int delta[2] = { 1,  2 };
	int dest1, dest2;

	for (int i = 0; i < 4; ++i)
	{
		// One horizontal and two vertical
		dest1 = row + delta[1];
		dest2 = col + delta[0];

		// Skip out-of board moves
		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			const int dest = dest1 * BOARD_SIDE + dest2;
			if (CanPieceMoveTo(dest, knight_pos, the_game, my_king_id, my_king_pos, allow_checks))
				return true;
		}

		// Two horizontal and one vertical
		dest1 = row + delta[0];
		dest2 = col + delta[1];

		// Skip out-of board moves
		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			const int dest = dest1 * BOARD_SIDE + dest2;
			if (CanPieceMoveTo(dest, knight_pos, the_game, my_king_id, my_king_pos, allow_checks))
				return true;
		}

		// Rotate by 90 degrees
		RotateBy90(delta);
	}

	return false;
} // Knight_CanMove