// Chicago, 7/04/2026
//	Completed on 8/04/2026

#include "../Types.h"
#include "../Pieces.h"
#include "../Chess.h"
#include "../ErrorCodes.h"
#include "piece_utilities.h"

#include <cmath>

void King_BuildPossibleMoves(Pieces king_id, int king_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks)
{
	const int row = king_pos / BOARD_SIDE;
	const int col = king_pos % BOARD_SIDE;
	const size_t max = the_game.m_Board.size();

	const int dest1 = king_pos - 1; // Move left
	const int dest2 = king_pos + 1; // Move right
	const int dest3 = king_pos - BOARD_SIDE; // Move up
	const int dest4 = king_pos + BOARD_SIDE; // Move down
	const int dest5 = king_pos - BOARD_SIDE - 1; // Move up left
	const int dest6 = king_pos - BOARD_SIDE + 1; // Move up right
	const int dest7 = king_pos + BOARD_SIDE - 1; // Move down left
	const int dest8 = king_pos + BOARD_SIDE + 1; // Move down right

	// 8 possible standard moves, try them all. Notice: we're moving the king, so we need to update final king_pos too!
	if (0 <= dest1 && dest1 <= max) MoveHelper(dest1, king_id, king_pos, the_game, my_king_id, dest1, out_moves, allow_checks);
	if (0 <= dest2 && dest2 <= max) MoveHelper(dest2, king_id, king_pos, the_game, my_king_id, dest2, out_moves, allow_checks);
	if (0 <= dest3 && dest3 <= max) MoveHelper(dest3, king_id, king_pos, the_game, my_king_id, dest3, out_moves, allow_checks);
	if (0 <= dest4 && dest4 <= max) MoveHelper(dest4, king_id, king_pos, the_game, my_king_id, dest4, out_moves, allow_checks);
	if (0 <= dest5 && dest5 <= max) MoveHelper(dest5, king_id, king_pos, the_game, my_king_id, dest5, out_moves, allow_checks);
	if (0 <= dest6 && dest6 <= max) MoveHelper(dest6, king_id, king_pos, the_game, my_king_id, dest6, out_moves, allow_checks);
	if (0 <= dest7 && dest7 <= max) MoveHelper(dest7, king_id, king_pos, the_game, my_king_id, dest7, out_moves, allow_checks);
	if (0 <= dest8 && dest8 <= max) MoveHelper(dest8, king_id, king_pos, the_game, my_king_id, dest8, out_moves, allow_checks);
		
	// Castling
	//	As in standard chess, Fischer Random Chess960 too allows each player to castle once per game.
	//	After castling, the king and rook's final positions are exactly the same as they would be in standard chess.
	// 
	//	After castling a-side (known as 0-0-0 or Queen-side castling in standard chess),
	//		the King lands on the c-file (c1 for White and c8 for Black) and the Rook is on the d-file (d1 for White; d8 for Black).
	//	After castling h-side (known as 0-0 or King-side castling),
	//		the King lands on the g-file (g1/g8) and the Rook on the f-file (f1/f8).
	//
	// 
	//	The rules for castling in FRC are the same as those in standard chess:
	//
	//	1)	UNMOVED:	The king and the castling rook must not have moved before in the game, including castling.
	// 
	//	2)	UNATTECKED:	All of the squares between the king's initial and final squares
	//						(including the initial and final squares) must not be under attack by any opposing piece.
	// 
	//	3)	VACANT:		All the squares between the king's initial and final squares (including the final square),
	//						and all of the squares between the rook's initial and final squares(including the final square),
	//						must be vacant except for the king and castling rook.
	int dest;

	// Black castle
	if (Pieces::Black_King == king_id)
	{
		// King side castle: king in g8 (i.e.: index = 6) and rook in f8 (i.e.: index = 5)
		dest = 6;
		if (the_game.m_can_black_castle && CanCastle(the_game, king_id, king_pos, dest, Pieces::Black_Rook_King_Side, dest - 1))
		{
			DebugHelper(king_id, king_pos, dest, "castle king side", true);

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			out_moves.push_back(dest);
		} // King side
		else
		{
			DebugHelper(king_id, king_pos, dest, "castle king side", false);
		}

		// Queen side castle: king in c8 (i.e.: index = 2) and rook in d8 (i.e.: index = 3)
		dest = 2;
		if (the_game.m_can_black_castle_long && CanCastle(the_game, king_id, king_pos, dest, Pieces::Black_Rook_Queen_Side, dest + 1))
		{
			DebugHelper(king_id, king_pos, dest, "castle queen side", true);

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			out_moves.push_back(dest);
		} // Queen side
		else
		{
			DebugHelper(king_id, king_pos, dest, "castle queen side", false);
		}

	} // White castle
	else if (Pieces::White_King == king_id)
	{
		// King side castle: king in g1 (i.e.: index = 62) and rook in f1 (i.e.: index = 61)
		dest = 62;
		if (the_game.m_can_white_castle && CanCastle(the_game, king_id, king_pos, dest, Pieces::White_Rook_King_Side, dest - 1))
		{
			DebugHelper(king_id, king_pos, dest, "castle king side", true);

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			out_moves.push_back(dest);
		} // King side
		else
		{
			DebugHelper(king_id, king_pos, dest, "castle king side", false);
		}

		// Queen side castle: king in c1 (i.e.: index = 58) and rook in d1 (i.e.: index = 59)
		dest = 58;
		if (the_game.m_can_white_castle_long && CanCastle(the_game, king_id, king_pos, dest, Pieces::White_Rook_Queen_Side, dest + 1))
		{
			DebugHelper(king_id, king_pos, dest, "castle queen side", true);

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			out_moves.push_back(dest);
		} // Queen side
		else
		{
			DebugHelper(king_id, king_pos, dest, "castle queen side", false);
		}
	}
	else
	{
		// We shouldn't be here
		THROW_CHESS_EXCEPTION(ErrorCode::KingNotSelected, "Piece %d is not a king!", king_id);
	}
} // King_BuildPossibleMoves


