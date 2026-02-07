// Milan, 7th December 2021
//	Completed on December, 11
// 

#include "Pieces.h"
#include "Chess.h"
#include "../Libraries/Logger.h"
using namespace ENGINE_NAMESPACE;

// Prototypes
static bool CheckHelper(const board_t& board, int king_pos);
static bool Castle(const board_t& board, int curr_king, int king_dest, Pieces rook_id, int rook_dest);

// If app doesn't want to prevent leaving kings under check, no need to know kings' pos
static int CanPieceMoveToSquare(const board_t& board, Pieces this_ID, int src, int dest, bool allowChecks, int king_pos = INVALID); 

// ========================================================================================================================================
// ========================================================================================================================================
// Helpers
// ========================================================================================================================================
// ========================================================================================================================================
#define FOUND_ENEMY		-1
#define CAN_MOVE		1
#define CANNOT_MOVE		0

#if DEBUGGING

#define CASE(x) case Pieces::x: str = #x; break;
__forceinline static void DebugHelper(Pieces id, int i, int j, const char* special, bool move_or_check = true)
{
	// Map id to string
	std::string str;
	switch (id)
	{
		CASE(Black_Rook_1);		// 1
		CASE(Black_Rook_2);		// 2
		CASE(Black_Knight_1);	// 3
		CASE(Black_Knight_2);	// 4
		CASE(Black_Bishop_1);	// 5
		CASE(Black_Bishop_2);	// 6
		CASE(Black_Queen);		// 7
		CASE(Black_King);		// 8
		CASE(Black_Pawn_1);		// 9
		CASE(Black_Pawn_2);		// 10
		CASE(Black_Pawn_3);		// 11
		CASE(Black_Pawn_4);		// 12
		CASE(Black_Pawn_5);		// 13
		CASE(Black_Pawn_6);		// 14
		CASE(Black_Pawn_7);		// 15
		CASE(Black_Pawn_8);		// 16
		CASE(White_Rook_1);		// 17
		CASE(White_Rook_2);		// 18
		CASE(White_Knight_1);	// 19
		CASE(White_Knight_2);	// 20
		CASE(White_Bishop_1);	// 21
		CASE(White_Bishop_2);	// 22
		CASE(White_Queen);		// 23
		CASE(White_King);		// 24
		CASE(White_Pawn_1);		// 25
		CASE(White_Pawn_2);		// 26
		CASE(White_Pawn_3);		// 27
		CASE(White_Pawn_4);		// 28
		CASE(White_Pawn_5);		// 29
		CASE(White_Pawn_6);		// 30
		CASE(White_Pawn_7);		// 31
		CASE(White_Pawn_8);		// 32
	default: str = "Unknown piece";
	} // switch

	// Create debug string
	if (move_or_check)
		printf("%-16s can be moved to (%d %d) - (%s)\n", str.c_str(), i + 1, j + 1, special);
	else
		printf("%s can be attacked by %s from (%d %d)\n", special, str.c_str(), i + 1, j + 1);
}
#undef CASE
#else
#define DebugHelper(id, i, j, b)	{ void(0); }
#endif

__forceinline static void RotateBy90(short vec[])
{
	// Multiply by rotation matrix, which according to linear algebra is 
	// | 0 -1 |
	// | 1	0 |
	// Given the rule for matrix multiplication, the product simply is
	short hold = vec[0];
	vec[0] = vec[1] * (-1);
	vec[1] = hold;
}

__forceinline static void SwapCoords(short vec[])
{
	// Multiply by reflexion matrix when theta = pi/2, which according to linear algebra is 
	// | 0 1 |
	// | 1 0 |
	// Given the rule for matrix multiplication, the product simply is
	short hold = vec[0];
	vec[0] = vec[1];
	vec[1] = hold;
}

bool CheckHelper(const board_t& board, int king_pos)
{
	Pieces id;
	Pieces enemy_start_id;
	Pieces enemy_stop_id;

	if (board[king_pos]->GetPieceID() == Pieces::White_King)
	{
		enemy_start_id = Pieces::Black_Rook_1;
		enemy_stop_id = Pieces::Black_Pawn_8;
	}
	else if (board[king_pos]->GetPieceID() == Pieces::Black_King)
	{
		enemy_start_id = Pieces::White_Rook_1;
		enemy_stop_id = Pieces::White_Pawn_8;
	}
	else
	{
		LOG_ERROR(LOGS::L_INVALID_PARAMETER, "Didn't select a king");
		return false;
	}

	for (int i = 0; i < board.size(); ++i)
	{
		if (board[i] != nullptr && (enemy_start_id <= (id = board[i]->GetPieceID()) && id <= enemy_stop_id) &&
			board[i]->CanEatKing(i, king_pos, board))
		{
			DebugHelper(id, i / BOARD_SIDE, i % BOARD_SIDE, (enemy_start_id == Pieces::Black_Rook_1 ? "White king" : "Black king"), false);
			return true;
		}
	}
	return false;
} // CheckHelper


