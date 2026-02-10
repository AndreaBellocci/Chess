#pragma once

#include "Types.h"
#include "Pieces.h"
#include "Chess.h"
#include "Pieces/piece_utilities.h"
#include "ChessEvents.h"
#include "ChessApp.h"
#include "ErrorCodes.h"

#include "../Libraries/EngineUtility.h"
#include "../Libraries/EventManager.h"
#include "../Libraries/Processes.h"
#include <string>
using namespace ENGINE_NAMESPACE;


ChessGame::ChessGame(const std::string& xml_settings_file)
	: m_privateEventManager(false)
{
	// Reset board
	for (size_t i = 0; i < this->m_game.m_Board.size(); ++i)
		this->m_game.m_Board[i] = nullptr;

	// Create pieces
	this->m_game.m_Pieces = ChessApp::SetupPieces(xml_settings_file);

	// TODO: Read FEN
	this->PrepareGame();
} // ChessGame

static inline Piece* FENHelper(const piece_list& pieces, 
	Pawn::Promotions promotion,
	Pieces start_id, Pieces promote_start_id,
	int& count, int& promoted_count,
	int max,
	const char* color, const char* type)
{
	if (promotion == Pawn::Promotions::Null)
	{
		if (count >= max)
		{
			THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Too many %s %s!", color, type);
		}

		// Return normal piece
		return pieces[static_cast<size_t>(start_id) + count++].get();
	}
	else
	{
		// At most 8 pawns per color can ever exist
		if (promoted_count >= 8)
		{
			THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Too many %s Pawns!", color);
		}

		// Return promoted piece - it's safe to use static_cast because pieces inside the pieces lists can't be reordered
		Pawn* promoted_pawn = static_cast<Pawn*>(pieces[static_cast<size_t>(promote_start_id) + promoted_count++].get());
		promoted_pawn->Promote(promotion);
		return promoted_pawn;
	}
} // FENHelper


