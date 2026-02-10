// Chicago, 8/04/2026
//	Completed on 8/04/2026

#include "../Types.h"
#include "../Pieces.h"
#include "../Chess.h"
#include "../ErrorCodes.h"
#include "piece_utilities.h"

bool isWhite(Pieces id)
{
	return first_white_piece_id <= id && id <= last_white_piece_id;
} // isWhite


bool isBlack(Pieces id)
{
	return first_black_piece_id <= id && id <= last_black_piece_id;
} // isBlack


bool isEnemy(Pieces other_id, Pieces my_piece_id)
{
	// Assume that other_id and my_piece_id are valid. There may be a faster way to check based on 
	// the difference between the two, though this is likely more maintainable and less error-prone.
	return 
		((first_black_piece_id <= other_id	  && other_id	 <= last_black_piece_id) &&	// They black
		(first_white_piece_id  <= my_piece_id && my_piece_id <= last_white_piece_id))	// We white
		|| 
		((first_white_piece_id <= other_id	  && other_id	 <= last_white_piece_id) && // They white
		(first_black_piece_id  <= my_piece_id && my_piece_id <= last_black_piece_id));	// We black
} // isEnemy


bool IsKingUnderCheck(const piece_list& pieces, const board_t& board, Pieces king_id, int king_pos)
{
	int enemy_start_id;
	int enemy_stop_id;

	// Determine which pieces are enemies
	if (king_id == Pieces::White_King)
	{
		enemy_start_id = first_black_piece_index;
		enemy_stop_id  = last_black_piece_index;
	}
	else if (king_id == Pieces::Black_King)
	{
		enemy_start_id = first_white_piece_index;
		enemy_stop_id  = last_white_piece_index;
	}
	else
	{
		THROW_CHESS_EXCEPTION(ErrorCode::InvalidID, "The ID %d is not valid for a king", static_cast<int>(king_id));
	}

	// Check if any enemy piece can eat the king in its current position
	for (int id = enemy_start_id; id < enemy_stop_id; ++id)
	{
		// Instead of looping through all the enemies, we could simply look at the row, col and diagonals of the king
		// and see if there are any enemies that can capture the king. This should reduce the number of iterations from
		// always 16 to as few as possible, but it's not straightforward to implement due to knights, en passant and castling moves.
		if (pieces[id]->CanEatKing(king_pos, board))
		{
			DebugHelper(static_cast<Pieces>(id), pieces[id]->GetPiecePos(), king_pos, "check", true);
			return true;
		}
	}
	return false;
} // IsKingUnderCheck


bool CanPieceMoveTo(int dest, int src, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks)
{
	const auto& board = the_game.m_Board;
	const auto& pieces = the_game.m_Pieces;

	// Check if there is a piece on dest
	if (board[dest] == nullptr)
	{
		// Do not let king expose itself is app says to
		if (!allow_checks)
		{
			// Build what board would look like if player did this move
			board_t b2 = board;
			b2[dest] = b2[src];
			b2[src] = nullptr;

			// If our king is not under check after this move, then we can move there
			return !IsKingUnderCheck(pieces, b2, my_king_id, my_king_pos);
		}

		// Moving to an empty cell is always possible
		return true;
	}
	else
	{
		// There is a piece on dest
		const Pieces other_id = board[dest]->GetPieceID();

		// Enemies can be eaten, but it's better if we leave friends alive 
		if (isEnemy(other_id, my_king_id))
		{
			// Do not let king expose itself is app says to
			if (!allow_checks)
			{
				// Build what board would look like if player did this move
				board_t b2 = board;
				b2[dest] = b2[src];
				b2[src] = nullptr;

				// If our king is not under check after this move, then we can move there
				return !IsKingUnderCheck(pieces, b2, my_king_id, my_king_pos);
			}

			// Capturing an enemy is always possible
			return true;
		}

		// Allies can never be captured
		return false;
	}
} // CanPieceMoveTo