bool Castle(const board_t& board, int curr_king, int king_dest, Pieces rook_id, int rook_dest)
{
	int i;
	int rook = INVALID;
	int king_rank = curr_king / BOARD_SIDE;

#if DEBUGGING
	{
		// Do a few checks; this function is local to this file so it's safe to remove them from release builds
		auto board_size = board.size();
		if (!(0 <= curr_king && curr_king < board_size) ||	// King must be on the board
			!(0 <= king_dest && king_dest < board_size) ||	// King's dest must be on the board
			!(0 <= rook_dest && rook_dest < board_size) ||	// Rook must be on the board
			king_rank != king_dest / BOARD_SIDE || king_rank != rook_dest / BOARD_SIDE)	// King, king's dest and rook must be on the same rank
		{
			LOG_ERROR(LOGS::L_INVALID_PARAMETER, "Out of the board");
			return false;
		}
	}
#endif

	// 2) Cannot castle if king is starting, moving or landing under check
	//		King can travel through the castle rook, but any other pieces prevents castling 
	if (curr_king > king_dest)
	{
		for (i = curr_king - 1; i >= king_dest; --i)
		{
			// Build what board would look like if player did this move
			board_t b2 = board;
			b2[i] = b2[curr_king];
			b2[curr_king] = nullptr;

			if ((board[i] != nullptr && board[i]->GetPieceID() != rook_id) || CheckHelper(b2, i))
				return false;
		}
	}
	else if (curr_king < king_dest)
	{
		for (i = curr_king + 1; i <= king_dest; ++i)
		{
			// Build what board would look like if player did this move
			board_t b2 = board;
			b2[i] = b2[curr_king];
			b2[curr_king] = nullptr;

			if ((board[i] != nullptr && board[i]->GetPieceID() != rook_id) || CheckHelper(b2, i))
				return false;
		}
	}
	else // King doesn't need to move: could happen in 960
	{
		// Build what board would look like if player did this move
		if (CheckHelper(board, king_dest))
			return false;
	}

	// Find the castling rook - it must be on the king's rank
	int index = king_rank * BOARD_SIDE;
	for (i = 0; i < BOARD_SIDE; ++i, ++index)
	{
		if (board[index] != nullptr && board[index]->GetPieceID() == rook_id)
		{
			rook = index;
			break;
		}
	}

	// 3) Cannot castle if rook path to its destination isn't clear
	//		Rook can travel through the king, but any other pieces prevents castling 
	if (rook == INVALID)
		return false;
	else if (rook > rook_dest)
	{
		for (i = rook - 1; i >= rook_dest; --i)
			if ((board[i] != nullptr && board[i]->GetPieceID() != rook_id))
				return false;
	}
	else if (rook < rook_dest)
	{
		for (i = rook + 1; i <= rook_dest; ++i)
			if ((board[i] != nullptr && board[i]->GetPieceID() != rook_id))
				return false;
	}

	// If everything is good, player can castle
	return true;
} // Castle


 // If app doesn't want to prevent leaving kings under check, no need to know kings' pos
__forceinline int CanPieceMoveToSquare(const board_t& board, Pieces this_ID, int src, int dest, bool allowChecks, int king_pos)
{
#if DEBUGGING
	{
		// Do a few checks; this function is local to this file so it's safe to remove them from release builds
		auto board_size = board.size();
		if (this_ID == Pieces::Black_King || this_ID == Pieces::White_King ||		// Doesn't work for kings
			(!allowChecks && !(0 <= king_pos && king_pos < board_size)) ||			// If app wants to prevent checks king pos must be valid
			!(0 <= src && src < board_size) || !(0 <= dest && dest < board_size))	// src and dest must be valid indeces
		{
			LOG_ERROR(LOGS::L_INVALID_PARAMETER, "Out of the board");
			return -2; // Just a random error code
		}
	}
#endif

	if (board[dest] == nullptr)
	{
		// Do not let king expose itself is app says to
		if (!allowChecks)
		{
			// Build what board would look like if player did this move
			board_t b2 = board;
			b2[dest] = b2[src];
			b2[src] = nullptr;

			if (CheckHelper(b2, king_pos))
				return CANNOT_MOVE;
		}
		return CAN_MOVE;
	}
	else
	{
		Pieces id = board[dest]->GetPieceID();

		// Enemy can be eaten, but it's better if we leave friends alive 
		if (((Pieces::White_Rook_1	<= this_ID	&& this_ID	<= Pieces::White_Pawn_8) &&		// We White
			(Pieces::Black_Rook_1	<= id		&& id		<= Pieces::Black_Pawn_8))	||	// They Black or
			((Pieces::Black_Rook_1	<= this_ID	&& this_ID	<= Pieces::Black_Pawn_8) &&		// We Black
			(Pieces::White_Rook_1	<= id		&& id		<= Pieces::White_Pawn_8)))		// They White
		{
			// Do not let king expose itself is app says to
			if (!allowChecks)
			{
				// Build what board would look like if player did this move
				board_t b2 = board;
				b2[dest] = b2[src];
				b2[src] = nullptr;

				if (CheckHelper(b2, king_pos))
					return CANNOT_MOVE;
			}
			return FOUND_ENEMY;
		}
	}

	return CANNOT_MOVE;
} // CanPieceMoveToSquare



// ========================================================================================================================================
// ========================================================================================================================================
// Algorithms to build moves' trees
// ========================================================================================================================================
// ========================================================================================================================================

void Rook_BuildPossibleMoves(Pieces this_ID, int indexInboard_t, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks)
{
	const int row = indexInboard_t / BOARD_SIDE;
	const int col = indexInboard_t % BOARD_SIDE;

	int king_pos = INVALID;
	int dest;
	int res = -2;

	const board_t& _board = board.m_Board;

	// Black piece - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White piece - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

#if DEBUGGING
	ThrowIfFalse(king_pos != INVALID, LOGS::L_ASSERTION_FAILED, "Cannot found king on board");
#endif

	// Horizontal backward
	for (int i = col - 1; i >= 0; --i)
	{
		dest = row * BOARD_SIDE + i;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, row, i, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, row, i, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	goto Part_2; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}

	// Horizontal forward
Part_2:
	for (int i = col + 1; i < BOARD_SIDE; ++i)
	{
		dest = row * BOARD_SIDE + i;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, row, i, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, row, i, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	goto Part_3; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}

	// Vertical up
Part_3:
	for (int i = row - 1; i >= 0; --i)
	{
		dest = i * BOARD_SIDE + col;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, i, col, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, i, col, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	goto Part_4; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}

	// Vertical down
Part_4:
	for (int i = row + 1; i < BOARD_SIDE; ++i)
	{
		dest = i * BOARD_SIDE + col;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, i, col, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, i, col, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	return; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}
} // ROOK->VBuildPossibleMoves


void Knight_BuildPossibleMoves(Pieces this_ID, int indexInboard_t, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks)
{
	const int row = indexInboard_t / BOARD_SIDE;
	const int col = indexInboard_t % BOARD_SIDE;
	
	// Eight possible positions
	short delta[2] = { 1,  2 };
	int dest, dest1, dest2;
	
	int king_pos = INVALID;
	int res = -2;

	const board_t& _board = board.m_Board;

	// Black piece - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White piece - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

#if DEBUGGING
	ThrowIfFalse(king_pos != INVALID, LOGS::L_ASSERTION_FAILED, "Cannot found king on board");
#endif

	for (int i = 0; i < 4; ++i)
	{
		// One horizontal and two vertical
		dest1 = row + delta[1];
		dest2 = col + delta[0];

		// Skip out-of board moves and same square
		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			dest = dest1 * BOARD_SIDE + dest2;
			res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
			if (res == CAN_MOVE || res == FOUND_ENEMY)
			{	
				DebugHelper(this_ID, dest1, dest2, "Normal");
				outMoves.push_back(dest);
			}
		}

		// Two horizontal and one vertical
		dest1 = row + delta[0];
		dest2 = col + delta[1];

		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			dest = dest1 * BOARD_SIDE + dest2;
			res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
			if (res == CAN_MOVE || res == FOUND_ENEMY)
			{
				DebugHelper(this_ID, dest1, dest2, "Normal");
				outMoves.push_back(dest);
			}
		}

		// Rotate by 90 degrees
		RotateBy90(delta);
	}
} // KNIGHT->VBuildPossibleMoves


