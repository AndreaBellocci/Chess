// Milan, 6th December 2021
//

#pragma once
#include <array>
#include <memory>
#include <string>


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
	bool ShouldQuit() const noexcept { return this->m_app.ShouldQuit(); }

	static piece_list SetupPieces(const std::string& xml_settings_file);


private:	
	ChessGame m_app;
	const std::string m_InitFile;
	std::list<std::unique_ptr<IGameView>> m_Views;	
}; // End class ChessApp declaration
