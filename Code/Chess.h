// Milan, 6th December 2021
//

#pragma once
#include <array>
#include <memory>
#include <string>


#include "../Libraries/EventManager.h"
#include "../Libraries/Processes.h"

#include "Pieces.h"
#include "GameViews.h"


ENGINE_NAMESPACE::EventManager& GetEventManager();

struct Board
{
	board_t m_Board;
	bool m_White_Castle		: 1 = true;	// False if king or king's side rook have moved
	bool m_White_CastleLong : 1 = true;	// False if king or queen's side rook have moved
	bool m_Black_Castle		: 1 = true;	// False if king or king's side rook have moved
	bool m_Black_CastleLong : 1 = true;	// False if king or queen's side rook have moved

	int m_EnPassantIndex = INVALID;
};



// Class ChessGame - contains everything needed to play chess
class ChessGame
{
public:
	ChessGame(const std::string& initFileName);
	~ChessGame();

	void OnInitialize();
	void OnInput();
	void OnUpdate(ENGINE_NAMESPACE::nanoseconds delta);
	void OnRender();

	

private:
	// Event Listeners
	void OnSelectionReset(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnIllegalMove(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnPieceSelected(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnStartMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnCastle(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnEndMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnPawnPromotion(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnPieceEaten(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnEndTurn(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	

	// Utility
	void LoadFENString();
	void StoreFENString();
	Piece* MapCharToPiece(char c) const;
	char MapPieceIDToChar(Pieces id) const;

	static ENGINE_NAMESPACE::EventManager& s_GlobalEventManager;
	
	Board m_Board;
	PossibleMovesIndeces m_Targets;

	int m_Half_Moves = 0;
	int m_Full_Moves = 0;

	const std::string m_InitFile;

	std::string m_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	bool m_Turn : 1 = true;			// True for white, false for black
	bool m_AllowChecks : 1 = false;	// If false, then the app prevents players from leaving the king under check

	std::list<std::unique_ptr<IGameView>> m_Views;	
}; // End class ChessGame declaration
