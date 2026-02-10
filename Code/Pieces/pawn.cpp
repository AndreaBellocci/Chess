// Chicago, 8/04/2026
//	Completed on 8/04/2026

#include "../Types.h"
#include "../Pieces.h"
#include "../Chess.h"
#include "../ErrorCodes.h"
#include "piece_utilities.h"

void Pawn_BuildPossibleMoves(Pieces pawn_id, int pawn_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks)
{
	const int row = pawn_pos / BOARD_SIDE;
	const int col = pawn_pos % BOARD_SIDE;
	const size_t max = the_game.m_Board.size();
	const auto& my_king_pos = the_game.m_Pieces[static_cast<int>(my_king_id)]->GetPiecePos();

	// Black pawns move down, white pawns move up, so row delta is positive for black and negative for white
	const int row_delta = (first_black_piece_id <= my_king_id && my_king_id <= last_black_piece_id) ? BOARD_SIDE : (0i64 - BOARD_SIDE);
	bool moved = false;
	if (first_black_piece_id <= my_king_id && my_king_id <= last_black_piece_id)
	{
		// If the pawn is black, its starting row is the second one, so if it's not on that row, it has already moved at least once
		moved = (row != 1);
	}
	else
	{
		// If the pawn is white, its starting row is the seventh one, so if it's not on that row, it has already moved at least once
		moved = (row != 6);
	}

	const int dest1 = pawn_pos + row_delta; // Normal move, but there must be no piece on dest
	const int dest2 = pawn_pos + row_delta - 1; // Eat left, but there must be an enemy piece to be eaten
	const int dest3 = pawn_pos + row_delta + 1; // Eat right, but there must be an enemy piece to be eaten
	const int dest4 = pawn_pos + (row_delta * 2); // Sprint start, but the pawn must not have moved yet and there must not be pieces in between

	if (0 <= dest1 && dest1 < max && the_game.m_Board[dest1] == nullptr) MoveHelper(dest1, pawn_id, pawn_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks);
	if (0 <= dest2 && dest2 < max && the_game.m_Board[dest2] != nullptr) MoveHelper(dest2, pawn_id, pawn_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks);
	if (0 <= dest3 && dest3 < max && the_game.m_Board[dest3] != nullptr) MoveHelper(dest3, pawn_id, pawn_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks);
	if (!moved && the_game.m_Board[dest1] == nullptr && the_game.m_Board[dest4] == nullptr) MoveHelper(dest4, pawn_id, pawn_pos, the_game, my_king_id, my_king_pos, out_moves, allow_checks);

	// En passant: the code works for any pawns of both sides
	if (the_game.m_en_passant_target_pos != INVALID)
	{
		// m_en_passant_target_pos stores the index of the square "behind" the pawn that just moved by 2
		const int en_passant_row = the_game.m_en_passant_target_pos / BOARD_SIDE;
		const int en_passant_col = the_game.m_en_passant_target_pos % BOARD_SIDE;

		// If white, then we need to go up; if black, we need to go down
		if ((en_passant_row == row - 1 || en_passant_row == row + 1) && (en_passant_col == col - 1 || en_passant_col == col + 1))
		{
			// For simplicity, we won't use MoveHelper, as we already know that the move is legal for the pawn, 
			// so we only need to check whether it would expose the king to check, if app says to
			bool king_ok = true;

			// Do not let king expose itself is app says to
			if (!allow_checks)
			{
				// Build what board would look like if player did this move
				board_t b2 = the_game.m_Board;
				b2[the_game.m_en_passant_target_pos] = b2[pawn_pos]; // Capturing pawn would move to board.m_EnPassantIndex

				// Captured pawn is either one row up or one row down, depending on pawn's color, which can be determined
				// considering that white can capture only on fifth rank (i.e.: r = 2), and black only on third (i.e.: r = 5)
				// If white is capturing, its prey will be one row down; if black's capturing, the prey is one row up
				b2[the_game.m_en_passant_target_pos + ((en_passant_row == 2) ? BOARD_SIDE : (0i64 - BOARD_SIDE))] = nullptr;

				king_ok = !IsKingUnderCheck(the_game.m_Pieces, b2, my_king_id, my_king_pos);
			}

			// Add this move if it's legal
			if (king_ok)
			{
				DebugHelper(pawn_id, pawn_pos, the_game.m_en_passant_target_pos, "capture en-passant", true);
				out_moves.push_back(the_game.m_en_passant_target_pos);
			}
			else
			{
				DebugHelper(pawn_id, pawn_pos, the_game.m_en_passant_target_pos, "capture en-passant", false);
			}
		}
	} // En passant
} // Pawn_BuildPossibleMoves


bool Pawn_CanEatKingInSquare(int pawn_pos, int enemy_king_pos, const board_t& board)
{
	// This time we need the id to determine the direction of the move
	const Pieces pawn_id = board[pawn_pos]->GetPieceID();

	// Black pawns move down, white pawns move up, so row delta is positive for black and negative for white
	const int row_delta = (first_black_piece_id <= pawn_id && pawn_id <= last_black_piece_id) ? BOARD_SIDE : (0i64 - BOARD_SIDE);

	const int dest1 = pawn_pos + row_delta - 1; // Eat left, but there must be an enemy piece to be eaten
	const int dest2 = pawn_pos + row_delta + 1; // Eat right, but there must be an enemy piece to be eaten
	return dest1 == enemy_king_pos || dest2 == enemy_king_pos;
} // Pawn_CanEatKingInSquare


bool Pawn_CanMove(int pawn_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	// Black pawns move down, white pawns move up, so row delta is positive for black and negative for white
	const int row_delta = (first_black_piece_id <= my_king_id && my_king_id <= last_black_piece_id) ? BOARD_SIDE : (0i64 - BOARD_SIDE);

	// Note: we're also considering the initial two-row move, because the latter is only possible if the first one is possible.
	const int dest1 = pawn_pos + row_delta; // Normal move, but there must be no piece on dest
	const int dest2 = pawn_pos + row_delta - 1; // Eat left, but there must be an enemy piece to be eaten
	const int dest3 = pawn_pos + row_delta + 1; // Eat right, but there must be an enemy piece to be eaten
	const size_t max = the_game.m_Board.size();
	return (
		(0 <= dest1 && dest1 < max && the_game.m_Board[dest1] == nullptr && CanPieceMoveTo(dest1, pawn_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest2 && dest2 < max && the_game.m_Board[dest2] != nullptr && CanPieceMoveTo(dest2, pawn_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(0 <= dest3 && dest3 < max && the_game.m_Board[dest3] != nullptr && CanPieceMoveTo(dest3, pawn_pos, the_game, my_king_id, my_king_pos, allow_checks)) ||
		(the_game.m_en_passant_target_pos != INVALID && CanPieceMoveTo(the_game.m_en_passant_target_pos, pawn_pos, the_game, my_king_id, my_king_pos, allow_checks)) // En passant
	);
} // Pawn_CanMove