// Milan, 10th December 2021
// 

#pragma once
#include <array>
#include <queue>
#include <string>
#include <raylib.h>
#include "../Libraries/Processes.h"
#include "ChessEvents.h"
#include "Pieces.h"

struct RECT
{
	int x;		// Rectangle top-left corner position x
	int y;		// Rectangle top-left corner position y
	int width;	// Rectangle width
	int height;	// Rectangle height
};

enum class GameViewType
{
	GameView_Human,
	GameView_Remote,
	GameView_AI,
	GameView_Recorder,
	GameView_Other
};


class TextureLoader
{
	friend class PawnPromoter;
public:
	static const Texture2D* GetPieceTextureAndName(PieceType typeID, std::string& outFileName);
	static const Texture2D* LoadPieceTextureFromFile(PieceType typeID, const std::string& fileName);
	static void SetTextureStartingSize(int new_width, int new_height);

protected:
	static int tex_width, tex_height;
	static std::array<std::pair<std::string, Texture2D>, (size_t)PieceType::Num_Types> textures;
}; // end static class TextureLoader declaration



// ========================================================================================================================================
// Class PawnPromoter Declaration
// ========================================================================================================================================
class PawnPromoter
{
	friend class HumanView;
protected:
	PawnPromoter() = default;

	void PromotePawn(Pawn* const pawn);
	void OnInput(int hovering_x, int hovering_y, bool selected);
	void OnRender();
	bool m_Visible = false;

private:
	Pawn* m_PawnToPromote = nullptr;
	int m_Start_x = INVALID;
	int m_Start_y = INVALID;
	int m_Hovering = INVALID;
}; // End class PawnPromoter Declaration


// ========================================================================================================================================
// ========================================================================================================================================
// Game Views Declarations
// ========================================================================================================================================
// ========================================================================================================================================

class IGameView
{
public:
	IGameView(const Board& board) : m_Board(board) {} // Constructor

	virtual void VOnInitialize() = 0;
	virtual void VOnResize() = 0;
	virtual void VOnInput() = 0;
	virtual void VOnRender() = 0;
	virtual void VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta) = 0;

protected:
	const Board& m_Board;
	static ENGINE_NAMESPACE::EventManager& s_GlobalEventManager;
}; // End interface IGameView
 

class HumanView : public IGameView
{
public:
	HumanView(const Board& board, const PossibleMovesIndeces& targets);
	HumanView(const Board& board, const PossibleMovesIndeces& targets, int left, int right, int top, int bottom);
	HumanView(const Board& board, const PossibleMovesIndeces& targets, Color background, Color first, Color second);
	HumanView(const Board& board, const PossibleMovesIndeces& targets, Color background, Color first, Color second, int left, int right, int top, int bottom);

	virtual void VOnInitialize() override final;
	virtual void VOnResize() override final;
	virtual void VOnInput() override final;
	virtual void VOnRender() override final;
	virtual void VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta) override final;

	// Accessors
	inline void GetHovering(int& x, int& y) const { x = this->m_Hovering_x; y = this->m_Hovering_y; }
	inline void GetSelected(int& x, int& y) const { x = this->m_Selected_x; y = this->m_Selected_y; }
	inline void SetHovering(int x, int y) { this->m_Hovering_x = x; this->m_Hovering_y = y; }
	inline void SetSelected(int x, int y) { this->m_Selected_x = x; this->m_Selected_y = y; }

	inline int GetHoveringX() const { return this->m_Hovering_x; }
	inline int GetHoveringY() const { return this->m_Hovering_y; }
	inline int GetSelectedX() const { return this->m_Selected_x; }
	inline int GetSelectedY() const { return this->m_Selected_y; }
	inline void SetHoveringX(int x) { this->m_Hovering_x = x; }
	inline void SetHoveringY(int y) { this->m_Hovering_y = y; }
	inline void SetSelectedX(int x) { this->m_Selected_x = x; }
	inline void SetSelectedY(int y) { this->m_Selected_y = y; }

	
	inline void SetHighLight_HoveringSquare(bool highlight) { this->m_HighLight_Hovering = highlight; }
	inline void SetHighLight_Targets(bool highlight)		{ this->m_HighLight_Targets = highlight; }
	inline void SetHighLight_Checks(bool highlight)			{ this->m_HighLight_Checks = highlight; }
	inline void SetHighLight_SelectedPiece(bool highlight)	{ this->m_HighLight_SelectedPiece = highlight; }
	inline void SetHighLight_PossibleMoves(bool highlight)	{ this->m_HighLight_PossibleMoves = highlight; }

	inline bool GetHighLight_HoveringSquare() const	{ return this->m_HighLight_Hovering; }
	inline bool GetHighLight_Targets() const		{ return this->m_HighLight_Targets; }	
	inline bool GetHighLight_Checks() const			{ return this->m_HighLight_Checks; }
	inline bool GetHighLight_SelectedPiece() const	{ return this->m_HighLight_SelectedPiece; }
	inline bool GetHighLight_PossibleMoves() const	{ return this->m_HighLight_PossibleMoves; }

