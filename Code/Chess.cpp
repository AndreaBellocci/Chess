// Milan, 6th December 2021
//

#include <raylib.h>
#include "ChessEvents.h"
#include "Chess.h"
#include "../Libraries/Logger.h"
using namespace ENGINE_NAMESPACE;

//	Definitions for the board, we use FEN notation (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
//	to define every piece on the board and an empty space to indicate that no piece is in that position
#define EMPTY			' '
#define WHITE_PAWN		'P'
#define WHITE_KNIGHT	'N'
#define WHITE_BISHOP	'B'
#define WHITE_ROOK		'R'
#define WHITE_QUEEN		'Q'
#define WHITE_KING		'K'
#define BLACK_PAWN		'p'
#define BLACK_KNIGHT	'n'
#define BLACK_BISHOP	'b'
#define BLACK_ROOK		'r'
#define BLACK_QUEEN		'q'
#define BLACK_KING		'k'

EventManager& GetEventManager()
{
	static EventManager manager(true);
	return manager;
} // GetEventManager

ENGINE_NAMESPACE::EventManager& ChessGame::s_GlobalEventManager = GetEventManager();

#if DEBUGGING
static void PrintBoardOnTerminal(const board_t& board)
{
	printf("\nCurrent Board:\n");
	char c = '\0';
	for (int row = 0; row < BOARD_SIDE; row++)
	{
		for (int col = 0; col < BOARD_SIDE; col++)
		{
			const Piece* curr = board[row * BOARD_SIDE + col];
			if (curr)
			{
				Pieces id = curr->GetPieceID();

				switch (id)
				{
				case Pieces::Black_Rook_1:		case Pieces::Black_Rook_2:		c = BLACK_ROOK;		break;
				case Pieces::Black_Knight_1:	case Pieces::Black_Knight_2:	c = BLACK_KNIGHT;	break;
				case Pieces::Black_Bishop_1:	case Pieces::Black_Bishop_2:	c = BLACK_BISHOP;	break;
				case Pieces::Black_Queen:										c = BLACK_QUEEN;	break;
				case Pieces::Black_King:										c = BLACK_KING;		break;
				case Pieces::Black_Pawn_1: case Pieces::Black_Pawn_2: case Pieces::Black_Pawn_3: case Pieces::Black_Pawn_4:
				case Pieces::Black_Pawn_5: case Pieces::Black_Pawn_6: case Pieces::Black_Pawn_7: case Pieces::Black_Pawn_8:
					c = BLACK_PAWN; break;

				case Pieces::White_Rook_1:		case Pieces::White_Rook_2:		c = WHITE_ROOK;		break;
				case Pieces::White_Knight_1:	case Pieces::White_Knight_2:	c = WHITE_KNIGHT;	break;
				case Pieces::White_Bishop_1:	case Pieces::White_Bishop_2:	c = WHITE_BISHOP;	break;
				case Pieces::White_Queen:										c = WHITE_QUEEN;	break;
				case Pieces::White_King:										c = WHITE_KING;		break;
				case Pieces::White_Pawn_1: case Pieces::White_Pawn_2: case Pieces::White_Pawn_3: case Pieces::White_Pawn_4:
				case Pieces::White_Pawn_5: case Pieces::White_Pawn_6: case Pieces::White_Pawn_7: case Pieces::White_Pawn_8:
					c = WHITE_PAWN; break;

				default:
					c = '?';
				} // Switch
			}
			else
				c = EMPTY;
			printf(" %c ", c);
		} // Inner for
		printf("\n");
	} // Outer for

	printf("\n");
} // PrintBoardOnTerminal
#else
#define PrintBoardOnTerminal(board) do { void(0); } while(0);
#endif

// ========================================================================================================================================
// Class ChessGame definition
// ========================================================================================================================================

