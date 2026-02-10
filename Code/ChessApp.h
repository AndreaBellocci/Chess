// Milan, 6th December 2021
//

#pragma once
#include <array>
#include <memory>
#include <string>

#include "../Libraries/EventManager.h"
#include "../Libraries/Processes.h"

#include "Chess.h"
#include "GameViews.h"


ENGINE_NAMESPACE::EventManager& GetEventManager();



// Class ChessGame - contains everything needed to play chess
class ChessApp
{
	friend class ChessGame;
public:
	ChessApp(const std::string& initFileName);
	~ChessApp();

	void OnInitialize();
	void OnInput();
	void OnUpdate(std::chrono::nanoseconds delta);
	void OnRender();

	static piece_list SetupPieces(const std::string& xml_settings_file);


private:
	// Utility
	void StoreFENString();
	Piece* MapCharToPiece(char c) const;
	char MapPieceIDToChar(Pieces id) const;

	static ENGINE_NAMESPACE::EventManager& s_GlobalEventManager;
	
	ChessGame m_app;
	PossibleMoves m_Targets;

	int m_Half_Moves = 0;
	int m_Full_Moves = 0;

	const std::string m_InitFile;

	std::string m_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	bool m_Turn : 1 = true;			// True for white, false for black
	bool m_AllowChecks : 1 = false;	// If false, then the app prevents players from leaving the king under check

	std::list<std::unique_ptr<IGameView>> m_Views;	
}; // End class ChessApp declaration
