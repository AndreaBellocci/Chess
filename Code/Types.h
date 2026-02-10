// Chicago, 8/04/2026
//	Completed on 8/04/2026

#pragma once
#include <array>
#include <memory>
#include <cstdint>

enum class PieceType : uint8_t
{
	Black_Rook = 0,
	Black_Knight,
	Black_Bishop,
	Black_Queen,
	Black_King,
	Black_Pawn,
	White_Rook,
	White_Knight,
	White_Bishop,
	White_Queen,
	White_King,
	White_Pawn,

	Num_Types
}; // End Scoped enum PieceType

constexpr auto num_piece_types = static_cast<int>(PieceType::Num_Types);

enum class Pieces : int8_t
{
	Null = -1,
	Black_Rook_Queen_Side,
	Black_Rook_King_Side,
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

	White_Rook_Queen_Side,
	White_Rook_King_Side,
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

constexpr auto first_black_piece_id = Pieces::Black_Rook_Queen_Side;
constexpr auto last_black_piece_id  = Pieces::Black_Pawn_8;
constexpr auto first_white_piece_id = Pieces::White_Rook_Queen_Side;
constexpr auto last_white_piece_id  = Pieces::White_Pawn_8;

constexpr auto first_black_piece_index = static_cast<int>(first_black_piece_id);
constexpr auto last_black_piece_index  = static_cast<int>(last_black_piece_id);
constexpr auto first_white_piece_index = static_cast<int>(first_white_piece_id);
constexpr auto last_white_piece_index  = static_cast<int>(last_white_piece_id);

// Forward declaration
class Piece;
constexpr auto num_pieces = static_cast<int>(Pieces::NumPieces);
typedef std::array<std::unique_ptr<class Piece>, num_pieces> piece_list;

// While this may not be the most memory-efficient solution,
// in a classic chess game the board is relatively small, so this is acceptable.
// This also simplifies the code and makes it more readable.
constexpr size_t BOARD_SIDE = 8;
typedef std::array<class Piece*, BOARD_SIDE * BOARD_SIDE> board_t;

// Board indeces are always nonnegative
constexpr auto INVALID = -1;