ChessGame::ChessGame(const std::string& initFileName)
	: m_InitFile(initFileName)
{
	for (int i = 0; i < this->m_Board.m_Board.size(); ++i)
		this->m_Board.m_Board[i] = nullptr;

	// Get starting FEN string
	tinyxml2::XMLDocument file;
	auto ret = file.LoadFile(this->m_InitFile.c_str());
	if (ret != tinyxml2::XML_SUCCESS)
	{
		if (ret == tinyxml2::XML_ERROR_FILE_NOT_FOUND ||
			ret == tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED ||
			ret == tinyxml2::XML_ERROR_FILE_READ_ERROR)
		{
			LOG_ERROR(LOGS::L_DATA_CORRUPTED, "Could not open init file");
		}

		std::terminate();
	}

	auto init = file.FirstChildElement("Initialization");
	if (init)
	{
		auto fen = init->FirstChildElement("FEN");
		if (fen)
		{
			auto str = fen->FindAttribute("string");
			if (str)
			{
				this->m_FEN = str->Value();
			}
		}
	}

} // Constructor

ChessGame::~ChessGame()
{
	// TODO: change this behaviour to something better
	FILE* f = nullptr;
	fopen_s(&f, "Fen_Strings.txt", "at"); // Simply append, do not erase contents
	if (f)
	{
		this->StoreFENString();
		fprintf(f, "%s\n", this->m_FEN.c_str());
		fclose(f);
	}
	else
		LOG_ERROR(LOGS::L_FUNCTION_FAILED, "Cannot store FEN string");

	tinyxml2::XMLDocument file;

	auto ret = file.LoadFile(this->m_InitFile.c_str());
	if (ret != tinyxml2::XML_SUCCESS)
	{
		if (ret == tinyxml2::XML_ERROR_FILE_NOT_FOUND ||
			ret == tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED ||
			ret == tinyxml2::XML_ERROR_FILE_READ_ERROR)
		{
			LOG_ERROR(LOGS::L_DATA_CORRUPTED, "Could not open init file");
		}

		std::terminate();
	}

	auto initNode = file.FirstChildElement("Initialization");
	if (initNode)
	{
		auto piecesElem = initNode->FirstChildElement("Pieces");
		if (piecesElem)
		{
			Pieces id = Pieces::Black_Pawn_1;
			for (int i = 1; i <= 8; ++i, id = Pieces((int)id +1))
			{
				// Find Pawn
				const Pawn* p = static_cast<Pawn*>(Piece::GetSet()[(size_t)id].get());
				Assert(p);
				auto pawn = piecesElem->FirstChildElement(p->GetPieceName().c_str());
				Assert(pawn);
				pawn->SetAttribute("Promotion", (int)p->GetPromotion());
			}

			id = Pieces::White_Pawn_1;
			for (int i = 1; i <= 8; ++i, id = Pieces((int)id + 1))
			{
				// Find Pawn
				const Pawn* p = static_cast<Pawn*>(Piece::GetSet()[(size_t)id].get());
				Assert(p);
				auto pawn = piecesElem->FirstChildElement(p->GetPieceName().c_str());
				Assert(pawn);
				pawn->SetAttribute("Promotion", (int)p->GetPromotion());
			}
		}
	}
} // Destructor


void ChessGame::OnInitialize()
{
	// Create human view
	this->m_Views.push_back(std::unique_ptr<IGameView>(ENGINE_NEW HumanView(this->m_Board, this->m_Targets)));

	const char* pieceNames[] = {
		"Black_Rook_1", "Black_Rook_2", "Black_Knight_1", "Black_Knight_2", "Black_Bishop_1", "Black_Bishop_2", "Black_Queen", "Black_King",
		"Black_Pawn_1", "Black_Pawn_2", "Black_Pawn_3", "Black_Pawn_4", "Black_Pawn_5", "Black_Pawn_6", "Black_Pawn_7", "Black_Pawn_8",
		"White_Rook_1", "White_Rook_2", "White_Knight_1", "White_Knight_2", "White_Bishop_1", "White_Bishop_2", "White_Queen", "White_King",
		"White_Pawn_1", "White_Pawn_2", "White_Pawn_3", "White_Pawn_4", "White_Pawn_5", "White_Pawn_6", "White_Pawn_7", "White_Pawn_8"
	};

	// Load assets
	for (size_t id = (size_t)Pieces::Black_Rook_1; id < (size_t)Pieces::NumPieces; ++id)
	{
		Assert(SetUpPieceFromFile(this->m_InitFile, (Pieces)id, pieceNames[id - (size_t)Pieces::Black_Rook_1])
			&& "Cannot instantiace piece");
	}

	this->LoadFENString();

	// Register events listeners
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnSelectionReset),	Event_SelectionReset::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnPieceSelected),		Event_PieceSelected::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnIllegalMove),		Event_IllegalMove::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnStartMovePiece),	Event_StartMovePiece::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnCastle),			Event_Castle::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnEndMovePiece),		Event_EndMovePiece::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnPieceEaten),		Event_PieceEaten::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &ChessGame::OnEndTurn),			Event_EndTurn::GetEventType());

	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnInitialize();
} // OnInitialize