void ChessGame::PrepareGame(const std::string& fen_string)
{
	// See notes on StoreFENString
	int row = 0;
	int col = 0;

	// Black pieces counters
	int n_black_pawns = 0;
	int n_black_knights = 0;
	int n_black_bishops = 0;
	int n_black_rooks = 0;
	int n_black_queens = 0;
	int n_black_kings = 0;

	// White pieces counters
	int n_white_pawns = 0;
	int n_white_knights = 0;
	int n_white_bishops = 0;
	int n_white_rooks = 0;
	int n_white_queens = 0;
	int n_white_kings = 0;

	// TODO: it would be better to use standard FEN strings, which don't have the $ sign to indicate promoted pieces.
	// This would require post processing, as the castling logic allows castling only with pieces with Rook ids
	size_t index;

	// First field: start rank 8 and end with rank 1;
	Piece* piece = nullptr;
	bool next_promoted = false;
	Pawn::Promotions promotion = Pawn::Promotions::Null;

	// FIRST FIELD: PIECE PLACEMENT
	for (index = 0; fen_string.at(index) != ChessGame::FEN_FIELD_DELIMITER; ++index)
	{
		const char current = fen_string.at(index);

		// Process char
		switch (current)
		{
		case ChessGame::FEN_NEXT_PROMOTED:
			next_promoted = true;
			break;

			// Black pieces
		case ChessGame::FEN_BLACK_ROOK:	  if (next_promoted) { promotion = Pawn::Promotions::Rook;   next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::Black_Rook_King_Side, Pieces::Black_Pawn_1, n_black_rooks,   n_black_pawns, 2, "Black", "Rooks");   break;
		case ChessGame::FEN_BLACK_KNIGHT: if (next_promoted) { promotion = Pawn::Promotions::Knight; next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::Black_Knight_1,		Pieces::Black_Pawn_1, n_black_knights, n_black_pawns, 2, "Black", "Knights"); break;
		case ChessGame::FEN_BLACK_BISHOP: if (next_promoted) { promotion = Pawn::Promotions::Bishop; next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::Black_Bishop_1,		Pieces::Black_Pawn_1, n_black_bishops, n_black_pawns, 2, "Black", "Bishops"); break;
		case ChessGame::FEN_BLACK_QUEEN:  if (next_promoted) { promotion = Pawn::Promotions::Queen;  next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::Black_Queen,			Pieces::Black_Pawn_1, n_black_queens,  n_black_pawns, 1, "Black", "Queens");  break;

		case ChessGame::FEN_BLACK_KING:
			if (n_black_kings >= 1)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Too many Black kings!");
			}
			if (next_promoted)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Pawns cannot be promoted to kings.");
			}
			piece = this->m_game.m_Pieces[static_cast<size_t>(Pieces::Black_King) + n_black_kings++].get();
			break;

		case ChessGame::FEN_BLACK_PAWN:
			if (n_black_pawns >= 8)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Too many Black pawns!");
			}
			if (next_promoted)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Pawns cannot be promoted to pawns.");
			}
			piece = this->m_game.m_Pieces[static_cast<size_t>(Pieces::Black_Pawn_1) + n_black_pawns++].get();
			break;

			// White pieces
		case ChessGame::FEN_WHITE_ROOK:	  if (next_promoted) { promotion = Pawn::Promotions::Rook;   next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::White_Rook_King_Side, Pieces::White_Pawn_1, n_white_rooks,   n_white_pawns, 2, "White", "Rooks");   break;
		case ChessGame::FEN_WHITE_KNIGHT: if (next_promoted) { promotion = Pawn::Promotions::Knight; next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::White_Knight_1,		Pieces::White_Pawn_1, n_white_knights, n_white_pawns, 2, "White", "Knights"); break;
		case ChessGame::FEN_WHITE_BISHOP: if (next_promoted) { promotion = Pawn::Promotions::Bishop; next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::White_Bishop_1,		Pieces::White_Pawn_1, n_white_bishops, n_white_pawns, 2, "White", "Bishops"); break;
		case ChessGame::FEN_WHITE_QUEEN:  if (next_promoted) { promotion = Pawn::Promotions::Queen;  next_promoted = false; } piece = FENHelper(this->m_game.m_Pieces, promotion, Pieces::White_Queen,			Pieces::White_Pawn_1, n_white_queens,  n_white_pawns, 1, "White", "Queens");  break;

		case ChessGame::FEN_WHITE_KING:
			if (n_white_kings >= 1)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Too many White kings!");
			}
			if (next_promoted)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Pawns cannot be promoted to kings.");
			}
			piece = this->m_game.m_Pieces[static_cast<size_t>(Pieces::White_King) + n_white_kings++].get();
			break;

		case ChessGame::FEN_WHITE_PAWN:
			if (n_white_pawns >= 8)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Too many White pawns!");
			}
			if (next_promoted)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Pawns cannot be promoted to pawns.");
			}
			piece = this->m_game.m_Pieces[static_cast<size_t>(Pieces::White_Pawn_1) + n_white_pawns++].get();
			break;

			// Rank delimiters
		case ChessGame::FEN_RANK_DELIMITER:
			if (next_promoted)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Promotion token must be followed by a piece in the same rank.");
			}

			// Go to start of next row
			row++;
			col = 0;
			break;

			// Generic chars. Numbers indicate the number of empty cells, but all other chars are invalid
		default:
			if (next_promoted)
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Promotion token must be followed by a valid piece.");
			}

			if ('1' <= current && current <= '8') // At most 8 empty cells can be in a row, and zero empty cells makes no sense
			{
				for (int i = 0; i < current - '0'; i++)
				{
					this->m_game.m_Board[row * BOARD_SIDE + col] = nullptr;
					col++;
				}
			}
			else
			{
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: unrecognised token %c", current);
			}
		}

		// If there's a piece to place on the board, place if
		if (piece)
		{
			const auto piece_pos = row * BOARD_SIDE + col;
			this->m_game.m_Board[piece_pos] = piece;
			piece->Reset(static_cast<int>(piece_pos));

			// Move to the next column in the same row
			piece = nullptr;
			col++;
		}
	} // for - first field

	// SECOND FIELD: TURN
	// Check that all 8 rows were processed and the kings are present
	if (row != 7)
	{
		THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: wrong number of rows");
	}
	if (n_black_kings != 1 || n_white_kings != 1)
	{
		THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: there must be exactly one king per color");
	}

	// Turn indicator is right after the first empty space
	index++;
	this->m_game.m_turn = fen_string.at(index) == ChessGame::FEN_WHITE;
	index++;
	if (fen_string.at(index) != ChessGame::FEN_FIELD_DELIMITER)
	{
		THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: missing fields");
	}

	// THIRD FIELD: CASTLING INFORMATION
	index++;
	if (fen_string.at(index) == ChessGame::FEN_EMPTY_FIELD)
	{
		// No castling available for either player
		this->m_game.m_can_white_castle = false;
		this->m_game.m_can_black_castle = false;
		this->m_game.m_can_white_castle_long = false;
		this->m_game.m_can_black_castle_long = false;
		index++;
		if (fen_string.at(index) != ChessGame::FEN_FIELD_DELIMITER)
		{
			THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: missing fields");
		}
	}
	else
	{
		// Enable castling rights according to the presence of the corresponding chars in the FEN string
		char c = fen_string.at(index);
		int n = 0;
		do
		{
			n++;
			switch (c)
			{
				// Normal chess castling notation
			case 'K': this->m_game.m_can_white_castle = true;		break;
			case 'k': this->m_game.m_can_black_castle = true;		break;
			case 'Q': this->m_game.m_can_white_castle_long = true;	break;
			case 'q': this->m_game.m_can_black_castle_long = true;	break;

				// TODO: process Chess960 castling notation, which uses the letters of the columns on which the rooks began the game
				// instead of KQkq (e.g.: HAha instead of KQkq). I fear this requires a more significant refactor of the castling logic.

			default:
				THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: unrecognised token for castling");
			}

			index++;
			c = fen_string.at(index);
		} while (c != ChessGame::FEN_FIELD_DELIMITER);

		if (n > 4)
		{
			THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: too many castling rights specified");
		}
	}

	// FOURTH FIELD: EN PASSANT TARGET SQUARE
	index++;
	if (fen_string.at(index) == ChessGame::FEN_EMPTY_FIELD)
	{
		this->m_game.m_en_passant_target_pos = INVALID;
		index++;
	}
	else
	{
		// e.g: e3
		const int col = fen_string.at(index++) - 'a'; // e => col = 4 = 'e' - 'a'
		const int row = BOARD_SIDE - (fen_string.at(index++) - '0');	// 3 => row = 5 = 8 - 3 = BOARD_SIDE - ('3' - '0')

		// Make sure the position is valid
		if (col < 0 || col >= BOARD_SIDE || row < 0 || row >= BOARD_SIDE)
		{
			THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: invalid en passant target square");
		}
		this->m_game.m_en_passant_target_pos = row * BOARD_SIDE + col;
	}

	// FIFTH FIELD: HALF MOVE CLOCK
	if (fen_string.at(index) != ChessGame::FEN_FIELD_DELIMITER)
	{
		THROW_CHESS_EXCEPTION(ErrorCode::DataCorrupted, "Incorrect FEN string: missing fields");
	}
	index++;
	const auto half_end = fen_string.find(ChessGame::FEN_FIELD_DELIMITER, index);
	this->m_game.m_half_move_clock = std::stoi(fen_string.substr(index, half_end - index));

	// SIXTH FIELD: FULL MOVE NUMBER
	this->m_game.m_full_move_number = std::stoi(fen_string.substr(half_end + 1));
} // PrepareGame


