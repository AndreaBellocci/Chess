// Milan, 12th December 2021
//	Completed on December, 12
// 

#pragma once
#include <string>

#include "../Libraries/EngineUtility.h"
#include "../Libraries/EventManager.h"
#include "Pieces.h"


// Debug Helpers
#if _DEBUG
#include <cstdio>
template <typename ...Args>
__forceinline void DBG_OnEventConstructor(const char* eventStr, Args... args)
{
	printf(eventStr, args...); putchar('\n');
};
__forceinline void DBG_OnEventHandler(const char* eventName) {};
#else
#define DBG_OnEventConstructor(eventStr, ...)	do { void(0); } while(0);
#define DBG_OnEventHandler(eventName)			do { void(0); } while(0);
#endif

// ========================================================================================================================================
// ========================================================================================================================================
// Events Declarations
// ========================================================================================================================================
// ========================================================================================================================================


// ========================================================================================================================================
// Class Event_LeftButtonDown Declaration
// ========================================================================================================================================
class Event_LeftButtonDown : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_LeftButtonDown(int x, int y) : m_x(x), m_y(y)
	{
		DBG_OnEventConstructor("%-32s: %d %d", typeid(*this).name(), x, y);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_LeftButtonDown::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_LeftButtonDown::sk_EventType;
	} // GetEventType

	const int m_x;
	const int m_y;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_LeftButtonDown declaration


// ========================================================================================================================================
// Class Event_RightButtonDown Declaration
// ========================================================================================================================================
class Event_RightButtonDown : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_RightButtonDown(int x, int y) : m_x(x), m_y(y)
	{
		DBG_OnEventConstructor("%-32s: %d %d", typeid(*this).name(), x, y);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_RightButtonDown::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_RightButtonDown::sk_EventType;
	} // GetEventType

	const int m_x;
	const int m_y;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_RightButtonDown declaration


// ========================================================================================================================================
// Class Event_SelectSquare Declaration
// ========================================================================================================================================
class Event_SelectSquare : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_SelectSquare(int x, int y) : m_x(x), m_y(y)
	{
		DBG_OnEventConstructor("%-32s: %d %d", typeid(*this).name(), x, y);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_SelectSquare::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_SelectSquare::sk_EventType;
	} // GetEventType

	const int m_x;
	const int m_y;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_SelectSquare declaration


// ========================================================================================================================================
// Class Event_IllegalMove Declaration
// ========================================================================================================================================
class Event_SelectionReset : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_SelectionReset()
	{
		DBG_OnEventConstructor("%-32s", typeid(*this).name());
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_SelectionReset::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_SelectionReset::sk_EventType;
	} // GetEventType

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_SelectionReset declaration


// ========================================================================================================================================
// Class Event_IllegalMove Declaration
// ========================================================================================================================================
class Event_IllegalMove : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_IllegalMove(int indexInBoard) : m_Index(indexInBoard)
	{
		DBG_OnEventConstructor("%-32s: %d", typeid(*this).name(), indexInBoard);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_IllegalMove::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_IllegalMove::sk_EventType;
	} // GetEventType

	const int m_Index;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;	
}; // end class Event_IllegalMove declaration


// ========================================================================================================================================
// Class Event_PieceSelected Declaration
// ========================================================================================================================================
class Event_PieceSelected : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_PieceSelected(int indexInBoard) : m_Index(indexInBoard) 
	{
		DBG_OnEventConstructor("%-32s: %d", typeid(*this).name(), indexInBoard);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_PieceSelected::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_PieceSelected::sk_EventType;
	} // GetEventType

	const int m_Index;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_PieceSelected declaration


// ========================================================================================================================================
// Class Event_StartMovePiece Declaration
// ========================================================================================================================================
class Event_StartMovePiece : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_StartMovePiece(Piece* to_move, int fromIndex, int destIndex)
		: m_Piece(to_move), m_Src(fromIndex), m_Dest(destIndex)
	{
		DBG_OnEventConstructor("%-32s: from %d to %d", typeid(*this).name(), fromIndex, destIndex);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_StartMovePiece::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_StartMovePiece::sk_EventType;
	} // GetEventType

	const int m_Src;
	const int m_Dest;
	Piece* const m_Piece;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_StartMovePiece declaration