void ChessGame::OnInput()
{
	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnInput();
} // OnInput

void ChessGame::OnUpdate(ENGINE_NAMESPACE::nanoseconds delta)
{
	s_GlobalEventManager.Update();

	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnUpdate(delta);
} // OnUpdate


void ChessGame::OnRender()
{
	auto it = this->m_Views.begin();
	for (it; it != this->m_Views.end(); ++it)
		(*it)->VOnRender();
} // OnRender


Piece* ChessGame::MapCharToPiece(char c) const
{
	static short blacks[6] = { 0, 0, 0, 0, 0, 0 };
	static short whites[6] = { 0, 0, 0, 0, 0, 0 };

#if DEBUGGING
	for (int i = 0; i < 6; ++i)
	{
		if ((i == 5 && blacks[i] > 8) || (i != 5 && blacks[i] > 2))
			LOG_ERROR(LOGS::L_DATA_CORRUPTED, "FEN string is incorrect");

		if ((i == 5 && whites[i] > 8) || (i != 5 && whites[i] > 2))
			LOG_ERROR(LOGS::L_DATA_CORRUPTED, "FEN string is incorrect");
	}
#endif

	switch (c)
	{
	case BLACK_ROOK:	return Piece::GetSet()[0 + blacks[0]++].get();
	case BLACK_KNIGHT:	return Piece::GetSet()[2 + blacks[1]++].get();
	case BLACK_BISHOP:	return Piece::GetSet()[4 + blacks[2]++].get();
	case BLACK_QUEEN:	return Piece::GetSet()[6 + blacks[3]++].get();
	case BLACK_KING:	return Piece::GetSet()[7 + blacks[4]++].get();
	case BLACK_PAWN:	return Piece::GetSet()[8 + blacks[5]++].get();

	case WHITE_ROOK:	return Piece::GetSet()[16 + whites[0]++].get();
	case WHITE_KNIGHT:	return Piece::GetSet()[18 + whites[1]++].get();
	case WHITE_BISHOP:	return Piece::GetSet()[20 + whites[2]++].get();
	case WHITE_QUEEN:	return Piece::GetSet()[22 + whites[3]++].get();
	case WHITE_KING:	return Piece::GetSet()[23 + whites[4]++].get();
	case WHITE_PAWN:	return Piece::GetSet()[24 + whites[5]++].get();

	case EMPTY: 
		for (int i = 0; i < 6; ++i)
			blacks[i] = whites[i] = 0;
		return nullptr;

	default:
		LOG_WARNING(LOGS::L_INFO, "Shouldn't get here...");
		return nullptr;	
	}
} // MapCharToPiece

