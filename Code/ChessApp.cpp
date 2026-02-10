// Milan, 6th December 2021
//

#include <raylib.h>
#include "ChessEvents.h"
#include "ChessApp.h"
#include "ErrorCodes.h"
#include "../Libraries/Logger.h"
#include "Pieces.h"
#include <memory>
using namespace ENGINE_NAMESPACE;


EventManager& GetEventManager()
{
	static EventManager manager(true);
	return manager;
} // GetEventManager

piece_list ChessApp::SetupPieces(const std::string& xml_settings_file)
{
	piece_list ret{};

	// Black pieces
	ret[static_cast<int>(Pieces::Black_Rook_King_Side)]	 = std::make_unique<Rook>(Pieces::Black_Rook_King_Side,  Pieces::Black_King, "King Side's Black Rook");
	ret[static_cast<int>(Pieces::Black_Rook_Queen_Side)] = std::make_unique<Rook>(Pieces::Black_Rook_Queen_Side, Pieces::Black_King, "Queen Side's Black Rook");
	ret[static_cast<int>(Pieces::Black_Knight_1)] = std::make_unique<Knight>(Pieces::Black_Knight_1, Pieces::Black_King, "Black Knight 1");
	ret[static_cast<int>(Pieces::Black_Knight_2)] = std::make_unique<Knight>(Pieces::Black_Knight_2, Pieces::Black_King, "Black Knight 2");
	ret[static_cast<int>(Pieces::Black_Bishop_1)] = std::make_unique<Bishop>(Pieces::Black_Bishop_1, Pieces::Black_King, "Black Bishop 1");
	ret[static_cast<int>(Pieces::Black_Bishop_2)] = std::make_unique<Bishop>(Pieces::Black_Bishop_2, Pieces::Black_King, "Black Bishop 2");
	ret[static_cast<int>(Pieces::Black_Queen)]	= std::make_unique<Queen>(Pieces::Black_Queen, Pieces::Black_King, "Black Queen");
	ret[static_cast<int>(Pieces::Black_King)]	= std::make_unique<King>(Pieces::Black_King,   Pieces::Black_King, "Black King");
	ret[static_cast<int>(Pieces::Black_Pawn_1)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_1, Pieces::Black_King, "Black Pawn 1");
	ret[static_cast<int>(Pieces::Black_Pawn_2)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_2, Pieces::Black_King, "Black Pawn 2");
	ret[static_cast<int>(Pieces::Black_Pawn_3)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_3, Pieces::Black_King, "Black Pawn 3");
	ret[static_cast<int>(Pieces::Black_Pawn_4)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_4, Pieces::Black_King, "Black Pawn 4");
	ret[static_cast<int>(Pieces::Black_Pawn_5)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_5, Pieces::Black_King, "Black Pawn 5");
	ret[static_cast<int>(Pieces::Black_Pawn_6)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_6, Pieces::Black_King, "Black Pawn 6");
	ret[static_cast<int>(Pieces::Black_Pawn_7)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_7, Pieces::Black_King, "Black Pawn 7");
	ret[static_cast<int>(Pieces::Black_Pawn_8)]	= std::make_unique<Pawn>(Pieces::Black_Pawn_8, Pieces::Black_King, "Black Pawn 8");

	ret[static_cast<int>(Pieces::White_Rook_King_Side)]  = std::make_unique<Rook>(Pieces::White_Rook_King_Side,  Pieces::White_King, "King Side's White Rook");
	ret[static_cast<int>(Pieces::White_Rook_Queen_Side)] = std::make_unique<Rook>(Pieces::White_Rook_Queen_Side, Pieces::White_King, "Queen Side's White Rook");
	ret[static_cast<int>(Pieces::White_Knight_1)] = std::make_unique<Knight>(Pieces::White_Knight_1, Pieces::White_King, "White Knight 1");
	ret[static_cast<int>(Pieces::White_Knight_2)] = std::make_unique<Knight>(Pieces::White_Knight_2, Pieces::White_King, "White Knight 2");
	ret[static_cast<int>(Pieces::White_Bishop_1)] = std::make_unique<Bishop>(Pieces::White_Bishop_1, Pieces::White_King, "White Bishop 1");
	ret[static_cast<int>(Pieces::White_Bishop_2)] = std::make_unique<Bishop>(Pieces::White_Bishop_2, Pieces::White_King, "White Bishop 2");
	ret[static_cast<int>(Pieces::White_Queen)]  = std::make_unique<Queen>(Pieces::White_Queen, Pieces::White_King, "White Queen");
	ret[static_cast<int>(Pieces::White_King)]   = std::make_unique<King>(Pieces::White_King,   Pieces::White_King, "White King");
	ret[static_cast<int>(Pieces::White_Pawn_1)] = std::make_unique<Pawn>(Pieces::White_Pawn_1, Pieces::White_King, "White Pawn 1");
	ret[static_cast<int>(Pieces::White_Pawn_2)] = std::make_unique<Pawn>(Pieces::White_Pawn_2, Pieces::White_King, "White Pawn 2");
	ret[static_cast<int>(Pieces::White_Pawn_3)] = std::make_unique<Pawn>(Pieces::White_Pawn_3, Pieces::White_King, "White Pawn 3");
	ret[static_cast<int>(Pieces::White_Pawn_4)] = std::make_unique<Pawn>(Pieces::White_Pawn_4, Pieces::White_King, "White Pawn 4");
	ret[static_cast<int>(Pieces::White_Pawn_5)] = std::make_unique<Pawn>(Pieces::White_Pawn_5, Pieces::White_King, "White Pawn 5");
	ret[static_cast<int>(Pieces::White_Pawn_6)] = std::make_unique<Pawn>(Pieces::White_Pawn_6, Pieces::White_King, "White Pawn 6");
	ret[static_cast<int>(Pieces::White_Pawn_7)] = std::make_unique<Pawn>(Pieces::White_Pawn_7, Pieces::White_King, "White Pawn 7");
	ret[static_cast<int>(Pieces::White_Pawn_8)] = std::make_unique<Pawn>(Pieces::White_Pawn_8, Pieces::White_King, "White Pawn 8");

	// TODO: read piece attributes from xml file

	return ret;
} // SetupPieces