bool King_CanEatKingInSquare(int our_king_index, int king_index, const board_t& board)
{
	const int row = our_king_index / BOARD_SIDE;
	const int col = our_king_index % BOARD_SIDE;

	const int king_row = king_index / BOARD_SIDE;
	const int king_col = king_index % BOARD_SIDE;

	// King can eat only in adiacent square
	const int diff_row = std::abs(row - king_row);
	const int diff_col = std::abs(col - king_row);

	// Castling can't capture pieces
	return diff_row == 1 && diff_col == 1;
} // King_CanEatKingInSquare


bool King_CanMove(int king_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	// Try to move by one in each direction
	const int dest1 = king_pos - 1; // Move left
	const int dest2 = king_pos + 1; // Move right
	const int dest3 = king_pos - BOARD_SIDE; // Move up
	const int dest4 = king_pos + BOARD_SIDE; // Move down
	const int dest5 = king_pos - BOARD_SIDE - 1; // Move up left
	const int dest6 = king_pos - BOARD_SIDE + 1; // Move up right
	const int dest7 = king_pos + BOARD_SIDE - 1; // Move down left
	const int dest8 = king_pos + BOARD_SIDE + 1; // Move down right
	const size_t max = the_game.m_Board.size();

	// No need to consider castling here: if the king can castle, it can already move in that direction
	return (
		(0 <= dest1 && dest1 < max && CanPieceMoveTo(dest1, king_pos, the_game, my_king_id, dest1, allow_checks)) ||
		(0 <= dest2 && dest2 < max && CanPieceMoveTo(dest2, king_pos, the_game, my_king_id, dest2, allow_checks)) ||
		(0 <= dest3 && dest3 < max && CanPieceMoveTo(dest3, king_pos, the_game, my_king_id, dest3, allow_checks)) ||
		(0 <= dest4 && dest4 < max && CanPieceMoveTo(dest4, king_pos, the_game, my_king_id, dest4, allow_checks)) ||
		(0 <= dest5 && dest5 < max && CanPieceMoveTo(dest5, king_pos, the_game, my_king_id, dest5, allow_checks)) ||
		(0 <= dest6 && dest6 < max && CanPieceMoveTo(dest6, king_pos, the_game, my_king_id, dest6, allow_checks)) ||
		(0 <= dest7 && dest7 < max && CanPieceMoveTo(dest7, king_pos, the_game, my_king_id, dest7, allow_checks)) ||
		(0 <= dest8 && dest8 < max && CanPieceMoveTo(dest8, king_pos, the_game, my_king_id, dest8, allow_checks))		
	);
} // King_CanMove

