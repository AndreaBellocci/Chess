// Milan, 11ht December 2021
//	Completed on December, 11
//

#include "Pieces.h"
#include "Libraries/Logger.h"
using namespace ENGINE_NAMESPACE;

PieceSet Piece::s_Set;

// ========================================================================================================================================
// Class Piece Definition
// ========================================================================================================================================

Piece::Piece(Pieces ID, const std::string& pieceName)
	: m_ID(ID), m_MovePieceImpl(nullptr), m_CanEatKingImpl(nullptr), m_CanMoveImpl(nullptr),
	m_Alive(false), m_PieceName(pieceName)
{

#if DEBUGGING
	if (Piece::s_Set[(size_t)ID] != nullptr)
		LOG_WARNING(LOGS::L_UNEXPECTED_VALID_POINTER, "Destroying an existing piece");
#endif

	Piece::s_Set[(size_t)ID] = std::unique_ptr<Piece>(this);
	this->m_Components.clear();
} // Constructor

Piece::~Piece()
{
	this->m_Components.clear();
} // Destructor

void Piece::BuildPossibleMoves(int indexInBoard, const Board& board, PossibleMovesIndeces& outMoves, bool allowCheck) const
{
	ThrowIfNullptr(this->m_MovePieceImpl, LOGS::L_INVALID_POINTER, true);

	this->m_MovePieceImpl(this->m_ID, indexInBoard, board, outMoves, allowCheck);
} // BuildPossibleMoves

bool Piece::CanEatKing(int this_index, int king_index, const board_t& board) const
{
	ThrowIfNullptr(this->m_CanEatKingImpl, LOGS::L_INVALID_POINTER, true);

	return this->m_CanEatKingImpl(this_index, king_index, board);
} // CanEatKing

bool Piece::CanMove(int this_index, const Board& board) const
{
	ThrowIfNullptr(this->m_CanEatKingImpl, LOGS::L_INVALID_POINTER, true);

	return this->m_CanMoveImpl(this_index, board);
} // CanMove

IPieceComponent* Piece::GetComponent(const std::string& componentName) const
{
	auto it = this->m_Components.find(componentName);
	if (it == this->m_Components.end())
		return nullptr;
	else
		return (*it).second.get();
} // GetComponent

// ========================================================================================================================================
// ========================================================================================================================================
// Classes implementations
// ========================================================================================================================================
// ========================================================================================================================================

Rook::Rook(Pieces ID, const std::string& pieceName)
	: Piece(ID, pieceName)
{
	this->m_MovePieceImpl = Rook_BuildPossibleMoves;
	this->m_CanEatKingImpl = Rook_CanEatKingInSquare;
	this->m_CanMoveImpl = Rook_CanMove;
} // Rook Constructor

Knight::Knight(Pieces ID, const std::string& pieceName)
	: Piece(ID, pieceName)
{
	this->m_MovePieceImpl = Knight_BuildPossibleMoves;
	this->m_CanEatKingImpl = Knight_CanEatKingInSquare;
	this->m_CanMoveImpl = Knight_CanMove;
} // Knight Constructor

Bishop::Bishop(Pieces ID, const std::string& pieceName)
	: Piece(ID, pieceName)
{
	this->m_MovePieceImpl = Bishop_BuildPossibleMoves;
	this->m_CanEatKingImpl = Bishop_CanEatKingInSquare;
	this->m_CanMoveImpl = Bishop_CanMove;
} // Bishop Constructor

Queen::Queen(Pieces ID, const std::string& pieceName)
	: Piece(ID, pieceName)
{
	this->m_MovePieceImpl = Queen_BuildPossibleMoves;
	this->m_CanEatKingImpl = Queen_CanEatKingInSquare;
	this->m_CanMoveImpl = Queen_CanMove;
} // Queen Constructor

King::King(Pieces ID, const std::string& pieceName)
	: Piece(ID, pieceName)
{
	this->m_MovePieceImpl = King_BuildPossibleMoves;
	this->m_CanEatKingImpl = King_CanEatKingInSquare;
	this->m_CanMoveImpl = King_CanMove;
} // King Constructor

Pawn::Pawn(Pieces ID, const std::string& pieceName)
	: Piece(ID, pieceName), m_Current(Promotions::Null)
{
	this->m_MovePieceImpl = Pawn_BuildPossibleMoves;
	this->m_CanEatKingImpl = Pawn_CanEatKingInSquare;
	this->m_CanMoveImpl = Pawn_CanMove;
} // Pawn Constructor


void Pawn::Promote(Promotions new_piece)
{
	// Each pawn can be promoted only once
	if (this->m_Current == Promotions::Null)
	{
		// Black or white?
		PieceType type;

		// Simply change implementation
		switch (new_piece)
		{
		case Promotions::Rook:
			type = (this->m_ID >= Pieces::White_Rook_1 ? PieceType::White_Rook : PieceType::Black_Rook);
			this->m_MovePieceImpl = Rook_BuildPossibleMoves;
			this->m_CanEatKingImpl = Rook_CanEatKingInSquare;
			this->m_CanMoveImpl = Rook_CanMove;
			LOG_MESSAGE(LOGS::L_INFO, "Promoting pawn to Rook");
			break;

		case Promotions::Knight:
			type = (this->m_ID >= Pieces::White_Rook_1 ? PieceType::White_Knight : PieceType::Black_Knight);
			this->m_MovePieceImpl = Knight_BuildPossibleMoves;
			this->m_CanEatKingImpl = Knight_CanEatKingInSquare;
			this->m_CanMoveImpl = Knight_CanMove;
			LOG_MESSAGE(LOGS::L_INFO, "Promoting pawn to Knight");
			break;

		case Promotions::Bishop:
			type = (this->m_ID >= Pieces::White_Rook_1 ? PieceType::White_Bishop : PieceType::Black_Bishop);
			this->m_MovePieceImpl = Bishop_BuildPossibleMoves;
			this->m_CanEatKingImpl = Bishop_CanEatKingInSquare;
			this->m_CanMoveImpl = Bishop_CanMove;
			LOG_MESSAGE(LOGS::L_INFO, "Promoting pawn to Bishop");
			break;

		case Promotions::Queen:
			type = (this->m_ID >= Pieces::White_Rook_1 ? PieceType::White_Queen : PieceType::Black_Queen);
			this->m_MovePieceImpl = Queen_BuildPossibleMoves;
			this->m_CanEatKingImpl = Queen_CanEatKingInSquare;
			this->m_CanMoveImpl = Queen_CanMove;
			LOG_MESSAGE(LOGS::L_INFO, "Promoting pawn to Queen");
			break;

		default:
			LOG_ERROR(LOGS::L_UNKNOWN_CODE, "Invalid pawn promotion");
			return;
		}

		// Change components to match the new identity
		this->m_Current = new_piece;
		auto it = this->m_Components.begin();
		for (it; it != this->m_Components.end(); ++it)
			it->second->VChangePieceComponent(type);
	}
} // Promote