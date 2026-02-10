// Milan, 11ht December 2021
//	Completed: 8th February 2026, Chicago
//

#include "Types.h"
#include "Pieces.h"
#include "Chess.h"
#include "ErrorCodes.h"

#include <string>

// ========================================================================================================================================
// Class Piece Definition
// ========================================================================================================================================

Piece::Piece(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: m_id(ID), m_king_id(king_ID), m_piece_name(pieceName), m_piece_pos(INVALID),
	m_MovePieceImpl(nullptr), m_CanEatKingImpl(nullptr), m_CanMoveImpl(nullptr),
	m_Alive(true), m_CachedMoves()
{} // Constructor

Piece::~Piece()
{
	this->m_Alive = false;
	this->m_CachedMoves.clear();
} // Destructor

void Piece::Reset(int new_pos)
{
	this->m_Alive = true;
	this->m_CachedMoves = {};
	this->m_piece_pos = new_pos;
} // Reset

void Piece::BuildPossibleMoves(const Chess& the_game, bool allow_checks)
{
	// Do not rebuild if already cached
	if (this->m_Alive && this->m_CachedMoves.empty())
		this->m_MovePieceImpl(this->m_id, this->m_piece_pos, the_game, this->m_king_id, this->m_CachedMoves, allow_checks);
} // BuildPossibleMoves

bool Piece::CanEatKing(int king_pos, const board_t& board) const
{
	// If the ID of the piece at this piece's position is not this piece's id, it means we're evaluating a possible future scenario
	return this->m_Alive && board[this->m_piece_pos]->m_id == this->m_id && this->m_CanEatKingImpl(this->m_piece_pos, king_pos, board);
} // CanEatKing

bool Piece::CanMove(const Chess& the_game, bool allow_checks) const
{
	const auto my_king_pos = the_game.m_Pieces[static_cast<int>(this->m_king_id)]->GetPiecePos();
	return this->m_Alive && this->m_CanMoveImpl(this->m_piece_pos, the_game, this->m_king_id, my_king_pos, allow_checks);
} // CanMove



// ========================================================================================================================================
// ========================================================================================================================================
// Classes implementations
// ========================================================================================================================================
// ========================================================================================================================================

Rook::Rook(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: Piece(ID, king_ID, pieceName)
{
	this->m_MovePieceImpl  = Rook_BuildPossibleMoves;
	this->m_CanEatKingImpl = Rook_CanEatKingInSquare;
	this->m_CanMoveImpl    = Rook_CanMove;
} // Rook Constructor

Knight::Knight(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: Piece(ID, king_ID, pieceName)
{
	this->m_MovePieceImpl  = Knight_BuildPossibleMoves;
	this->m_CanEatKingImpl = Knight_CanEatKingInSquare;
	this->m_CanMoveImpl    = Knight_CanMove;
} // Knight Constructor

Bishop::Bishop(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: Piece(ID, king_ID, pieceName)
{
	this->m_MovePieceImpl  = Bishop_BuildPossibleMoves;
	this->m_CanEatKingImpl = Bishop_CanEatKingInSquare;
	this->m_CanMoveImpl    = Bishop_CanMove;
} // Bishop Constructor

Queen::Queen(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: Piece(ID, king_ID, pieceName)
{
	this->m_MovePieceImpl  = Queen_BuildPossibleMoves;
	this->m_CanEatKingImpl = Queen_CanEatKingInSquare;
	this->m_CanMoveImpl    = Queen_CanMove;
} // Queen Constructor

King::King(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: Piece(ID, king_ID, pieceName)
{
	this->m_MovePieceImpl  = King_BuildPossibleMoves;
	this->m_CanEatKingImpl = King_CanEatKingInSquare;
	this->m_CanMoveImpl    = King_CanMove;
} // King Constructor

Pawn::Pawn(Pieces ID, Pieces king_ID, const std::string& pieceName)
	: Piece(ID, king_ID, pieceName), m_Rank(Promotions::Null)
{
	this->m_MovePieceImpl  = Pawn_BuildPossibleMoves;
	this->m_CanEatKingImpl = Pawn_CanEatKingInSquare;
	this->m_CanMoveImpl    = Pawn_CanMove;
} // Pawn Constructor


void Pawn::Promote(Promotions new_piece)
{
	// Each pawn can be promoted only once
	if (this->m_Rank == Promotions::Null)
	{
		// Black or white?
		PieceType type;

		// Simply change implementation
		switch (new_piece)
		{
		case Promotions::Rook:
			type = (this->m_king_id == Pieces::White_King ? PieceType::White_Rook : PieceType::Black_Rook);
			this->m_MovePieceImpl  = Rook_BuildPossibleMoves;
			this->m_CanEatKingImpl = Rook_CanEatKingInSquare;
			this->m_CanMoveImpl    = Rook_CanMove;
			break;

		case Promotions::Knight:
			type = (this->m_king_id == Pieces::White_King ? PieceType::White_Knight : PieceType::Black_Knight);
			this->m_MovePieceImpl  = Knight_BuildPossibleMoves;
			this->m_CanEatKingImpl = Knight_CanEatKingInSquare;
			this->m_CanMoveImpl    = Knight_CanMove;
			break;

		case Promotions::Bishop:
			type = (this->m_king_id == Pieces::White_King ? PieceType::White_Bishop : PieceType::Black_Bishop);
			this->m_MovePieceImpl  = Bishop_BuildPossibleMoves;
			this->m_CanEatKingImpl = Bishop_CanEatKingInSquare;
			this->m_CanMoveImpl    = Bishop_CanMove;
			break;

		case Promotions::Queen:
			type = (this->m_king_id == Pieces::White_King ? PieceType::White_Queen : PieceType::Black_Queen);
			this->m_MovePieceImpl  = Queen_BuildPossibleMoves;
			this->m_CanEatKingImpl = Queen_CanEatKingInSquare;
			this->m_CanMoveImpl    = Queen_CanMove;
			break;

		default:
			THROW_CHESS_EXCEPTION(ErrorCode::InvalidPromotion, "The promotion %d is not valid", static_cast<int>(new_piece));
		}
	}
} // Promote