char ChessGame::MapPieceIDToChar(Pieces id) const
{
	char c = '\0';
	switch (id)
	{
	case Pieces::Black_Rook_1:		case Pieces::Black_Rook_2:		c = BLACK_ROOK;		break;
	case Pieces::Black_Knight_1:	case Pieces::Black_Knight_2:	c = BLACK_KNIGHT;	break;
	case Pieces::Black_Bishop_1:	case Pieces::Black_Bishop_2:	c = BLACK_BISHOP;	break;
	case Pieces::Black_Queen:										c = BLACK_QUEEN;	break;
	case Pieces::Black_King:										c = BLACK_KING;		break;
	case Pieces::Black_Pawn_1: case Pieces::Black_Pawn_2: case Pieces::Black_Pawn_3: case Pieces::Black_Pawn_4:
	case Pieces::Black_Pawn_5: case Pieces::Black_Pawn_6: case Pieces::Black_Pawn_7: case Pieces::Black_Pawn_8:
		c = BLACK_PAWN; break;

	case Pieces::White_Rook_1:		case Pieces::White_Rook_2:		c = WHITE_ROOK;		break;
	case Pieces::White_Knight_1:	case Pieces::White_Knight_2:	c = WHITE_KNIGHT;	break;
	case Pieces::White_Bishop_1:	case Pieces::White_Bishop_2:	c = WHITE_BISHOP;	break;
	case Pieces::White_Queen:										c = WHITE_QUEEN;	break;
	case Pieces::White_King:										c = WHITE_KING;		break;
	case Pieces::White_Pawn_1: case Pieces::White_Pawn_2: case Pieces::White_Pawn_3: case Pieces::White_Pawn_4:
	case Pieces::White_Pawn_5: case Pieces::White_Pawn_6: case Pieces::White_Pawn_7: case Pieces::White_Pawn_8:
		c = WHITE_PAWN; break;

	default:
		c = '?';
	} // Switch

	return c;
} // MapPieceIDToChar

void ChessGame::LoadFENString()
{
	// See notes on StoreFENString
	int row = 0;
	int col = 0;

	size_t index = 0;
	try
	{
		while (true) // Loop will terminate when the first space is reached
		{
			char current = this->m_FEN[index]; // Get current fen string character

			if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z'))
			{
				// If the char is a letter then it must be a piece	
				Piece* p = this->MapCharToPiece(current);
				if (p)
				{
					this->m_Board.m_Board[row * BOARD_SIDE + col] = p;
					p->SetAlive(true);
				}
				else
					LOG_ERROR(LOGS::L_INVALID_POINTER, "No piece found");

				col++;
			}
			else if (current == '/')
			{
				// Go to start of next row
				row++;
				col = 0;
			}
			else if ('0' <= current && current <= '9')
			{
				// If the char is a number then it's the number of empty cells
				for (int i = 0; i < current - '0'; i++)
				{
					this->m_Board.m_Board[row * BOARD_SIDE + col] = nullptr;
					col++;
				}
			}
			else
			{
				// if we reach a space we are done with the pieces
				this->MapCharToPiece(EMPTY);
				break; // Next chars will be checked manually
			}

			++index;
		} // while 

		if (!Piece::GetSet()[(size_t)Pieces::Black_King]->IsAlive() || !Piece::GetSet()[(size_t)Pieces::White_King]->IsAlive())
		{
			LOG_ERROR(LOGS::L_DATA_CORRUPTED, "Missing kings...");
			std::terminate();
		}

		// Turn indicator is right after the first empty space - this fiels must exist
		index++;
		this->m_Turn = this->m_FEN[index] == 'w' || this->m_FEN[index] == 'W';
		index++; // Space

	Castling:
		index++;
		switch (this->m_FEN[index])
		{
		case 'K': this->m_Board.m_White_Castle = true;		goto Castling;
		case 'k': this->m_Board.m_Black_Castle = true;		goto Castling;
		case 'Q': this->m_Board.m_White_CastleLong = true;	goto Castling;
		case 'q': this->m_Board.m_Black_CastleLong = true;	goto Castling;

		case '-':
			this->m_Board.m_White_Castle = false;
			this->m_Board.m_Black_Castle = false;
			this->m_Board.m_White_CastleLong = false;
			this->m_Board.m_Black_CastleLong = false;
			++index; // Skip space, fall through
		case ' ': goto En_Passant;
		default:
			LOG_ERROR(LOGS::L_DATA_CORRUPTED, "Incorrect FEN string: unrecognised token for castling");
		} // Switch

	En_Passant:
		index++;
		if (this->m_FEN[index] == '-')
		{
			this->m_Board.m_EnPassantIndex = INVALID;
			index += 2; // Also skip next space
		}
		else
		{
			// e.g: e3
			int col = this->m_FEN[index] - 'a';						// e => col = 4			= 'e' - 'a'
			int row = BOARD_SIDE - (this->m_FEN[index + 1] - '0');	// 3 => row = 5 = 8 - 3 = BOARD_SIDE - ('3' - '0')
			this->m_Board.m_EnPassantIndex = row * BOARD_SIDE + col;
			index += 3; // Also skip next space
		}

		ThrowIfFalse(sscanf_s(&this->m_FEN[index], "%d %d", &this->m_Half_Moves, &this->m_Full_Moves) == 2,
			LOGS::L_DATA_CORRUPTED, "Incorrect values");;
	} // end try
	catch (...)
	{
		Assert(false && "Could not read FEN string");
	} // end generic catch
} // LoadFENString