// ========================================================================================================================================
// Class ChessGame definition
// ========================================================================================================================================

ChessApp::ChessApp(const std::string& initFileName)
	: m_InitFile(initFileName), m_app(initFileName)
{
	// 

	

} // Constructor

ChessApp::~ChessApp()
{
	//// TODO: change this behaviour to something better
	//FILE* f = nullptr;
	//fopen_s(&f, "Fen_Strings.txt", "at"); // Simply append, do not erase contents
	//if (f)
	//{
	//	this->StoreFENString();
	//	fprintf(f, "%s\n", this->m_FEN.c_str());
	//	fclose(f);
	//}
	//else
	//	LOG_ERROR(LOGS::L_FUNCTION_FAILED, "Cannot store FEN string");

	//tinyxml2::XMLDocument file;

	//auto ret = file.LoadFile(this->m_InitFile.c_str());
	//if (ret != tinyxml2::XML_SUCCESS)
	//{
	//	if (ret == tinyxml2::XML_ERROR_FILE_NOT_FOUND ||
	//		ret == tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED ||
	//		ret == tinyxml2::XML_ERROR_FILE_READ_ERROR)
	//	{
	//		LOG_ERROR(LOGS::L_DATA_CORRUPTED, "Could not open init file");
	//	}

	//	std::terminate();
	//}

	//auto initNode = file.FirstChildElement("Initialization");
	//if (initNode)
	//{
	//	auto piecesElem = initNode->FirstChildElement("Pieces");
	//	if (piecesElem)
	//	{
	//		Pieces id = Pieces::Black_Pawn_1;
	//		for (int i = 1; i <= 8; ++i, id = Pieces((int)id +1))
	//		{
	//			// Find Pawn
	//			const Pawn* p = static_cast<Pawn*>(Piece::GetSet()[(size_t)id].get());
	//			Assert(p);
	//			auto pawn = piecesElem->FirstChildElement(p->GetPieceName().c_str());
	//			Assert(pawn);
	//			pawn->SetAttribute("Promotion", (int)p->GetPromotion());
	//		}

	//		id = Pieces::White_Pawn_1;
	//		for (int i = 1; i <= 8; ++i, id = Pieces((int)id + 1))
	//		{
	//			// Find Pawn
	//			const Pawn* p = static_cast<Pawn*>(Piece::GetSet()[(size_t)id].get());
	//			Assert(p);
	//			auto pawn = piecesElem->FirstChildElement(p->GetPieceName().c_str());
	//			Assert(pawn);
	//			pawn->SetAttribute("Promotion", (int)p->GetPromotion());
	//		}
	//	}
	//}
	this->m_Views.clear();
} // Destructor