void Bishop_BuildPossibleMoves(Pieces this_ID, int indexInboard_t, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks)
{
	// Diagonals
	const int row = indexInboard_t / BOARD_SIDE;
	const int col = indexInboard_t % BOARD_SIDE;

	int i, j;
	int dest;
	
	int king_pos = INVALID;
	int res = -2;

	const board_t& _board = board.m_Board;

	// Black piece - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White piece - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

#if DEBUGGING
	ThrowIfFalse(king_pos != INVALID, LOGS::L_ASSERTION_FAILED, "Cannot found king on board");
#endif

	// First diagonal - down 
	for (i = row + 1, j = col + 1; i < BOARD_SIDE && j < BOARD_SIDE; ++i, ++j)
	{
		dest = i * BOARD_SIDE + j;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, i, j, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, i, j, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	goto Part_2; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}

	// First diagonal - up 
Part_2:
	for (i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j)
	{
		dest = i * BOARD_SIDE + j;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, i, j, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, i, j, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	goto Part_3; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}

	// Second diagonal - down
Part_3:
	for (i = row + 1, j = col - 1; i < BOARD_SIDE && j >= 0; ++i, --j)
	{
		dest = i * BOARD_SIDE + j;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, i, j, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, i, j, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	goto Part_4; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}

	// Second diagonal - up 
Part_4:
	for (i = row - 1, j = col + 1; i >= 0 && j < BOARD_SIDE; --i, ++j)
	{
		dest = i * BOARD_SIDE + j;
		res = CanPieceMoveToSquare(_board, this_ID, indexInboard_t, dest, allowChecks, king_pos);
		switch (res)
		{
		case CAN_MOVE:		DebugHelper(this_ID, i, j, "Normal"); outMoves.push_back(dest); break; // Note that this exits from switch, not from for
		case FOUND_ENEMY:	DebugHelper(this_ID, i, j, "Capture"); outMoves.push_back(dest); // Fall through
		case CANNOT_MOVE:	return; // Obstacle 
		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Cannot decide if piece can move");
		}
	}
} // BISHOP->VBuildPossibleMoves


void Queen_BuildPossibleMoves(Pieces this_ID, int indexInboard_t, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks)
{
	Rook_BuildPossibleMoves(this_ID, indexInboard_t, board, outMoves, allowChecks);
	Bishop_BuildPossibleMoves(this_ID, indexInboard_t, board, outMoves, allowChecks);
} // QUEEN->VBuildPossibleMoves


void King_BuildPossibleMoves(Pieces this_ID, int indexInboard_t, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks)
{
	const int row = indexInboard_t / BOARD_SIDE;
	const int col = indexInboard_t % BOARD_SIDE;
	const board_t& _board = board.m_Board;
	
	int dest, dest1, dest2;
	Pieces id;

	// 1 in every direction
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			dest1 = row + i;
			dest2 = col + j;

			if ((i != j || (i != 0 && j != 0)) && 0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
			{
				dest = dest1 * BOARD_SIDE + dest2;

				// Do not let king expose itself is app says to
				if (!allowChecks)
				{
					// Build what board would look like if player did this move
					board_t b2 = _board;
					b2[dest] = b2[indexInboard_t];
					b2[indexInboard_t] = nullptr;

					if (CheckHelper(b2, dest))
						continue;
				}

				if (_board[dest] == nullptr)
				{
					DebugHelper(this_ID, dest1, dest2, "Normal");
					outMoves.push_back(dest);
				}
				else
				{
					id = _board[dest]->GetPieceID();

					// Enemy
					if (((Pieces::White_King	== this_ID) &&	// We White King
						(Pieces::Black_Rook_1	<= id		&& id	<= Pieces::Black_Pawn_8))	||	// They Black
						((Pieces::Black_King	== this_ID) &&	// We Black King
						(Pieces::White_Rook_1	<= id		&& id	<= Pieces::White_Pawn_8)))		// They White
					{
						DebugHelper(this_ID, dest1, dest2, "Capture");
						outMoves.push_back(dest);
					}
				}
			}
		} // for col
	} // for row


	// Castling
	// 
	//	As in standard chess, Fischer Random Chess960 allows each player to castle once per game.
	//	After castling, the king and rook's final positions are exactly the same as they would be in standard chess.
	//	Thus, after castling a-side (known as 0-0-0 or Queen-side castling in standard chess),
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
	
	// Black castle
	if (Pieces::Black_King == this_ID)
	{
		// King side castle: king in g8 (i.e.: index = 6) and rook in f8 (i.e.: index = 5)
		dest = 6;
		if (board.m_Black_Castle && Castle(_board, indexInboard_t, dest, Pieces::Black_Rook_2, dest - 1)) // 1) Unmoved
		{
			DebugHelper(this_ID, row, dest, "Castling");

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			outMoves.push_back(dest);
		} // King side

		// Queen side castle: king in c8 (i.e.: index = 2) and rook in d8 (i.e.: index = 3)
		dest = 2;
		if (board.m_Black_CastleLong && Castle(_board, indexInboard_t, dest, Pieces::Black_Rook_1, dest + 1)) // 1) Unmoved
		{
			DebugHelper(this_ID, row, dest, "Castling");

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			outMoves.push_back(dest);
		} // Queen side

	} // White castle
	else if (Pieces::White_King == this_ID)
	{
		// King side castle: king in g1 (i.e.: index = 62) and rook in f1 (i.e.: index = 61)
		dest = 62;
		if (board.m_Black_Castle && Castle(_board, indexInboard_t, dest, Pieces::White_Rook_2, dest - 1)) // 1) Unmoved
		{
			DebugHelper(this_ID, row, dest, "Castling");

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			outMoves.push_back(dest);
		} // King side

		// Queen side castle: king in c1 (i.e.: index = 58) and rook in d1 (i.e.: index = 59)
		dest = 58;
		if (board.m_Black_CastleLong && Castle(_board, indexInboard_t, dest, Pieces::White_Rook_1, dest + 1)) // 1) Unmoved
		{
			DebugHelper(this_ID, row, dest, "Castling");

			// If everything is good, insert move - NOTE: the app must manually move the rook if players choose to castle
			outMoves.push_back(dest);
		} // Queen side
	}
} // KING->VBuildPossibleMoves