bool MoveHelper(int dest, Pieces piece_id, int src, const Chess& the_game, Pieces my_king_id, int my_king_pos, PossibleMoves& out_moves, bool allow_checks)
{
	const auto& board = the_game.m_Board;
	const auto& pieces = the_game.m_Pieces;

	// Check if there is a piece on dest
	if (board[dest] == nullptr)
	{
		// Do not let king expose itself is app says to
		if (!allow_checks)
		{
			// Build what board would look like if player did this move
			board_t b2 = board;
			b2[dest] = b2[src];
			b2[src] = nullptr;

			// If the king is under check after this move, then we cannot move there
			// However, there may other moves that can be done, so we should keep searching.
			if (IsKingUnderCheck(pieces, b2, my_king_id, my_king_pos))
			{
				DebugHelper(piece_id, src, dest, "move to", false);
				return false;
			}
		}

		// No piece on dest, we can move there
		DebugHelper(piece_id, src, dest, "move to", true);
		out_moves.push_back(dest);
		return false; // Keep searching
	}
	else
	{
		// There is a piece on dest. Regardless of whether this move is valid or not, 
		// we cannot move further in this direction, so we should stop searching after this iteration.
		Pieces id = board[dest]->GetPieceID();

		// Enemies can be eaten, but it's better if we leave friends alive 
		if (isEnemy(id, piece_id))
		{
			// Do not let king expose itself is app says to
			if (!allow_checks)
			{
				// Build what board would look like if player did this move
				board_t b2 = board;
				b2[dest] = b2[my_king_pos];
				b2[my_king_pos] = nullptr;

				if (IsKingUnderCheck(pieces, b2, my_king_id, my_king_pos))
				{
					DebugHelper(piece_id, src, dest, "capture", false);
					return false; // Keep searching
				}
			}

			// Capture the enemy piece
			DebugHelper(piece_id, src, dest, "capture", true);
			out_moves.push_back(dest);
			return false; // Keep searching
		}

		// Stop searching in this direction
		DebugHelper(piece_id, src, dest, "capture", false);
		return true;
	}
} // MoveHelper


bool CanCastle(const Chess& the_game, Pieces king_id, int king_src, int king_dest, Pieces rook_id, int rook_dest)
{
	const auto& board = the_game.m_Board;
	const auto& pieces = the_game.m_Pieces;
	const auto& rook_pos = pieces[static_cast<int>(rook_id)]->GetPiecePos();

	// Cannot castle if king is starting, moving or landing under check
	if (IsKingUnderCheck(pieces, board, king_id, king_src))
		return false;

	if (king_src > king_dest) // Go left
	{
		// Check dest too, as the king must not be under check in its final position.
		for (int i = king_src - 1; i >= king_dest; --i)
		{
			// Build what board would look like if player did this move.
			board_t b2 = board;
			b2[i] = b2[king_src];
			b2[king_src] = nullptr;

			// Kings can travel through the castle rook, but any other pieces prevents castling.
			if ((board[i] != nullptr && board[i]->GetPieceID() != rook_id) || IsKingUnderCheck(pieces, b2, king_id, i))
				return false;
		}
	}
	else if (king_src < king_dest) // Go right
	{
		// Check dest too, as the king must not be under check in its final position.
		for (int i = king_src + 1; i <= king_dest; ++i)
		{
			// Build what board would look like if player did this move.
			board_t b2 = board;
			b2[i] = b2[king_src];
			b2[king_src] = nullptr;

			// Kings can travel through the castle rook, but any other pieces prevents castling.
			if ((board[i] != nullptr && board[i]->GetPieceID() != rook_id) || IsKingUnderCheck(pieces, b2, king_id, i))
				return false;
		}
	}
	else 
	{
		// King doesn't need to move: could happen in 960.
		// We already checked that the king is not under check in its current position, so we can skip that part now.
	}

	// Cannot castle if rook path to its destination isn't clear
	// Classic chess:
	//	- for king side castle, if the king can reach its destination, then the rook can too, so we only need to check the king's path.
	//	- for queen side castle, however, the rook travels also through the knight, so we need to check if that square is clear too.
	// Fischer Random Chess960:
	//	-> in both cases, there may be pieces between the rook and its destination even if the king's path is clear
	if (rook_pos > rook_dest)
	{
		// Rooks can travel through the king, but any other pieces prevents castling
		for (int i = rook_pos - 1; i >= rook_dest; --i) // Go left
			if ((board[i] != nullptr && board[i]->GetPieceID() != king_id))
				return false;
	}
	else if (rook_pos < rook_dest)
	{
		// Rooks can travel through the king, but any other pieces prevents castling
		for (int i = rook_pos + 1; i <= rook_dest; ++i) // Go right
			if ((board[i] != nullptr && board[i]->GetPieceID() != king_id))
				return false;
	}
	else
	{
		// Rook doesn't need to move: could happen in 960.
	}

	// If everything is good, player can castle
	return true;
} // Castle