private:
	// Utility
	void DrawBoard() const;
	void TranslateAndDispachInput();

	// Event listeners
	void OnRightClick(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnLeftClick(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnSelectSquare(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnSelectionReset(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnStartMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnCastle(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnPawnPromotion(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnCheck(const ENGINE_NAMESPACE::IEventPtr& pEvent);
	void OnEndMatch(const ENGINE_NAMESPACE::IEventPtr& pEvent);

	// Used to animate movements
	ENGINE_NAMESPACE::ProcessManager m_ProcessManager;

	PawnPromoter m_PawnPromoter;
	const PossibleMovesIndeces& m_Targets;
	
	Color m_BackGround = PURPLE;
	Color m_FirstColor = YELLOW;
	Color m_SecondColor = GRAY;

	bool m_HighLight_Hovering		: 1 = true;	
	bool m_HighLight_Targets		: 1 = true;
	bool m_HighLight_Checks			: 1 = true;
	bool m_HighLight_SelectedPiece	: 1 = true;
	bool m_HighLight_PossibleMoves	: 1 = true;
	Event_EndMatch::END_STATUS m_MatchOver = Event_EndMatch::END_STATUS::Playing;

	int m_CheckKingIndex = INVALID;
	
	Color m_HoveringColor	= GREEN;
	Color m_TargetsColor	= DARKBLUE;	
	Color m_ChecksColor		= RED;
	Color m_SelectedColor	= SKYBLUE;
	Color m_PossibleMoves	= MAGENTA;

	int m_Screen_Width;
	int m_Screen_Height;
	int m_Square_Side;

	int m_Offset_Left;
	int m_Offset_Right;
	int m_Offset_Top;
	int m_Offset_Bottom;

	int m_Selected_x; // In range [0,7]
	int m_Selected_y; // In range [0,7]

	int m_Hovering_x; // In range [0,7]
	int m_Hovering_y; // In range [0,7]

	// This class offers a method for entering script commands at runtime.
	class Console
	{
	public:
		Console();

		void AddDisplayText(const std::string& newText);
		void SetDisplayText(const std::string& newText);

		inline void SetActive(bool bIsActive)	{ this->m_bActive = bIsActive; }
		inline bool IsActive() const			{ return this->m_bActive; }

		void HandleKeyboardInput();

		void OnUpdate(ENGINE_NAMESPACE::milliseconds delta);
		void OnRender();

	private:
		std::queue<std::string> m_DisplayStrings;

		RECT m_Console;

		Color m_InputColor;
		Color m_OutputColor;

		std::string m_CurrentOutputString;
		std::string m_CurrentInputString;

		int m_ConsoleInputSize;	// Height of the input console window

		constexpr static char const* const ExitString = "exit";
		constexpr static char const* const ClearString = "clear";
		constexpr static ENGINE_NAMESPACE::milliseconds CursorBlinkTimeMS = 500;

		ENGINE_NAMESPACE::milliseconds m_CursorBlinkTimer; // Countdown to toggle cursor blink state
		bool m_bCursorOn;		// Is the cursor currently displayed?

		bool m_bShiftDown;		// Is the shift button down?
		bool m_bCapsLockDown;	// Is the caps lock button down?

		// If this is true, we have a string to execute on our next update cycle.  .
		bool m_bExecuteStringOnUpdate;

		bool m_bActive;
	}; // End class Console declaration

	//Console m_Console;
}; // End class HumanView declaration




// ========================================================================================================================================
// ========================================================================================================================================
// Processes Declarations
// ========================================================================================================================================
// ========================================================================================================================================


// ========================================================================================================================================
// Delay Process
// ========================================================================================================================================
class DelayProcess : public ENGINE_NAMESPACE::Process
{
public:
	explicit DelayProcess(ENGINE_NAMESPACE::nanoseconds delayTime);

protected:
	virtual void VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta) override final;
	inline virtual void VOnSuccess() override final {};
	inline virtual void VOnFail() override final {};
	inline virtual void VOnAbort() override final {};

private:
	const ENGINE_NAMESPACE::nanoseconds m_Delay;
	ENGINE_NAMESPACE::nanoseconds m_TimePassedSoFar;
}; // DelayProcess


// ========================================================================================================================================
// Animate pieces movements Process
// ========================================================================================================================================
class MovePieceProcess : public ENGINE_NAMESPACE::Process
{
public:
	explicit MovePieceProcess(Piece2DRenderComponent& image,
		ENGINE_NAMESPACE::nanoseconds executionTime, int stop_row_coord, int stop_col_coord);

protected:
	virtual void VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta) override final;
	inline virtual void VOnSuccess() override final {};
	inline virtual void VOnFail() override final {};
	inline virtual void VOnAbort() override final {};

private:
	const ENGINE_NAMESPACE::nanoseconds m_TotalTimeAvailable;
	ENGINE_NAMESPACE::nanoseconds m_TimePassedSoFar;
	Piece2DRenderComponent& m_Image;
	const int m_Stop_Row;
	const int m_Stop_Col;
}; // MovePieceProcess