void ChessApp::OnInitialize()
{
	// Create human view
	this->m_Views.push_back(std::make_unique<HumanView>(this->m_app));

	/*const char* pieceNames[] = {
		"Black_Rook_1", "Black_Rook_2", "Black_Knight_1", "Black_Knight_2", "Black_Bishop_1", "Black_Bishop_2", "Black_Queen", "Black_King",
		"Black_Pawn_1", "Black_Pawn_2", "Black_Pawn_3", "Black_Pawn_4", "Black_Pawn_5", "Black_Pawn_6", "Black_Pawn_7", "Black_Pawn_8",
		"White_Rook_1", "White_Rook_2", "White_Knight_1", "White_Knight_2", "White_Bishop_1", "White_Bishop_2", "White_Queen", "White_King",
		"White_Pawn_1", "White_Pawn_2", "White_Pawn_3", "White_Pawn_4", "White_Pawn_5", "White_Pawn_6", "White_Pawn_7", "White_Pawn_8"
	};*/

	//// Load assets
	//for (size_t id = (size_t)Pieces::Black_Rook_1; id < (size_t)Pieces::NumPieces; ++id)
	//{
	//	Assert(SetUpPieceFromFile(this->m_InitFile, (Pieces)id, pieceNames[id - (size_t)Pieces::Black_Rook_King_Side])
	//		&& "Cannot instantiace piece");
	//}

	
	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnInitialize("");
} // OnInitialize

void ChessApp::OnInput()
{
	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnInput();
} // OnInput


void ChessApp::OnUpdate(std::chrono::nanoseconds delta)
{
	// Update the game state
	this->m_app.Update();

	// Update views
	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnUpdate(delta);
} // OnUpdate


void ChessApp::OnRender()
{
	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnRender();
} // OnRender