void ChessGame::StoreFENString()
{
	// From https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
	//
	// A FEN record contains six fields. The separator between fields is a space. The fields are:
	//	1) Piece placement (from White's perspective). 
	//			Each rank is described, starting with rank 8 and ending with rank 1;
	//			within each rank, the contents of each square are described from file "a" through file "h".
	//			Following the Standard Algebraic Notation (SAN), each piece is identified by a single letter taken from
	//			the standard English names (pawn = "P", knight = "N", bishop = "B", rook = "R", queen = "Q" and king = "K").
	//			White pieces are designated using upper-case letters ("PNBRQK") while black pieces use lowercase ("pnbrqk").
	//			Empty squares are noted using digits 1 through 8 (the number of empty squares), and "/" separates ranks.
	//
	//	2) Active color.
	//			 "w" means White moves next, "b" means Black moves next.
	// 
	//	3) Castling availability.
	//			If neither side can castle, this is "-". Otherwise, this has one or more letters:
	//			"K" (White can castle kingside),			"Q" (White can castle queenside),
	//			"k" (Black can castle kingside), and /or	"q" (Black can castle queenside).
	//			A move that temporarily prevents castling does not negate this notation.
	//		TODO: for Chess960 games, the FEN castling availability encoding (KQkq) is inadequate for positions
	//		in which there are two rooks on the same side of the king on the back rank, as if only one rook were
	//		available for castling it would be ambiguous which rook it was without knowing their initial positions, 
	//		so the letters of the columns on which the rooks began the game can be used instead (e.g.: HAha instead of KQkq).
	// 
	//	4) En passant target square in algebraic notation.
	//			If there's no en passant target square, this is "-". If a pawn has just made a two-square move, this is the position
	//			"behind" the pawn. This is recorded regardless of whether there is a pawn in position to make an en passant capture.
	// 
	//	5) Halfmove clock.
	//			The number of halfmoves since the last capture or pawn advance, used for the fifty - move rule.
	// 
	//	6) Fullmove number.
	//			The number of the full move. It starts at 1, and is incremented after Black's move.

	int i, j;
	int num_spaces;
	this->m_FEN = "";

	// Board status
	for (i = 0; i < BOARD_SIDE; ++i)
	{
		num_spaces = 0;
		for (j = 0; j < BOARD_SIDE; ++j)
		{
			const Piece* curr = this->m_Board.m_Board[i * BOARD_SIDE + j];
			if (curr == nullptr)
				num_spaces++;
			else
			{
				if (num_spaces != 0)
				{
					// num_spaces will never be > 8 (which is BOARD_SIDE)
					this->m_FEN += '0' + num_spaces;
					num_spaces = 0;
				}

				this->m_FEN += this->MapPieceIDToChar(curr->GetPieceID());
			}
		}

		// End of row
		if (num_spaces != 0)
		{
			// num_spaces will never be > 8 (which is BOARD_SIDE)
			this->m_FEN += '0' + num_spaces;
			num_spaces = 0;
		}
		this->m_FEN += '/';
	}

	// Remove last slash and set active player
	this->m_FEN[this->m_FEN.size() - 1] = ' ';
	this->m_FEN += (this->m_Turn ? "w " : "b ");

	// Castling
	if (!this->m_Board.m_Black_Castle &&
		!this->m_Board.m_Black_CastleLong &&
		!this->m_Board.m_White_Castle &&
		!this->m_Board.m_White_CastleLong)
	{
		// No castlings available
		this->m_FEN += '-';
	}
	else
	{
		// Order doesn't matter
		if (this->m_Board.m_White_Castle)		this->m_FEN += 'K';
		if (this->m_Board.m_White_CastleLong)	this->m_FEN += 'Q';
		if (this->m_Board.m_Black_Castle)		this->m_FEN += 'k';
		if (this->m_Board.m_Black_CastleLong)	this->m_FEN += 'q';
	}

	// En Passant
	if (0 <= this->m_Board.m_EnPassantIndex && this->m_Board.m_EnPassantIndex < this->m_Board.m_Board.size())
	{
		// e.g.: row = 5 and col = 3
		int row = this->m_Board.m_EnPassantIndex / BOARD_SIDE; // row = '3' = '0' + (BOARD_SIDE - 5)
		int col = this->m_Board.m_EnPassantIndex % BOARD_SIDE; // col = 'd' = 'a' + 4
		
		this->m_FEN += ' '; 
		this->m_FEN += 'a' + col;
		this->m_FEN += '0' + (BOARD_SIDE - row);
	}
	else
	{
		// No en passants available
		this->m_FEN += " -";
	}

	// Half moves and full moves
	this->m_FEN += ' ' + std::to_string(this->m_Half_Moves) + ' ' + std::to_string(this->m_Full_Moves);
} // StoreFENString


