// Chicago, 7/04/2026
//	Completed on 8/04/2026

#pragma once
#include <array>

#include "Types.h"
#include "../Libraries/EngineUtility.h"
#include "../Libraries/EventManager.h"
#include "../Libraries/Processes.h"

// Struct Chess represents the state of a chess game, including the pieces and the board.
struct Chess
{
	board_t m_Board; // Current state of the board, with pointers to pieces or nullptr for empty squares
	piece_list m_Pieces; // List of all pieces, even those that have been captured

	bool m_can_white_castle			= true; // False if king or king's side rook have moved
	bool m_can_white_castle_long	= true; // False if king or queen's side rook have moved
	bool m_can_black_castle			= true; // False if king or king's side rook have moved
	bool m_can_black_castle_long	= true; // False if king or queen's side rook have moved

	bool m_turn = true; // True for white, false for black
	int m_en_passant_target_pos = INVALID; // Destination of current 'en passant' capture option, if any
	int m_half_move_clock  = 0;
	int m_full_move_number = 0;
}; // Chess


class ChessGame
{
public:
	ChessGame(const std::string& xml_settings_file);
	void Update();
	bool ShouldQuit() const noexcept { return this->m_should_quit; }

	// Event Listeners
	void OnPieceSelected(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnStartMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnCastle(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnEndMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnPawnPromotion(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnPieceEaten(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnEndTurn(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnEndMatch(const ENGINE_NAMESPACE::IEventPtr& pEvent);

	std::string PrintGameStatus() const; // For debug purposes, returns a string representation of the board and pieces on it

#if _DEBUG
	void OnSelectionReset(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnIllegalMove(const ENGINE_NAMESPACE::IEventPtr& pEvent);
#endif

	const piece_list& GetPieces() const noexcept { return this->m_game.m_Pieces; }
	const Chess& GetGameState() const noexcept { return this->m_game; }

	ENGINE_NAMESPACE::EventManager& GetGameEventManager() noexcept { return this->m_privateEventManager; }

private:
	Chess m_game; // Current state of the board and pieces

	bool m_should_quit = false;
	bool m_AllowChecks = false; // If false, then the app prevents players from leaving the king under check
	ENGINE_NAMESPACE::EventManager m_privateEventManager; // Event manager for this game, used to trigger events and register listeners

	bool CanNextPlayerMove() const;

	// We use FEN notation (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
	// to define every piece on the board and an empty space to indicate that no piece is in that position
	constexpr static const char* STARTING_FEN_STRING = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	static constexpr char FEN_WHITE_PAWN	= 'P';
	static constexpr char FEN_WHITE_KNIGHT	= 'N';
	static constexpr char FEN_WHITE_BISHOP	= 'B';
	static constexpr char FEN_WHITE_ROOK	= 'R';
	static constexpr char FEN_WHITE_QUEEN	= 'Q';
	static constexpr char FEN_WHITE_KING	= 'K';
	static constexpr char FEN_BLACK_PAWN	= 'p';
	static constexpr char FEN_BLACK_KNIGHT	= 'n';
	static constexpr char FEN_BLACK_BISHOP	= 'b';
	static constexpr char FEN_BLACK_ROOK	= 'r';
	static constexpr char FEN_BLACK_QUEEN	= 'q';
	static constexpr char FEN_BLACK_KING	= 'k';
	static constexpr char FEN_NEXT_PROMOTED = '$'; // Used in the FEN string to indicate that the following piece is a promoted pawn
	static constexpr char FEN_FIELD_DELIMITER = ' '; // Delimiter between fields in the FEN string
	static constexpr char FEN_RANK_DELIMITER = '/';  // Delimiter between ranks in the FEN string
	static constexpr char FEN_WHITE = 'w'; // Indicates white player in the FEN string
	static constexpr char FEN_BLACK = 'b'; // Indicates black player in the FEN string
	static constexpr char FEN_EMPTY_FIELD = '-'; // Indicates an empty field in the FEN string
	void PrepareGame(const std::string& fen_string = STARTING_FEN_STRING);
}; // ChessGame