// TODO: this function could be rewritten
void Pawn_BuildPossibleMoves(Pieces this_ID, int indexInboard_t, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks)
{
	const int row = indexInboard_t / BOARD_SIDE;
	const int col = indexInboard_t % BOARD_SIDE;

	const board_t& _board = board.m_Board;
	Pieces id;
	int dest;
	int king_pos = INVALID;
	bool king_ok = true;

	// Black pawns
	if (Pieces::Black_Pawn_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8 && row != BOARD_SIDE - 1)
	{
		// Black piece - find black king
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}

#if DEBUGGING
		ThrowIfFalse(king_pos != INVALID, LOGS::L_ASSERTION_FAILED, "Cannot found king on board");
#endif

		// Normal move: Same column, one row down
		dest = indexInboard_t + BOARD_SIDE;
		if (_board[dest] == nullptr) // No obstacles
		{
			king_ok = true;
			// Do not let king expose itself is app says to
			if (!allowChecks)
			{
				// Build what board would look like if player did this move
				board_t b2 = _board;
				b2[dest] = b2[indexInboard_t];
				b2[indexInboard_t] = nullptr;

				king_ok = !CheckHelper(b2, king_pos);
			}
			if (king_ok)
			{
				DebugHelper(this_ID, dest, col, "Normal");
				outMoves.push_back(dest);
			}
		}


		// Normal Meal: one column either left or right, one row down

		// Left
		if (col != 0)
		{
			dest = indexInboard_t + BOARD_SIDE - 1;
			if (_board[dest] != nullptr && // Only eat enemies
				(Pieces::White_Rook_1 <= (id = _board[dest]->GetPieceID()) && id <= Pieces::White_Pawn_8))
			{
				king_ok = true;
				// Do not let king expose itself is app says to
				if (!allowChecks)
				{
					// Build what board would look like if player did this move
					board_t b2 = _board;
					b2[dest] = b2[indexInboard_t];
					b2[indexInboard_t] = nullptr;

					king_ok = !CheckHelper(b2, king_pos);
				}
				if (king_ok)
				{
					DebugHelper(this_ID, row + 1, col - 1, "Pawn Capture");
					outMoves.push_back(dest);
				}
			}
		}

		// Right
		if (col != BOARD_SIDE - 1)
		{
			dest = indexInboard_t + BOARD_SIDE + 1;
			if (_board[dest] != nullptr && // Only eat enemies
				(Pieces::White_Rook_1 <= (id = _board[dest]->GetPieceID()) && id <= Pieces::White_Pawn_8))
			{
				king_ok = true;
				// Do not let king expose itself is app says to
				if (!allowChecks)
				{
					// Build what board would look like if player did this move
					board_t b2 = _board;
					b2[dest] = b2[indexInboard_t];
					b2[indexInboard_t] = nullptr;

					king_ok = !CheckHelper(b2, king_pos);
				}
				if (king_ok)
				{
					DebugHelper(this_ID, row + 1, col + 1, "Pawn Capture");
					outMoves.push_back(dest);
				}
			}
		}

		// Sprint start: at home pos (seventh rank), two row down
		dest = indexInboard_t + 2 * BOARD_SIDE;
		if ((BOARD_SIDE <= indexInboard_t && indexInboard_t < 2 * BOARD_SIDE) && // In seventh rank
			_board[indexInboard_t + BOARD_SIDE] == nullptr && _board[dest] == nullptr) // No obstacles
		{
			king_ok = true;
			// Do not let king expose itself is app says to
			if (!allowChecks)
			{
				// Build what board would look like if player did this move
				board_t b2 = _board;
				b2[dest] = b2[indexInboard_t];
				b2[indexInboard_t] = nullptr;

				king_ok = !CheckHelper(b2, king_pos);
			}
			if (king_ok)
			{
				DebugHelper(this_ID, row + 2, col, "Pawn super start");
				outMoves.push_back(dest);
			}
		}
	}

	// White pawns
	else if (Pieces::White_Pawn_1 <= this_ID && this_ID <= Pieces::White_Pawn_8 && row != 0)
	{
		// White piece - find white king
		for (int i = 0; i < _board.size(); ++i)
		{
			if (_board[i] != nullptr && _board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}

#if DEBUGGING
		ThrowIfFalse(king_pos != INVALID, LOGS::L_ASSERTION_FAILED, "Cannot found king on board");
#endif

		// Normal move: Same column, one row up
		dest = indexInboard_t - BOARD_SIDE;
		if (_board[dest] == nullptr) // No obstacles
		{
			king_ok = true;
			// Do not let king expose itself is app says to
			if (!allowChecks)
			{
				// Build what board would look like if player did this move
				board_t b2 = _board;
				b2[dest] = b2[indexInboard_t];
				b2[indexInboard_t] = nullptr;

				king_ok = !CheckHelper(b2, king_pos);
			}
			if (king_ok)
			{
				DebugHelper(this_ID, row - 1, col, "Normal");
				outMoves.push_back(dest);
			}
		}


		// Normal Meal: one column either left or right, one row up

		// Left
		if (col != 0)
		{
			dest = indexInboard_t - BOARD_SIDE - 1;
			if (_board[dest] != nullptr && // Only eat enemies
				(Pieces::Black_Rook_1 <= (id = _board[dest]->GetPieceID()) && id <= Pieces::Black_Pawn_8))
			{
				king_ok = true;
				// Do not let king expose itself is app says to
				if (!allowChecks)
				{
					// Build what board would look like if player did this move
					board_t b2 = _board;
					b2[dest] = b2[indexInboard_t];
					b2[indexInboard_t] = nullptr;

					king_ok = !CheckHelper(b2, king_pos);
				}
				if (king_ok)
				{
					DebugHelper(this_ID, row - 1, col - 1, "Pawn Capture");
					outMoves.push_back(dest);
				}
			}
		}

		// Right
		if (col != BOARD_SIDE - 1)
		{
			dest = indexInboard_t - BOARD_SIDE + 1;
			if (_board[dest] != nullptr && // Only eat enemies
				(Pieces::Black_Rook_1 <= (id = _board[dest]->GetPieceID()) && id <= Pieces::Black_Pawn_8))
			{
				king_ok = true;
				// Do not let king expose itself is app says to
				if (!allowChecks)
				{
					// Build what board would look like if player did this move
					board_t b2 = _board;
					b2[dest] = b2[indexInboard_t];
					b2[indexInboard_t] = nullptr;

					king_ok = !CheckHelper(b2, king_pos);
				}
				if (king_ok)
				{
					DebugHelper(this_ID, row - 1, col + 1, "Pawn Capture");
					outMoves.push_back(dest);
				}
			}
		}


		// Sprint start: at home pos (second rank), two row up
		dest = indexInboard_t - 2 * BOARD_SIDE;
		if ((6 * BOARD_SIDE <= indexInboard_t && indexInboard_t < 7 * BOARD_SIDE) && // In second rank
			_board[indexInboard_t - BOARD_SIDE] == nullptr && _board[dest] == nullptr) // No obstacles
		{
			king_ok = true;
			// Do not let king expose itself is app says to
			if (!allowChecks)
			{
				// Build what board would look like if player did this move
				board_t b2 = _board;
				b2[dest] = b2[indexInboard_t];
				b2[indexInboard_t] = nullptr;

				king_ok = !CheckHelper(b2, king_pos);
			}
			if (king_ok)
			{
				DebugHelper(this_ID, row - 2, col, "Pawn super start");
				outMoves.push_back(dest);
			}
		}
	}
	else
		LOG_ERROR(LOGS::L_INVALID_ID, "Not a Pawn...");


	// En passant: the code works for any pawns of both sides
	if (board.m_EnPassantIndex != INVALID)
	{
		// m_EnPassantIndex stores the index of the square "behind" the pawn that just moved by 2,
		// so in order to capture it an enemy pawn must be on the same row, either at left or right
		const int r = board.m_EnPassantIndex / BOARD_SIDE;
		const int c = board.m_EnPassantIndex % BOARD_SIDE;

		if ((r == row - 1 || r == row + 1) && (c == col - 1 || c == col + 1))
		{
			king_ok = true;
			// Do not let king expose itself is app says to
			if (!allowChecks)
			{
				// Build what board would look like if player did this move
				board_t b2 = _board;
				b2[board.m_EnPassantIndex] = b2[indexInboard_t]; // Capturing pawn would move to board.m_EnPassantIndex

				// Captured pawn is either one row up or one row down, depending on pawn's color, which can be determined
				// considering that white can capture only on fifth rank (i.e.: r = 2), and black only on third (i.e.: r = 5)
				// if white is capturing, its prey will be one row down; if black's capturing, the prey is one row up
				b2[board.m_EnPassantIndex + ((r == 2) ? BOARD_SIDE : -BOARD_SIDE)] = nullptr;

				king_ok = !CheckHelper(b2, king_pos);
			}
			if (king_ok)
			{
				DebugHelper(this_ID, board.m_EnPassantIndex, board.m_EnPassantIndex + ((r == 2) ? BOARD_SIDE : -BOARD_SIDE), "En Passant");
				outMoves.push_back(board.m_EnPassantIndex);
			}
		}
	} // En passant
} // PAWN->VBuildPossibleMoves



