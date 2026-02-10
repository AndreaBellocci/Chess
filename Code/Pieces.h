// Milan, 11ht December 2021
//	Completed: 8th February 2026, Chicago
//

#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "Types.h"
#include "Chess.h"


// ========================================================================================================================================
// Chess algorithms - they're implemented in files inside 'Pieces'
// ========================================================================================================================================
typedef std::vector<int> PossibleMoves;

void Rook_BuildPossibleMoves  (Pieces rook_id,   int rook_pos,	 const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);
void Knight_BuildPossibleMoves(Pieces knight_id, int knight_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);
void Bishop_BuildPossibleMoves(Pieces bishop_id, int bishop_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);
void Queen_BuildPossibleMoves (Pieces queen_id,  int queen_pos,	 const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);
void King_BuildPossibleMoves  (Pieces king_id,   int king_pos,	 const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);
void Pawn_BuildPossibleMoves  (Pieces pawn_id,   int pawn_pos,	 const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);

bool Rook_CanEatKingInSquare  (int rook_pos,   int enemy_king_pos, const board_t& board);
bool Knight_CanEatKingInSquare(int knight_pos, int enemy_king_pos, const board_t& board);
bool Bishop_CanEatKingInSquare(int bishop_pos, int enemy_king_pos, const board_t& board);
bool Queen_CanEatKingInSquare (int queen_pos,  int enemy_king_pos, const board_t& board);
bool King_CanEatKingInSquare  (int king_pos,   int enemy_king_pos, const board_t& board);
bool Pawn_CanEatKingInSquare  (int pawn_pos,   int enemy_king_pos, const board_t& board);

bool Rook_CanMove  (int rook_pos,   const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);
bool Knight_CanMove(int knight_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);
bool Bishop_CanMove(int bishop_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);
bool Queen_CanMove (int queen_pos,  const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);
bool King_CanMove  (int king_pos,   const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);
bool Pawn_CanMove  (int pawn_pos,   const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);


// ========================================================================================================================================
// Class Piece Declaration
// ========================================================================================================================================
class Piece
{
	typedef void (*BuildMoveTreeImpl)		(Pieces pawn_id, int pawn_pos, const Chess& the_game, Pieces my_king_id, PossibleMoves& out_moves, bool allow_checks);
	typedef bool (*CanEatKingInSquareImpl)	(int pawn_pos, int enemy_king_pos, const board_t& board);
	typedef bool (*CanMoveImpl)				(int pawn_pos, const Chess& the_game, Pieces my_king_id, int my_king_pos, bool allow_checks);

	friend class ChessGame;
public:
	virtual ~Piece();

	void Reset(int new_pos);
	void BuildPossibleMoves(const Chess& the_game, bool allow_checks);
	bool CanEatKing(int king_pos, const board_t& board) const;
	bool CanMove(const Chess& the_game, bool allow_checks) const;

	inline int GetPiecePos() const noexcept   { return this->m_piece_pos; }
	inline Pieces GetPieceID() const noexcept { return this->m_id; }
	inline const std::string& GetPieceName() const noexcept { return this->m_piece_name; }
	inline const PossibleMoves* GetCachedMoves() const noexcept { return &this->m_CachedMoves; }
	inline void ClearCachedMoves() noexcept { this->m_CachedMoves.clear(); }

	inline void Die() noexcept { this->m_Alive = false; this->m_piece_pos = INVALID; this->m_CachedMoves.clear(); } // A piece cannot be revived
	inline bool IsAlive() const noexcept { return this->m_Alive; }

protected:
	Piece(Pieces ID, Pieces king_ID, const std::string& pieceName);

	const Pieces m_id;
	const Pieces m_king_id;
	const std::string m_piece_name;

	CanMoveImpl m_CanMoveImpl;
	BuildMoveTreeImpl m_MovePieceImpl;
	CanEatKingInSquareImpl m_CanEatKingImpl;
	
	PossibleMoves m_CachedMoves;
	int m_piece_pos;
	bool m_Alive;
}; // End class Piece declaration



// ========================================================================================================================================
// Derived classes Declarations
// ========================================================================================================================================

class Rook : public Piece
{
public:
	Rook(Pieces ID, Pieces king_ID, const std::string& pieceName);
}; // End Class Rook declaration


class Knight : public Piece
{
public:
	Knight(Pieces ID, Pieces king_ID, const std::string& pieceName);
}; // End Class Knight declaration


class Bishop : public Piece
{
public:
	Bishop(Pieces ID, Pieces king_ID, const std::string& pieceName);
}; // End Class Bishop declaration


class Queen : public Piece
{
public:
	Queen(Pieces ID, Pieces king_ID, const std::string& pieceName);
}; // End Class Queen declaration


class King : public Piece
{
public:
	King(Pieces ID, Pieces king_ID, const std::string& pieceName);
}; // End Class King declaration


class Pawn : public Piece
{
public:
	Pawn(Pieces ID, Pieces king_ID, const std::string& pieceName);

	enum class Promotions { Null = 0, Rook, Knight, Bishop, Queen };
	inline Promotions GetPromotion() const noexcept { return this->m_Rank; }
	inline bool HasBeenPromoted() const noexcept { return this->m_Rank != Promotions::Null; }
	void Promote(Promotions new_piece);

private:
	Promotions m_Rank;
}; // End Class Pawn declaration