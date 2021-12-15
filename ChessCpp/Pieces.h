// Milan, 11ht December 2021
//	Completed on December, 11
//

#pragma once
#include <map>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include "PieceComponents.h"


// Forward declarations for classes/structs defined in other headers
struct Board;

enum class Pieces : char
{
	Null = -1,
	Black_Rook_1,
	Black_Rook_2,
	Black_Knight_1,
	Black_Knight_2,
	Black_Bishop_1,
	Black_Bishop_2,
	Black_Queen,
	Black_King,
	Black_Pawn_1,
	Black_Pawn_2,
	Black_Pawn_3,
	Black_Pawn_4,
	Black_Pawn_5,
	Black_Pawn_6,
	Black_Pawn_7,
	Black_Pawn_8,

	White_Rook_1,
	White_Rook_2,
	White_Knight_1,
	White_Knight_2,
	White_Bishop_1,
	White_Bishop_2,
	White_Queen,
	White_King,
	White_Pawn_1,
	White_Pawn_2,
	White_Pawn_3,
	White_Pawn_4,
	White_Pawn_5,
	White_Pawn_6,
	White_Pawn_7,
	White_Pawn_8,

	NumPieces
}; // End Scoped enum Pieces

// This is awful, pieces themselves should not even care about being in a board or whatever,
// but some algorithms requires the simpler board type (board_t), so not doing it it's worse
// and I have no time or desire to come up with a better solution
#define BOARD_SIDE 8
typedef std::array<class Piece*, BOARD_SIDE * BOARD_SIDE> board_t;
typedef std::array<std::unique_ptr<Piece>, (size_t)Pieces::NumPieces> PieceSet;

// ========================================================================================================================================
// Chess algorithms - they're implemented in ChessAlgorithms.cpp
// ========================================================================================================================================
typedef std::vector<int> PossibleMovesIndeces;

void Rook_BuildPossibleMoves(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks = false);
void Knight_BuildPossibleMoves(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks = false);
void Bishop_BuildPossibleMoves(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks = false);
void Queen_BuildPossibleMoves(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks = false);
void King_BuildPossibleMoves(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks = false);
void Pawn_BuildPossibleMoves(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks = false);

bool Rook_CanEatKingInSquare(int rook_index, int king_index, const board_t& board);
bool Knight_CanEatKingInSquare(int knight_index, int king_index, const board_t& board);
bool Bishop_CanEatKingInSquare(int bishop_index, int king_index, const board_t& board);
bool Queen_CanEatKingInSquare(int queen_index, int king_index, const board_t& board);
bool King_CanEatKingInSquare(int our_king_index, int king_index, const board_t& board);
bool Pawn_CanEatKingInSquare(int pawn_index, int king_index, const board_t& board);

bool Rook_CanMove(int this_index, const Board& board);
bool Knight_CanMove(int this_index, const Board& board);
bool Bishop_CanMove(int this_index, const Board& board);
bool Queen_CanMove(int this_index, const Board& board);
bool King_CanMove(int this_index, const Board& board);
bool Pawn_CanMove(int this_index, const Board& board);

bool IsKingUnderCheck(const board_t& board, int king_pos);
bool CanPlayerMove(bool player, const Board& board);

// ========================================================================================================================================
// Class Piece Declaration
// ========================================================================================================================================
class Piece
{
	friend class ChessGame;
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);

	// ========================================================================================================================================
	// Typedef for method's implementation Declaration
	// ========================================================================================================================================
	typedef void (*MovePieceImpl)(Pieces this_ID, int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowChecks);
	typedef bool (*CanEatKingInSquareImpl)(int piece_index, int king_index, const board_t& board);
	typedef bool (*CanMoveImpl)(int piece_index, const Board& board);

	
protected:
	Piece(Pieces ID, const std::string& pieceName);

public:
	virtual ~Piece();

	IPieceComponent* GetComponent(const std::string& componentName) const;

	inline void Die()			{ this->m_Alive = false; } // A piece cannot be revived
	inline bool IsAlive() const { return this->m_Alive; }

	inline Pieces GetPieceID() const { return this->m_ID; }
	inline const std::string& GetPieceName() const { return this->m_PieceName; };

	void BuildPossibleMoves(int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowCheck) const;
	bool CanEatKing(int this_index, int king_index, const board_t& board) const;
	bool CanMove(int this_index, const Board& board) const;

	static __forceinline const PieceSet& GetSet() { return Piece::s_Set; }

protected:
	// This method is useful only on app startup, when placing pieces on the board
	inline void SetAlive(bool alive) { this->m_Alive = alive; }

	const Pieces m_ID;
	CanMoveImpl m_CanMoveImpl;
	MovePieceImpl m_MovePieceImpl;
	CanEatKingInSquareImpl m_CanEatKingImpl;
	std::map<std::string, std::shared_ptr<IPieceComponent>> m_Components;

	static PieceSet s_Set;


	const std::string m_PieceName;
	bool m_Alive;
}; // End class Piece declaration



// ========================================================================================================================================
// Derived classes Declarations
// ========================================================================================================================================

class Rook : public Piece
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
protected:
	Rook(Pieces ID, const std::string& pieceName);
}; // End Class Rook declaration


class Knight : public Piece
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
protected:
	Knight(Pieces ID, const std::string& pieceName);
}; // End Class Knight declaration


class Bishop : public Piece
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
protected:
	Bishop(Pieces ID, const std::string& pieceName);
}; // End Class Bishop declaration


class Queen : public Piece
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
protected:
	Queen(Pieces ID, const std::string& pieceName);
}; // End Class Queen declaration


class King : public Piece
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
protected:
	King(Pieces ID, const std::string& pieceName);
}; // End Class King declaration


class Pawn : public Piece
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);

public:
	enum class Promotions { Null = 0, Rook, Knight, Bishop, Queen };

	inline Promotions GetPromotion() const { return this->m_Current; }
	inline bool HasBeenPromoted() const { return this->m_Current != Promotions::Null; }
	void Promote(Promotions new_piece);

protected:
	Pawn(Pieces ID, const std::string& pieceName);

private:
	Promotions m_Current;
}; // End Class Pawn declaration