void ChessGame::OnPieceSelected(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_PieceSelected& event = *static_cast<Event_PieceSelected*>(pEvent.get());
	const Piece* selected = this->m_Board.m_Board[event.m_Index];

	if (selected)
	{
		// Only process pieces belonging to the current player
		Pieces id = selected->GetPieceID();

		if ((this->m_Turn == false	&& Pieces::Black_Rook_1 <= id && id <= Pieces::Black_Pawn_8) || // Black player
			(this->m_Turn == true	&& Pieces::White_Rook_1 <= id && id <= Pieces::White_Pawn_8)) // White player
		{
			// Build tree of possible moves
			this->m_Targets.clear();
			selected->BuildPossibleMoves(event.m_Index, this->m_Board, this->m_Targets, this->m_AllowChecks);
			printf("Total of %lld possible moves.\n\n", this->m_Targets.size());
		}
		else
		{
			// Clear selection if opponent's piece was selected
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectionReset()));
		}
	}
	else
		LOG_ERROR(LOGS::L_INVALID_POINTER, "Event pointer was NULL");
} // OnPieceSelected Listener

void ChessGame::OnSelectionReset(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	this->m_Targets.clear();
} // OnSelectionReset Listener

void ChessGame::OnIllegalMove(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	printf("Illegal move\n");
} // OnIllegalMove Listener