// ========================================================================================================================================
// ========================================================================================================================================
// Algorithms to determine whether any pieces is currently threatened - used usually on kings to look for possible checks
// ========================================================================================================================================
// ========================================================================================================================================

bool Rook_CanEatKingInSquare(int rook_index, int king_index, const board_t& board)
{
	const int row = rook_index / BOARD_SIDE;
	const int col = rook_index % BOARD_SIDE;

	const int king_row = king_index / BOARD_SIDE;
	const int king_col = king_index % BOARD_SIDE;

	// Rooks can only move by rows or columns if no obstacle is found

	// Same col
	if (col == king_col)
	{
		int i = INVALID;
		if (row < king_row) // Go down
		{
			for (i = row + 1; i <= king_row; ++i)
				if (board[i * BOARD_SIDE + col] != nullptr) // Found a piece
					break;
		}
		else if (row > king_row) // Go down
		{
			for (i = row - 1; i >= king_row; --i)
				if (board[i * BOARD_SIDE + col] != nullptr) // Found a piece
					break;
		}

		if (i == king_row) // King was reached
			return true;
	}

	// Same row - cannot be on both, otherwise it would be the same piece
	else if (row == king_row)
	{
		int i = INVALID;
		if (row < king_row) // Go down
		{
			for (i = row + 1; i <= king_row; ++i)
				if (board[row * BOARD_SIDE + i] != nullptr) // Found a piece
					break;
		}
		else if (row > king_row) // Go down
		{
			for (i = row - 1; i >= king_row; --i)
				if (board[row * BOARD_SIDE + i] != nullptr) // Found a piece
					break;
		}

		if (i == king_row) // King was reached
			return true;
	}

	return false;
} // Rook_CanEatKingInSquare

bool Knight_CanEatKingInSquare(int knight_index, int king_index, const board_t& board)
{
	const int row = knight_index / BOARD_SIDE;
	const int col = knight_index % BOARD_SIDE;

	const int king_row = king_index / BOARD_SIDE;
	const int king_col = king_index % BOARD_SIDE;

	short delta[2] = { 1,  2 };

	for (int i = 0; i < 4; ++i)
	{
		if ((row + delta[1] == king_row && col + delta[0] == king_col) ||
			(row + delta[0] == king_row && col + delta[1] == king_col))
		{
			return true;
		}

		// Rotate by 90 degrees
		RotateBy90(delta);
	}

	return false;
} // Knight_CanEatKingInSquare