//void ChessApp::StoreFENString()
//{
//	// From https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
//	//
//	// A FEN record contains six fields. The separator between fields is a space. The fields are:
//	//	1) Piece placement (from White's perspective). 
//	//			Each rank is described, starting with rank 8 and ending with rank 1;
//	//			within each rank, the contents of each square are described from file "a" through file "h".
//	//			Following the Standard Algebraic Notation (SAN), each piece is identified by a single letter taken from
//	//			the standard English names (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K").
//	//			White pieces are designated using upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk").
//	//			Empty squares are noted using digits 1 through 8 (the number of empty squares), and "/" separates ranks.
//	//
//	//	2) Active color.
//	//			 "w" means White moves next, "b" means Black moves next.
//	// 
//	//	3) Castling availability.
//	//			If neither side can castle, this is "-". Otherwise, this has one or more letters:
//	//			"K" (White can castle kingside),			"Q" (White can castle queenside),
//	//			"k" (Black can castle kingside), and /or	"q" (Black can castle queenside).
//	//			A move that temporarily prevents castling does not negate this notation.
//	//		TODO: for Chess960 games, the FEN castling availability encoding (KQkq) is inadequate for positions
//	//		in which there are two rooks on the same side of the king on the back rank, as if only one rook were
//	//		available for castling it would be ambiguous which rook it was without knowing their initial positions, 
//	//		so the letters of the columns on which the rooks began the game can be used instead (e.g.: HAha instead of KQkq).
//	// 
//	//	4) En passant target square in algebraic notation.
//	//			If there's no en passant target square, this is "-". If a pawn has just made a two-square move, this is the position
//	//			"behind" the pawn. This is recorded regardless of whether there is a pawn in position to make an en passant capture.
//	// 
//	//	5) Halfmove clock.
//	//			The number of halfmoves since the last capture or pawn advance, used for the fifty - move rule.
//	// 
//	//	6) Fullmove number.
//	//			The number of the full move. It starts at 1, and is incremented after Black's move.
//	int i, j;
//	int num_spaces;
//	this->m_FEN = "";
//
//	// Board status
//	for (i = 0; i < BOARD_SIDE; ++i)
//	{
//		num_spaces = 0;
//		for (j = 0; j < BOARD_SIDE; ++j)
//		{
//			const Piece* curr = this->m_game.m_game.m_Board.m_Board[i * BOARD_SIDE + j];
//			if (curr == nullptr)
//				num_spaces++;
//			else
//			{
//				if (num_spaces != 0)
//				{
//					// num_spaces will never be > 8 (which is BOARD_SIDE)
//					this->m_FEN += '0' + num_spaces;
//					num_spaces = 0;
//				}
//
//				this->m_FEN += this->MapPieceIDToChar(curr->GetPieceID());
//			}
//		}
//
//		// End of row
//		if (num_spaces != 0)
//		{
//			// num_spaces will never be > 8 (which is BOARD_SIDE)
//			this->m_FEN += '0' + num_spaces;
//			num_spaces = 0;
//		}
//		this->m_FEN += '/';
//	}
//
//	// Remove last slash and set active player
//	this->m_FEN[this->m_FEN.size() - 1] = ' ';
//	this->m_FEN += (this->m_Turn ? "w " : "b ");
//
//	// Castling
//	if (!this->m_game.m_game.m_Board.m_Black_Castle &&
//		!this->m_game.m_game.m_Board.m_Black_CastleLong &&
//		!this->m_game.m_game.m_Board.m_White_Castle &&
//		!this->m_game.m_game.m_Board.m_White_CastleLong)
//	{
//		// No castlings available
//		this->m_FEN += '-';
//	}
//	else
//	{
//		// Order doesn't matter
//		if (this->m_game.m_game.m_White_Castle)		this->m_FEN += 'K';
//		if (this->m_game.m_game.m_White_CastleLong)	this->m_FEN += 'Q';
//		if (this->m_game.m_game.m_Black_Castle)		this->m_FEN += 'k';
//		if (this->m_game.m_game.m_Black_CastleLong)	this->m_FEN += 'q';
//	}
//
//	// En Passant
//	if (0 <= this->m_game.m_game.m_Board.m_en_passant_target_pos && this->m_game.m_game.m_Board.m_en_passant_target_pos < this->m_game.m_game.m_Board.m_Board.size())
//	{
//		// e.g.: row = 5 and col = 3
//		int row = this->m_game.m_game.m_en_passant_target_pos / BOARD_SIDE; // row = '3' = '0' + (BOARD_SIDE - 5)
//		int col = this->m_game.m_game.m_en_passant_target_pos % BOARD_SIDE; // col = 'd' = 'a' + 4
//		
//		this->m_FEN += ' '; 
//		this->m_FEN += 'a' + col;
//		this->m_FEN += '0' + (BOARD_SIDE - row);
//	}
//	else
//	{
//		// No en passants available
//		this->m_FEN += " -";
//	}
//
//	// Half moves and full moves
//	this->m_FEN += ' ' + std::to_string(this->m_Half_Moves) + ' ' + std::to_string(this->m_Full_Moves);
//} // StoreFENString