void ChessGame::OnStartMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	const Event_StartMovePiece& event = *static_cast<Event_StartMovePiece*>(pEvent.get());
	const Pieces piece_id = event.m_Piece->GetPieceID();

	// Look for king castling
	if (piece_id == Pieces::Black_King || piece_id == Pieces::White_King && (this->m_Board.m_Black_Castle ||
		this->m_Board.m_Black_CastleLong || this->m_Board.m_White_Castle || this->m_Board.m_White_CastleLong))
	{
		// Find and move also the corresponding rook
		Pieces rook_id = Pieces::Null;
		int rook_index = INVALID;
		Piece* rook = nullptr;

		// Black Queen-side castle: king in c8 (i.e.: index = 2) and rook in d8 (i.e.: index = 3)
		// Black King-side castle: king in g8 (i.e.: index = 6) and rook in f8 (i.e.: index = 5)	
		// White Queen-side castle: king in c1 (i.e.: index = 58) and rook in d1 (i.e.: index = 59)
		// White King-side castle: king in g1 (i.e.: index = 62) and rook in f1 (i.e.: index = 61)		
		switch (event.m_Dest)
		{
		case 2:		rook_index = 3;		rook_id = Pieces::Black_Rook_1;  rook = Piece::GetSet()[(size_t)rook_id].get();	break;
		case 6:		rook_index = 5;		rook_id = Pieces::Black_Rook_2;  rook = Piece::GetSet()[(size_t)rook_id].get();	break;
		case 58:	rook_index = 59;	rook_id = Pieces::White_Rook_1;  rook = Piece::GetSet()[(size_t)rook_id].get();	break;
		case 62:	rook_index = 61;	rook_id = Pieces::White_Rook_2;  rook = Piece::GetSet()[(size_t)rook_id].get();	break;
		default: goto Next; // Not castling
		}

		if (rook->IsAlive())
		{
			// Find rook on board
			int where_rook;
			for (where_rook = 0; ; ++where_rook) // If the rook's alive, it must be on the board, so it isn't necessary to check
				if (this->m_Board.m_Board[where_rook] != nullptr && this->m_Board.m_Board[where_rook]->GetPieceID() == rook_id)
					break;

			// Prevent player from castling again
			if (piece_id == Pieces::Black_King)
				this->m_Board.m_Black_Castle = this->m_Board.m_Black_CastleLong = false;
			else
				this->m_Board.m_White_Castle = this->m_Board.m_White_CastleLong = false;

			// Move the rook
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_Castle(rook, where_rook, rook_index)));
		}
	}
	else if ((Pieces::Black_Pawn_1 <= piece_id && piece_id <= Pieces::Black_Pawn_8) ||  // Look for possible en passants 
			(Pieces::White_Pawn_1 <= piece_id && piece_id <= Pieces::White_Pawn_8))
	{
		// If capturing en passant
		if (this->m_Board.m_EnPassantIndex == event.m_Dest)
		{
			if (Pieces::White_Pawn_1 <= piece_id) // White pawn eat up so look for captured pawn down
				s_GlobalEventManager.TriggerEvent(IEventPtr(
					ENGINE_NEW Event_PieceEaten(this->m_Board.m_Board[this->m_Board.m_EnPassantIndex + BOARD_SIDE])));
			else // Black pawn eat down so look for captured pawn up
				s_GlobalEventManager.TriggerEvent(IEventPtr(
					ENGINE_NEW Event_PieceEaten(this->m_Board.m_Board[this->m_Board.m_EnPassantIndex - BOARD_SIDE])));

			this->m_Board.m_EnPassantIndex = INVALID;
		}
		else // Super sprint
		{
			const int start_row = event.m_Src / BOARD_SIDE;
			const int end_row = event.m_Dest / BOARD_SIDE;

			if (start_row - end_row == 2 || start_row - end_row == -2)
			{
				// Store the position "behind" the pawn
				this->m_Board.m_EnPassantIndex = ((start_row + end_row) / 2) * BOARD_SIDE + (event.m_Dest % BOARD_SIDE);
			}
			else 
				this->m_Board.m_EnPassantIndex = INVALID;
		}
	}
	else // Every other move reset en passant index
		this->m_Board.m_EnPassantIndex = INVALID;

Next:
	this->m_Board.m_Board[event.m_Src] = nullptr; // Piece is not anymore there
	s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_EndMovePiece(event.m_Piece, event.m_Dest)));
} // OnStartMovePiece Listener

void ChessGame::OnCastle(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	const Event_Castle& event = *static_cast<Event_Castle*>(pEvent.get());

	// Update board 
	this->m_Board.m_Board[event.m_Src] = nullptr; // Piece is not anymore there
	this->m_Board.m_Board[event.m_Dest] = event.m_Piece;
} // OnCastle

void ChessGame::OnEndMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	const Event_EndMovePiece& event = *static_cast<Event_EndMovePiece*>(pEvent.get());
	const Pieces id = event.m_Piece->GetPieceID();

	// If a piece was eaten, fire an event
	if (this->m_Board.m_Board[event.m_Dest] != nullptr)
	{
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_PieceEaten(this->m_Board.m_Board[event.m_Dest])));
		this->m_Half_Moves = 0;
	}
	else
	{
		if (!(Pieces::White_Pawn_1 <= id && id <= Pieces::White_Pawn_8) &&
			!(Pieces::Black_Pawn_1 <= id && id <= Pieces::Black_Pawn_8))
		{
			// Increment number of half moves after each move that didn't involve a pawn
			this->m_Half_Moves++;
		}
	}

	// If a pawn needs to be promoted, fire an event
	if ((this->m_Turn == true &&										// White turn and
			Pieces::White_Pawn_1 <= id && id <= Pieces::White_Pawn_8 &&	// White pawn and
			event.m_Dest / BOARD_SIDE == 0) ||							// On first rank	or
		(this->m_Turn == false &&										// Black turn and
			Pieces::Black_Pawn_1 <= id && id <= Pieces::Black_Pawn_8 && // Black pawn and
			event.m_Dest / BOARD_SIDE == BOARD_SIDE - 1))				// On last rank
	{
		auto pawn = static_cast<Pawn*>(event.m_Piece);
		
		if (!pawn->HasBeenPromoted())
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_PawnPromotion(pawn)));
	}
	
	// Update board 
	this->m_Board.m_Board[event.m_Dest] = event.m_Piece;

	s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_EndTurn()));
} // OnEndMovePiece Listener