bool Bishop_CanEatKingInSquare(int bishop_index, int king_index, const board_t& board)
{
	const int row = bishop_index / BOARD_SIDE;
	const int col = bishop_index % BOARD_SIDE;

	const int king_row = king_index / BOARD_SIDE;
	const int king_col = king_index % BOARD_SIDE;

	// Bishops can only move diagonally if no obstacle is found
	int i = INVALID, j = INVALID;
	if (row < king_row && col < king_col) // First diagonal, go down
	{
		for (i = row + 1, j = col + 1; i <= king_row && j <= king_col; ++i, ++j)
			if (board[i * BOARD_SIDE + j] != nullptr) // Found a piece
				break;
	}
	else if (row > king_row && col > king_col) // First diagonal, go up
	{
		for (i = row - 1, j = col - 1; i >= king_row && j >= king_col; --i, --j)
			if (board[i * BOARD_SIDE + j] != nullptr) // Found a piece
				break;
	}
	else if (row < king_row && col > king_col) // Second diagonal, go down
	{
		for (i = row + 1, j = col - 1; i <= king_row && j >= king_col; ++i, --j)
			if (board[i * BOARD_SIDE + j] != nullptr) // Found a piece
				break;
	}
	else if (row > king_row && col < king_col) // Second diagonal, go up
	{
		for (i = row - 1, j = col + 1; i >= king_row && j <= king_col; --i, ++j)
			if (board[i * BOARD_SIDE + j] != nullptr) // Found a piece
				break;
	}
	else
	{
		// The only other option is that the pieces stands either on the same row or col
		// In this case the bishop can never reach its prey
		return false;
	}

	if (i == king_row && j == king_col) // King was reached
		return true;

	return false;
} // Bishop_CanEatKingInSquare

bool Queen_CanEatKingInSquare(int queen_index, int king_index, const board_t& board)
{
	return Bishop_CanEatKingInSquare(queen_index, king_index, board) || Rook_CanEatKingInSquare(queen_index, king_index, board);
} // Queen_CanEatKingInSquare

bool King_CanEatKingInSquare(int our_king_index, int king_index, const board_t& board)
{
	const int row = our_king_index / BOARD_SIDE;
	const int col = our_king_index % BOARD_SIDE;

	const int king_row = king_index / BOARD_SIDE;
	const int king_col = king_index % BOARD_SIDE;

	// King can eat only in adiacent square
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			if ((i != j || (i != 0 && j != 0)) && row == king_row + i && col == king_col + j)
				return true;
		}
	}

	return false;
} // King_CanEatKingInSquare

bool Pawn_CanEatKingInSquare(int pawn_index, int king_index, const board_t& board)
{
	const int row = pawn_index / BOARD_SIDE;
	const int col = pawn_index % BOARD_SIDE;

	const int king_row = king_index / BOARD_SIDE;
	const int king_col = king_index % BOARD_SIDE;

	Pieces id;

	// Pawn can only eat if king is in diagonal
	if (board[pawn_index] != nullptr)
	{
		id = board[pawn_index]->GetPieceID();

		if ((king_col == col - 1 || king_col == col + 1) &&											// Left or right
			((king_row == row + 1 && Pieces::Black_Pawn_1 <= id && id <= Pieces::Black_Pawn_8) ||	// Black pawn - eat one row down or
			(king_row == row - 1 && Pieces::White_Pawn_1 <= id && id <= Pieces::White_Pawn_8)))		// White pawn - eat one row up
		{
			return true;
		}
	}
	else
		LOG_ERROR(LOGS::L_INVALID_POINTER, "Shouldn't get here");

	return false;
} // Pawn_CanEatKingInSquare


bool Rook_CanMove(int this_index, const Board& board)
{
	const Pieces this_ID = board.m_Board[this_index]->GetPieceID();
	int king_pos = INVALID;

	// Black piece - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White piece - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

	// Try to move by one in row or col
	const int dest1 = this_index - 1;
	const int dest2 = this_index + 1;
	const int dest3 = this_index - BOARD_SIDE;
	const int dest4 = this_index + BOARD_SIDE;
	const size_t max = board.m_Board.size();
	if ((0 <= dest1 && dest1 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest1, false, king_pos) != CANNOT_MOVE) ||
		(0 <= dest2 && dest2 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest2, false, king_pos) != CANNOT_MOVE) ||
		(0 <= dest3 && dest3 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest3, false, king_pos) != CANNOT_MOVE) ||
		(0 <= dest4 && dest4 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest4, false, king_pos) != CANNOT_MOVE))
	{
		return true;
	}
	return false;
} // Rook_CanMove


bool Knight_CanMove(int this_index, const Board& board)
{
	const int row = this_index / BOARD_SIDE;
	const int col = this_index % BOARD_SIDE;
	const Pieces this_ID = board.m_Board[this_index]->GetPieceID();
	int king_pos = INVALID;

	// Black piece - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White piece - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

	// Eight possible positions
	short delta[2] = { 1,  2 };
	int dest, dest1, dest2;

	for (int i = 0; i < 4; ++i)
	{
		// One horizontal and two vertical
		dest1 = row + delta[1];
		dest2 = col + delta[0];

		// Skip out-of board moves and same square
		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			dest = dest1 * BOARD_SIDE + dest2;
			if (CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest, false, king_pos) != CANNOT_MOVE)
				return true;
		}

		// Two horizontal and one vertical
		dest1 = row + delta[0];
		dest2 = col + delta[1];

		if (0 <= dest1 && dest1 < BOARD_SIDE && 0 <= dest2 && dest2 < BOARD_SIDE)
		{
			dest = dest1 * BOARD_SIDE + dest2;
			if (CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest, false, king_pos) != CANNOT_MOVE)
				return true;
		}

		// Rotate by 90 degrees
		RotateBy90(delta);
	}

	return false;
} // Knight_CanMove


bool Bishop_CanMove(int this_index, const Board& board)
{
	const Pieces this_ID = board.m_Board[this_index]->GetPieceID();
	int king_pos = INVALID;

	// Black piece - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White piece - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

	// Try to move by one in each diagonal
	const int dest1 = this_index - BOARD_SIDE - 1;
	const int dest2 = this_index - BOARD_SIDE + 1;
	const int dest3 = this_index + BOARD_SIDE - 1;
	const int dest4 = this_index + BOARD_SIDE + 1;
	const size_t max = board.m_Board.size();
	if ((0 <= dest1 && dest1 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest1, false, king_pos) != CANNOT_MOVE) ||
		(0 <= dest2 && dest2 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest2, false, king_pos) != CANNOT_MOVE) ||
		(0 <= dest3 && dest3 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest3, false, king_pos) != CANNOT_MOVE) ||
		(0 <= dest4 && dest4 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest4, false, king_pos) != CANNOT_MOVE))
	{
		return true;
	}
	return false;
} // Bishop_CanMove


bool Queen_CanMove(int this_index, const Board& board)
{
	return Bishop_CanMove(this_index, board) || Rook_CanMove(this_index, board);
} // Queen_CanMove