// ========================================================================================================================================
// Class Event_Castle Declaration
// ========================================================================================================================================
class Event_Castle : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_Castle(Piece* to_move, int fromIndex, int destIndex)
		: m_Piece(to_move), m_Src(fromIndex), m_Dest(destIndex)
	{
		DBG_OnEventConstructor("%-32s: from %d to %d", typeid(*this).name(), fromIndex, destIndex);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_Castle::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_Castle::sk_EventType;
	} // GetEventType

	const int m_Src;
	const int m_Dest;
	Piece* const m_Piece;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_Castle declaration

// ========================================================================================================================================
// Class Event_EndMovePiece Declaration
// ========================================================================================================================================
class Event_EndMovePiece : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_EndMovePiece(Piece* to_move, int destIndex) : m_Piece(to_move), m_Dest(destIndex)
	{
		DBG_OnEventConstructor("%-32s: arrived at %d", typeid(*this).name(), destIndex);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_EndMovePiece::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_EndMovePiece::sk_EventType;
	} // GetEventType

	const int m_Dest;
	Piece* const m_Piece;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_EndMovePiece declaration


// ========================================================================================================================================
// Class Event_PieceEaten Declaration
// ========================================================================================================================================
class Event_PieceEaten : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_PieceEaten(Piece* eaten) : m_Piece(eaten)
	{
		DBG_OnEventConstructor("%-32s: %s:", typeid(*this).name(), eaten->GetPieceName().c_str());
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_PieceEaten::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_PieceEaten::sk_EventType;
	} // GetEventType

	Piece* const m_Piece;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_PieceEaten declaration


// ========================================================================================================================================
// Class Event_PawnPromotion Declaration
// ========================================================================================================================================
class Event_PawnPromotion : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_PawnPromotion(Pawn* to_promote) : m_Pawn(to_promote)
	{
		DBG_OnEventConstructor("%-32s: promoting %s", typeid(*this).name(), m_Pawn->GetPieceName().c_str());
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_PawnPromotion::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_PawnPromotion::sk_EventType;
	} // GetEventType

	Pawn* const m_Pawn;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_PawnPromotion declaration


// ========================================================================================================================================
// Class Event_EndTurn Declaration
// ========================================================================================================================================
class Event_Check : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_Check(int king_pos) : m_KingIndex(king_pos)
	{
		DBG_OnEventConstructor("%-32s on king at pos %d", typeid(*this).name(), king_pos);
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_Check::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_Check::sk_EventType;
	} // GetEventType

	const int m_KingIndex;

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_Check declaration


// ========================================================================================================================================
// Class Event_EndTurn Declaration
// ========================================================================================================================================
class Event_EndTurn : public ENGINE_NAMESPACE::IEvent
{
public:
	inline Event_EndTurn() 
	{
		DBG_OnEventConstructor("%-32s", typeid(*this).name());
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_EndTurn::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_EndTurn::sk_EventType;
	} // GetEventType

private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_EndTurn declaration


// ========================================================================================================================================
// Class Event_EndMatch Declaration
// ========================================================================================================================================
class Event_EndMatch : public ENGINE_NAMESPACE::IEvent
{
public:
	enum class END_STATUS { White = 1, Black = 0, Stalemate = -1, Playing = -2 };

	inline Event_EndMatch(END_STATUS end) : m_Status(end)
	{
		std::string s;
		if (end == END_STATUS::Stalemate) s = "Stalemate";
		else if (end == END_STATUS::White) s = "Winner is White";
		else if (end == END_STATUS::Black) s = "Winner is Black";

		DBG_OnEventConstructor("*** %-32s %s! ***", typeid(*this).name(), s.c_str());
	} // Constructor

	constexpr virtual ENGINE_NAMESPACE::EventType VGetEventType() const override
	{
		return Event_EndMatch::sk_EventType;
	} // end method VGetEventType

	constexpr static ENGINE_NAMESPACE::EventType GetEventType()
	{
		return Event_EndMatch::sk_EventType;
	} // GetEventType

	enum END_STATUS m_Status;
private:
	static const ENGINE_NAMESPACE::EventType sk_EventType;
}; // end class Event_EndMatch declaration