void ChessGame::OnPieceSelected(const IEventPtr& pEvent)
{
	// Get event
	const Event_PieceSelected& event = *static_cast<Event_PieceSelected*>(pEvent.get());
	Piece* selected = this->m_game.m_Board[event.m_Index];

	if (selected)
	{
		// Only process pieces belonging to the current player
		const Pieces id = selected->GetPieceID();

		if ((this->m_game.m_turn == false && isBlack(id)) || // Black player
			(this->m_game.m_turn == true  && isWhite(id)))   // White player
		{
			// Build tree of possible moves
			selected->BuildPossibleMoves(this->m_game, this->m_AllowChecks);
		}
		else
		{
			// Clear selection if opponent's piece was selected
			this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_SelectionReset()));
		}
	}
	else
	{
		THROW_CHESS_EXCEPTION(ErrorCode::InvalidSelection, "Selected cell %d is empty", event.m_Index);
	}
} // OnPieceSelected Listener


#if _DEBUG
void ChessGame::OnSelectionReset(const IEventPtr& pEvent)
{
	printf("Selection reset\n");
} // OnSelectionReset Listener


void ChessGame::OnIllegalMove(const IEventPtr& pEvent)
{
	printf("Illegal move\n");
} // OnIllegalMove Listener
#endif


//void ChessGame::OnStartMovePiece(const IEventPtr& pEvent)
//{
//	const Event_StartMovePiece& event = *static_cast<Event_StartMovePiece*>(pEvent.get());
//	const Pieces piece_id = event.m_Piece->GetPieceID();
//
//	// Look for king castling
//	if ((piece_id == Pieces::Black_King && (this->m_game.m_can_black_castle || this->m_game.m_can_black_castle_long)) || 
//		(piece_id == Pieces::White_King && (this->m_game.m_can_white_castle || this->m_game.m_can_white_castle_long)))
//	{
//		// Find and move also the corresponding rook
//		Pieces rook_id = Pieces::Null;
//		int rook_curr_pos = INVALID;
//		Piece* rook = nullptr;
//
//		// Black Queen-side castle: king in c8 (i.e.: index = 2)  and rook in d8 (i.e.: index = 3)
//		// Black King-side  castle: king in g8 (i.e.: index = 6)  and rook in f8 (i.e.: index = 5)	
//		// White Queen-side castle: king in c1 (i.e.: index = 58) and rook in d1 (i.e.: index = 59)
//		// White King-side  castle: king in g1 (i.e.: index = 62) and rook in f1 (i.e.: index = 61)		
//		switch (event.m_Dest)
//		{
//		case 2:	 rook_curr_post = 3;	 rook_id = Pieces::Black_Rook_King_Side;  rook = Piece::GetSet()[(size_t)rook_id].get();	break;
//		case 6:	 rook_curr_post = 5;	 rook_id = Pieces::Black_Rook_Queen_Side; rook = Piece::GetSet()[(size_t)rook_id].get();	break;
//		case 58: rook_curr_post = 59; rook_id = Pieces::White_Rook_King_Side;  rook = Piece::GetSet()[(size_t)rook_id].get();	break;
//		case 62: rook_curr_post = 61; rook_id = Pieces::White_Rook_Queen_Side; rook = Piece::GetSet()[(size_t)rook_id].get();	break;
//		default: goto Next; // Not castling
//		}
//
//		if (rook->IsAlive())
//		{
//			// Find rook on board
//			int where_rook;
//			for (where_rook = 0; ; ++where_rook) // If the rook's alive, it must be on the board, so it isn't necessary to check
//				if (this->m_game.m_Board[where_rook] != nullptr && this->m_game.m_Board[where_rook]->GetPieceID() == rook_id)
//					break;
//
//			// Prevent player from castling again
//			if (piece_id == Pieces::Black_King)
//				this->m_game.m_Black_Castle = this->m_game.m_Black_CastleLong = false;
//			else
//				this->m_game.m_White_Castle = this->m_game.m_White_CastleLong = false;
//
//			// Move the rook
//			this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_Castle(rook, where_rook, rook_index)));
//		}
//	}
//	else if ((Pieces::Black_Pawn_1 <= piece_id && piece_id <= Pieces::Black_Pawn_8) ||  // Look for possible en passants 
//		(Pieces::White_Pawn_1 <= piece_id && piece_id <= Pieces::White_Pawn_8))
//	{
//		// If capturing en passant
//		if (this->m_game.m_en_passant_target_pos == event.m_Dest)
//		{
//			if (Pieces::White_Pawn_1 <= piece_id) // White pawn eat up so look for captured pawn down
//				this->m_privateEventManager.TriggerEvent(IEventPtr(
//					new Event_PieceEaten(this->m_game.m_Board[this->m_game.m_en_passant_target_pos + BOARD_SIDE])));
//			else // Black pawn eat down so look for captured pawn up
//				this->m_privateEventManager.TriggerEvent(IEventPtr(
//					new Event_PieceEaten(this->m_game.m_Board[this->m_game.m_en_passant_target_pos - BOARD_SIDE])));
//
//			this->m_game.m_en_passant_target_pos = INVALID;
//		}
//		else // Super sprint
//		{
//			const int start_row = event.m_Src / BOARD_SIDE;
//			const int end_row = event.m_Dest / BOARD_SIDE;
//
//			if (start_row - end_row == 2 || start_row - end_row == -2)
//			{
//				// Store the position "behind" the pawn
//				this->m_game.m_en_passant_target_pos = ((start_row + end_row) / 2) * BOARD_SIDE + (event.m_Dest % BOARD_SIDE);
//			}
//			else
//				this->m_game.m_en_passant_target_pos = INVALID;
//		}
//	}
//	else // Every other move reset en passant index
//		this->m_game.m_en_passant_target_pos = INVALID;
//
//Next:
//	this->m_game.m_Board[event.m_Src] = nullptr; // Piece is not anymore there
//	this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_EndMovePiece(event.m_Piece, event.m_Dest)));
//} // OnStartMovePiece Listener
//
//void ChessGame::OnCastle(const IEventPtr& pEvent)
//{
//	const Event_Castle& event = *static_cast<Event_Castle*>(pEvent.get());
//
//	// Update board 
//	this->m_game.m_Board[event.m_Src] = nullptr; // Piece is not anymore there
//	this->m_game.m_Board[event.m_Dest] = event.m_Piece;
//} // OnCastle
//
//void ChessGame::OnEndMovePiece(const IEventPtr& pEvent)
//{
//	const Event_EndMovePiece& event = *static_cast<Event_EndMovePiece*>(pEvent.get());
//	const Pieces id = event.m_Piece->GetPieceID();
//
//	// If a piece was eaten, fire an event
//	if (this->m_game.m_Board[event.m_Dest] != nullptr)
//	{
//		this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_PieceEaten(this->m_game.m_Board[event.m_Dest])));
//		this->m_Half_Moves = 0;
//	}
//	else
//	{
//		if (!(Pieces::White_Pawn_1 <= id && id <= Pieces::White_Pawn_8) &&
//			!(Pieces::Black_Pawn_1 <= id && id <= Pieces::Black_Pawn_8))
//		{
//			// Increment number of half moves after each move that didn't involve a pawn
//			this->m_Half_Moves++;
//		}
//	}
//
//	// If a pawn needs to be promoted, fire an event
//	if ((this->m_game->m_turn == true &&										// White turn and
//		Pieces::White_Pawn_1 <= id && id <= Pieces::White_Pawn_8 &&	// White pawn and
//		event.m_Dest / BOARD_SIDE == 0) ||							// On first rank	or
//		(this->m_game->m_turn == false &&										// Black turn and
//			Pieces::Black_Pawn_1 <= id && id <= Pieces::Black_Pawn_8 && // Black pawn and
//			event.m_Dest / BOARD_SIDE == BOARD_SIDE - 1))				// On last rank
//	{
//		auto pawn = static_cast<Pawn*>(event.m_Piece);
//
//		if (!pawn->HasBeenPromoted())
//			this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_PawnPromotion(pawn)));
//	}
//
//	// Update board 
//	this->m_game.m_Board[event.m_Dest] = event.m_Piece;
//
//	this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_EndTurn()));
//} // OnEndMovePiece Listener
//
//void ChessGame::OnEndTurn(const IEventPtr& pEvent)
//{
//	// Reset selection
//	this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_SelectionReset()));
//
//	// Print board and piece status for debug purpose
//#if DEBUGGING
//	PrintBoardOnTerminal(this->m_game.m_Board);
//	const auto& set = Piece::GetSet();
//	int dead = 0, alive = 0;
//	for (int i = 0; i < set.size(); ++i)
//	{
//		if (set[i]->IsAlive())
//		{
//			printf("%s is alive\n", set[i]->GetPieceName().c_str());
//			++alive;
//		}
//		else
//		{
//			printf("%s is dead\n", set[i]->GetPieceName().c_str());
//			++dead;
//		}
//	}
//
//	printf("Dead pieces: %d Alive: %d\n\n\n", dead, alive);
//#endif
//
//	// If king is now under check
//	int where_king = INVALID;
//	if (this->m_game->m_turn) // Find black king
//	{
//		for (int i = 0; i < this->m_game.m_Board.size(); ++i)
//			if (this->m_game.m_Board[i] && this->m_game.m_Board[i]->GetPieceID() == Pieces::Black_King)
//			{
//				where_king = i; break;
//			}
//	}
//	else // Find white king
//	{
//		for (int i = 0; i < this->m_game.m_Board.size(); ++i)
//			if (this->m_game.m_Board[i] && this->m_game.m_Board[i]->GetPieceID() == Pieces::White_King)
//			{
//				where_king = i; break;
//			}
//	}
//	Assert(where_king != INVALID && "King not on board");
//
//	if (IsKingUnderCheck(this->m_game.m_Board, where_king))
//	{
//		if (CanPlayerMove(!this->m_game->m_turn, this->m_game)) //  Player didn't lose, fire an event
//			this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_Check(where_king)));
//		else // Player lost
//			this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_EndMatch((Event_EndMatch::END_STATUS)this->m_game->m_turn)));
//	}
//	else if (!CanPlayerMove(!this->m_game->m_turn, this->m_game) || this->m_Half_Moves >= 50) // Stalemate or fifty moves rule
//	{
//		this->m_privateEventManager.TriggerEvent(IEventPtr(new Event_EndMatch(Event_EndMatch::END_STATUS::Stalemate)));
//	}
//
//	// Increment number of full moves after each black move
//	if (!this->m_game->m_turn)
//		this->m_Full_Moves++;
//
//	// Change turn
//	this->m_game->m_turn = !this->m_game->m_turn;
//
//	printf("End turn\n");
//} // OnEndTurn Listener
//
//void ChessGame::OnPieceEaten(const IEventPtr& pEvent)
//{
//	const Event_PieceEaten& event = *static_cast<Event_PieceEaten*>(pEvent.get());
//
//	event.m_Piece->Die();
//	printf("Piece Eaten: %s\n", event.m_Piece->GetPieceName().c_str());
//} // OnPieceEaten Listener