bool King_CanMove(int this_index, const Board& board)
{
	const Pieces this_ID = board.m_Board[this_index]->GetPieceID();

	// Try to move by one in each diagonal
	const int dest1 = this_index - BOARD_SIDE - 1;
	const int dest2 = this_index - BOARD_SIDE + 1;
	const int dest3 = this_index + BOARD_SIDE - 1;
	const int dest4 = this_index + BOARD_SIDE + 1;
	const int dest5 = this_index - 1;
	const int dest6 = this_index + 1;
	const int dest7 = this_index - BOARD_SIDE;
	const int dest8 = this_index + BOARD_SIDE;
	const size_t max = board.m_Board.size();

	board_t b1 = board.m_Board; 
	board_t b2 = board.m_Board;
	board_t b3 = board.m_Board; 
	board_t b4 = board.m_Board; 
	board_t b5 = board.m_Board;
	board_t b6 = board.m_Board; 
	board_t b7 = board.m_Board; 
	board_t b8 = board.m_Board; 

	if ((0 <= dest1 && dest1 < max && b1[dest1] == nullptr && (b1[dest1] = b1[this_index], b1[this_index] = nullptr, !CheckHelper(b1, dest1))) ||
		(0 <= dest2 && dest2 < max && b2[dest2] == nullptr && (b2[dest2] = b2[this_index], b2[this_index] = nullptr, !CheckHelper(b2, dest2))) ||
		(0 <= dest3 && dest3 < max && b3[dest3] == nullptr && (b3[dest3] = b3[this_index], b3[this_index] = nullptr, !CheckHelper(b3, dest3))) ||
		(0 <= dest4 && dest4 < max && b4[dest4] == nullptr && (b4[dest4] = b4[this_index], b4[this_index] = nullptr, !CheckHelper(b4, dest4))) ||
		(0 <= dest5 && dest5 < max && b5[dest5] == nullptr && (b5[dest5] = b5[this_index], b5[this_index] = nullptr, !CheckHelper(b5, dest5))) ||
		(0 <= dest6 && dest6 < max && b6[dest6] == nullptr && (b6[dest6] = b6[this_index], b6[this_index] = nullptr, !CheckHelper(b6, dest6))) ||
		(0 <= dest7 && dest7 < max && b7[dest7] == nullptr && (b7[dest7] = b7[this_index], b7[this_index] = nullptr, !CheckHelper(b7, dest7))) ||
		(0 <= dest8 && dest8 < max && b8[dest8] == nullptr && (b8[dest8] = b8[this_index], b8[this_index] = nullptr, !CheckHelper(b8, dest8))))
	{
		return true;
	}
	return false;
} // King_CanMove


bool Pawn_CanMove(int this_index, const Board& board)
{
	const int row = this_index / BOARD_SIDE;
	const int col = this_index % BOARD_SIDE;
	const Pieces this_ID = board.m_Board[this_index]->GetPieceID();
	int king_pos = INVALID;
	int row_increment = INVALID;

	// Black pawn - find black king
	if (Pieces::Black_Rook_1 <= this_ID && this_ID <= Pieces::Black_Pawn_8)
	{
		row_increment = BOARD_SIDE; // Black pawns go down
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::Black_King)
			{
				king_pos = i;
				break;
			}
		}
	}
	// White pawn - find white king
	else if (Pieces::White_Rook_1 <= this_ID && this_ID <= Pieces::White_Pawn_8)
	{
		row_increment = -BOARD_SIDE; // White pawns go down
		for (int i = 0; i < board.m_Board.size(); ++i)
		{
			if (board.m_Board[i] != nullptr && board.m_Board[i]->GetPieceID() == Pieces::White_King)
			{
				king_pos = i;
				break;
			}
		}
	}

	const int dest1 = this_index + row_increment;			// Normal move
	const int dest2 = this_index + row_increment - 1;		// Eat left
	const int dest3 = this_index + row_increment + 1;		// Eat right
	const int dest4 = this_index + (row_increment * 2);		// Super sprint
	const size_t max = board.m_Board.size();
	if ((0 <= dest1 && dest1 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest1, false, king_pos) == CAN_MOVE) || 
		(0 <= dest2 && dest2 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest2, false, king_pos) == FOUND_ENEMY) ||
		(0 <= dest3 && dest3 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest3, false, king_pos) == FOUND_ENEMY) ||
		(0 <= dest4 && dest4 < max && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, dest4, false, king_pos) == CAN_MOVE) ||
		(board.m_EnPassantIndex != INVALID && CanPieceMoveToSquare(board.m_Board, this_ID, this_index, board.m_EnPassantIndex, false, king_pos) == CAN_MOVE)) // En passant
	{
		return true;
	}
	return false;
} // Pawn_CanMove


bool IsKingUnderCheck(const board_t& board, int king_pos)
{
	return CheckHelper(board, king_pos);
} // IsKingUnderCheck


bool CanPlayerMove(bool player, const Board& board)
{
	Pieces start_id, end_id;
	if (player)
	{
		start_id = Pieces::White_Rook_1;
		end_id = Pieces::White_Pawn_8;
	}
	else
	{
		start_id = Pieces::Black_Rook_1;
		end_id = Pieces::Black_Pawn_8;
	}

	// TODO: change this to something faster, this can be really slow
	for (int i = 0; i < board.m_Board.size(); ++i)
	{
		if (board.m_Board[i] && board.m_Board[i]->IsAlive())
		{
			Pieces id = board.m_Board[i]->GetPieceID();
			if (start_id <= id && id <= end_id && board.m_Board[i]->CanMove(i, board))
				return true;
		}
	}

	return false;
} // CanPlayerMove


#undef FOUND_ENEMY
#undef CAN_MOVE
#undef CANNOT_MOVE






 // Old CheckHelper