//void ChessGame::OnPawnPromotion(const ENGINE_NAMESPACE::IEventPtr& pEvent)
//{
//	const Event_PawnPromotion& event = *static_cast<Event_PawnPromotion*>(pEvent.get());
//	this->m_PawnToPromote->Promote(event.m_Promotion);
//	this->m_PawnToPromote = nullptr;
//} // OnPawnPromotion Listener

void ChessGame::OnEndTurn(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Reset selection
	s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectionReset()));

	// Print board and piece status for debug purpose
#if DEBUGGING
	PrintBoardOnTerminal(this->m_Board.m_Board);
	const auto& set = Piece::GetSet();
	int dead = 0, alive = 0;
	for (int i = 0; i < set.size(); ++i)
	{
		if (set[i]->IsAlive())
		{
			printf("%s is alive\n", set[i]->GetPieceName().c_str());
			++alive;
		}
		else
		{
			printf("%s is dead\n", set[i]->GetPieceName().c_str());
			++dead;
		}
	}

	printf("Dead pieces: %d Alive: %d\n\n\n", dead, alive);
#endif
	
	// If king is now under check
	int where_king = INVALID;
	if (this->m_Turn) // Find black king
	{
		for (int i = 0; i < this->m_Board.m_Board.size(); ++i)
			if (this->m_Board.m_Board[i] && this->m_Board.m_Board[i]->GetPieceID() == Pieces::Black_King)
			{ where_king = i; break; }
	}
	else // Find white king
	{
		for (int i = 0; i < this->m_Board.m_Board.size(); ++i)
			if (this->m_Board.m_Board[i] && this->m_Board.m_Board[i]->GetPieceID() == Pieces::White_King)
			{ where_king = i; break; }
	}
	Assert(where_king != INVALID && "King not on board");

	if (IsKingUnderCheck(this->m_Board.m_Board, where_king))
	{
		if (CanPlayerMove(!this->m_Turn, this->m_Board)) //  Player didn't lose, fire an event
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_Check(where_king)));
		else // Player lost
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_EndMatch((Event_EndMatch::END_STATUS)this->m_Turn)));
	}
	else if (!CanPlayerMove(!this->m_Turn, this->m_Board) || this->m_Half_Moves >= 50) // Stalemate or fifty moves rule
	{
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_EndMatch(Event_EndMatch::END_STATUS::Stalemate)));
	}

	// Increment number of full moves after each black move
	if(!this->m_Turn)
		this->m_Full_Moves++;
	
	// Change turn
	this->m_Turn = !this->m_Turn;

	printf("End turn\n");
} // OnEndTurn Listener

void ChessGame::OnPieceEaten(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	const Event_PieceEaten& event = *static_cast<Event_PieceEaten*>(pEvent.get());

	event.m_Piece->Die();
	printf("Piece Eaten: %s\n", event.m_Piece->GetPieceName().c_str());
} // OnPieceEaten Listener



#undef EMPTY
#undef WHITE_PAWN
#undef WHITE_KNIGHT
#undef WHITE_BISHOP
#undef WHITE_ROOK
#undef WHITE_QUEEN
#undef WHITE_KING
#undef BLACK_PAWN
#undef BLACK_KNIGHT
#undef BLACK_BISHOP
#undef BLACK_ROOK
#undef BLACK_QUEEN
#undef BLACK_KING