std::string ChessGame::PrintGameStatus() const
{
	std::string ret;
	ret.reserve(2 * BOARD_SIDE * (BOARD_SIDE + 1)); // Each cell is two characters, and then there is one line feed per row

	for (int row = 0; row < BOARD_SIDE; row++)
	{
		for (int col = 0; col < BOARD_SIDE; col++)
		{
			const Piece* curr = this->m_game.m_Board[row * BOARD_SIDE + col];
			if (curr)
			{
				Pieces id = curr->GetPieceID();

				switch (id)
				{
				case Pieces::Black_Rook_King_Side:	case Pieces::Black_Rook_Queen_Side:	ret += ChessGame::FEN_BLACK_ROOK;   break;
				case Pieces::Black_Knight_1:		case Pieces::Black_Knight_2:		ret += ChessGame::FEN_BLACK_KNIGHT; break;
				case Pieces::Black_Bishop_1:		case Pieces::Black_Bishop_2:		ret += ChessGame::FEN_BLACK_BISHOP; break;
				case Pieces::Black_Queen:												ret += ChessGame::FEN_BLACK_QUEEN;	break;
				case Pieces::Black_King:												ret += ChessGame::FEN_BLACK_KING;	break;
				case Pieces::Black_Pawn_1: case Pieces::Black_Pawn_2: case Pieces::Black_Pawn_3: case Pieces::Black_Pawn_4:
				case Pieces::Black_Pawn_5: case Pieces::Black_Pawn_6: case Pieces::Black_Pawn_7: case Pieces::Black_Pawn_8:
					ret += ChessGame::FEN_BLACK_PAWN; break;

				case Pieces::White_Rook_King_Side:	case Pieces::White_Rook_Queen_Side:	ret += ChessGame::FEN_WHITE_ROOK;	break;
				case Pieces::White_Knight_1:		case Pieces::White_Knight_2:		ret += ChessGame::FEN_WHITE_KNIGHT; break;
				case Pieces::White_Bishop_1:		case Pieces::White_Bishop_2:		ret += ChessGame::FEN_WHITE_BISHOP; break;
				case Pieces::White_Queen:												ret += ChessGame::FEN_WHITE_QUEEN;	break;
				case Pieces::White_King:												ret += ChessGame::FEN_WHITE_KING;	break;
				case Pieces::White_Pawn_1: case Pieces::White_Pawn_2: case Pieces::White_Pawn_3: case Pieces::White_Pawn_4:
				case Pieces::White_Pawn_5: case Pieces::White_Pawn_6: case Pieces::White_Pawn_7: case Pieces::White_Pawn_8:
					ret += ChessGame::FEN_WHITE_PAWN; break;

				default:
					ret += '?';
				} // Switch
			}
			else
				ret += ' ';

			ret += ' ';
		} // Inner for

		ret += "\n";
	} // Outer for

	ret += "\nCurrent turn: " + std::string((this->m_game.m_turn ? "White" : "Black"));
	ret += "\nBlack castle (king side): "  + std::string(this->m_game.m_can_black_castle ? "true" : "false");
	ret += "\nBlack castle (queen side): " + std::string(this->m_game.m_can_black_castle_long ? "true" : "false");
	ret += "\nWhite castle (king side): "  + std::string(this->m_game.m_can_white_castle ? "true" : "false");
	ret += "\nWhite castle (queen side): " + std::string(this->m_game.m_can_white_castle_long ? "true" : "false");
	ret += "\nEn Passant target: " + std::to_string(this->m_game.m_en_passant_target_pos);
	ret += "\nHalf moves: " + std::to_string(this->m_game.m_half_move_clock);
	ret += "\nFull moves: " + std::to_string(this->m_game.m_full_move_number);
	ret += "\n";
	return ret;
} // PrintBoardOnTerminal