#if 0
static bool CheckHelper(const board_t& board, int king_pos)
{
	// King can be checked on rows, columns, diagonals and L squares
	bool enemy_found = false;
	// TODO: bool shield_found = false;

	const int row = king_pos / BOARD_SIDE;
	const int col = king_pos % BOARD_SIDE;
	const int row_index = row * BOARD_SIDE;

	// Find out which king are we going to protect
	int i, j;
	Pieces id;
	Pieces enemy_rook;
	Pieces enemy_bishop;
	Pieces enemy_knight;
	Pieces enemy_queen;
	Pieces enemy_king;
	Pieces enemy_pawn;

	if (board[king_pos]->GetPieceID() == Pieces::White_King)
	{
		enemy_rook		= Pieces::Black_Rook_1;
		enemy_knight	= Pieces::Black_Knight_1;
		enemy_bishop	= Pieces::Black_Bishop_1;
		enemy_queen		= Pieces::Black_Queen;
		enemy_king		= Pieces::Black_King;
		enemy_pawn		= Pieces::Black_Pawn_1;
	}
	else if (board[king_pos]->GetPieceID() == Pieces::Black_King)
	{
		enemy_rook		= Pieces::White_Rook_1;
		enemy_knight	= Pieces::White_Knight_1;
		enemy_bishop	= Pieces::White_Bishop_1;	
		enemy_queen		= Pieces::White_Queen;
		enemy_king		= Pieces::White_King;
		enemy_pawn		= Pieces::White_Pawn_1;
	}
	else
	{
		LOG_ERROR(LOGS::L_INVALID_PARAMETER, "Didn't select a king");
		return false;
	}


	// Check first part of king's row - only queens
	for (i = row_index; i < row_index + col; ++i)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i] != NULL)
		{
			id = board[i]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), row, i, "On king's row", false);
				enemy_found = true;
			}
			else 
			{
				DebugHelper(board[king_pos]->GetPieceID(), row, i, "Shield", false);
				enemy_found = true; // Found a friend and a shield
			}

			break;
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;

	// Check second part of king's row
	for (i = row_index + BOARD_SIDE - 1; i > row_index + col; --i)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i] != NULL)
		{
			id = board[i]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), row, i, "On king's row", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), row, i, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;


	// Check first part of king's col
	for (i = col; i < row_index; i += BOARD_SIDE)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i] != NULL)
		{
			id = board[i]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), i / BOARD_SIDE, col, "On king's col", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), i / BOARD_SIDE, col, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;

	// Check second part of king's col
	for (i = board.size() - BOARD_SIDE + col + 1; i > row_index; i -= BOARD_SIDE)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i] != NULL)
		{
			id = board[i]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), i / BOARD_SIDE, col, "On king's col", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), i / BOARD_SIDE, col, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;


	// Check first part of king's first diagonal
	for (i = row + 1, j = col + 1; i < BOARD_SIDE && j < BOARD_SIDE; ++i, ++j)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i * BOARD_SIDE + j] != NULL)
		{
			id = board[i * BOARD_SIDE + j]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "On king's first diag", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;

	// Check second part of king's first diagonal
	for (i = row - 1, j = col - 1; i >= 0 && j >= 0; --i, --j)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i * BOARD_SIDE + j] != NULL)
		{
			id = board[i * BOARD_SIDE + j]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "On king's first diag", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;


	// Check first part of king's second diagonal
	for (i = row + 1, j = col - 1; i < BOARD_SIDE && j >= 0; ++i, --j)
	{
		// If an enemy is found, look if its path to the king is clear
		if (board[i * BOARD_SIDE + j] != NULL)
		{
			id = board[i * BOARD_SIDE + j]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "On king's second diag", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;

	// Check second part of king's second diagonal
	for (i = row - 1, j = col + 1; i >= 0 && j < BOARD_SIDE; --i, ++j)
	{
		DebugHelper(board[king_pos]->GetPieceID(), i, j, "On king's second diag", false);

		// If an enemy is found, look if its path to the king is clear
		if (board[i * BOARD_SIDE + j] != NULL)
		{
			id = board[i * BOARD_SIDE + j]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "On king's second diag", false);
				enemy_found = true;
			}
			else
			{
				DebugHelper(board[king_pos]->GetPieceID(), i, j, "Shield", false);
				enemy_found = false; // Found a friend and a shield
			}
		}
	}
	if (enemy_found) // Even a single check is enough
		return true;


	// Check for knight autism - there's no shield against it
	short delta[2] = { 1,  2 };
	int knigth, knigth1, knigth2;
	for (int i = 0; i < 4; ++i)
	{
		// One horizontal and two vertical
		knigth1 = row + delta[1];
		knigth2 = col + delta[0];
		knigth = knigth1 * BOARD_SIDE + knigth2;

		// If an enemy is found, look if its path to the king is clear
		if (0 <= knigth1 && knigth1 < BOARD_SIDE && 0 <= knigth2 && knigth2 < BOARD_SIDE && board[knigth] != NULL)
		{
			id = board[knigth]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), knigth1, knigth2, "By Knight", false);
				return true;
			}
		}

		// Two horizontal and one vertical
		knigth1 = row + delta[0];
		knigth2 = col + delta[1];
		knigth = knigth1 * BOARD_SIDE + knigth2;

		// If an enemy is found, look if its path to the king is clear
		if (0 <= knigth1 && knigth1 < BOARD_SIDE && 0 <= knigth2 && knigth2 < BOARD_SIDE && board[knigth] != NULL)
		{
			id = board[knigth]->GetPieceID();
			if (start_enemy_id <= id && id <= stop_enemy_id)
			{
				DebugHelper(board[king_pos]->GetPieceID(), knigth1, knigth2, "By Knight", false);
				return true;
			}
		}

		// Rotate by 90 degrees
		RotateBy90(delta);
	}

	return false;
} // CheckHelper
#endif


// Old bishop algorithm
#if 0
// 
	// 
	//// First diagonal
	//int start_row = std::min(row, king_row) + 1;
	//int start_col = std::min(col, king_col) + 1;
	//int stop_row = std::max(row, king_row);
	//int stop_col = std::max(col, king_col);

	//int i, j;
	//for (i = start_row, j = start_col; i <= stop_row && j <= stop_col; ++i, ++j)
	//{
	//	if (board[i * BOARD_SIDE + j] != NULL) // Found a piece
	//		break;
	//}

	//if (i == stop_row && j == stop_col) // King was reached
	//	return true;

	//// Second diagonal
	//start_row = std::min(row, king_row) + 1;
	//start_col = std::max(col, king_col) - 1;
	//stop_row = std::max(row, king_row);
	//stop_col = std::min(col, king_col);

	//for (i = start_row, j = start_col; i <= stop_row && j >= stop_col; ++i, --j)
	//{
	//	if (board[i * BOARD_SIDE + j] != NULL) // Found a piece
	//		break;
	//}

	//if (i == stop_row && j == stop_col) // King was reached
	//	return true;
#endif
