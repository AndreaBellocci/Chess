// Milan, 10th December 2021
// 

#include "../Libraries/Logger.h"
#include "Chess.h"
#include "ChessEvents.h"
#include "GameViews.h"
#include "Pieces/piece_utilities.h"
#include "ErrorCodes.h"
using namespace ENGINE_NAMESPACE;


// ========================================================================================================================================
// Textures Data
// ========================================================================================================================================
class TextureManager
{
public:
	~TextureManager()
	{
		for (auto& tex : this->textures)
			UnloadTexture(tex);
	} // Destructor

	void SetTextureSize(int width, int height) noexcept
	{
		this->tex_width = width;
		this->tex_height = height;
	} // SetTextureSize

	void GetTextureSize(int& width, int& height) const noexcept
	{
		width = this->tex_width;
		height = this->tex_height;
	} // GetTextureSize

	const std::array<Texture2D, num_piece_types>& GetTextures() const noexcept
	{
		return this->textures;
	} // GetTextures

	const Texture2D* GetTexture(PieceType type) const noexcept
	{
		return &this->textures[static_cast<size_t>(type)];
	} // GetTexture

	const Texture2D* LoadPieceTextureFromFile(PieceType type, const std::string& fname)
	{
		// Create an image from a file in memory
		Image image = LoadImage(fname.c_str());

		// Resize image to the desired width and height
		ImageResize(&image, this->tex_width, this->tex_height);

		// then create a texture in vram from said image and sore it in the texture array
		Texture2D texture = LoadTextureFromImage(image);

		// Dealocate the image
		UnloadImage(image);

		this->textures[static_cast<size_t>(type)] = texture;
		return &this->textures[static_cast<size_t>(type)];
	} // LoadPieceTextureFromFile

private:
	// Textures
	std::array<Texture2D, num_piece_types> textures;

	int tex_width;
	int tex_height;
} g_TextureManager;



// ========================================================================================================================================
// Class PawnPromoter Definition
// ========================================================================================================================================
void PawnPromoter::PromotePawn(Pawn* const pawn)
{
	Assert(pawn);
	this->m_PawnToPromote = pawn;

	this->m_Visible = true;
} // PromotePawn

void PawnPromoter::OnInput(int hovering_x, int hovering_y, bool selected)
{
	if (this->m_Visible && hovering_x != INVALID && hovering_y != INVALID)
	{
		int w, h;
		g_TextureManager.GetTextureSize(w, h);

		const auto first_x = this->m_Start_x;
		const auto first_y = this->m_Start_y;
		const auto second_x = this->m_Start_x + w;
		const auto second_y = this->m_Start_y + h;
		const auto end_x = this->m_Start_x + w * 2;
		const auto end_y = this->m_Start_y + h * 2;

		if (selected)
		{
			Pawn::Promotions prom = Pawn::Promotions::Null;

			if (first_x <= hovering_x && hovering_x <= second_x) // First row
			{
				if (first_y <= hovering_y && hovering_y <= second_y) // First column -> Rook
					prom = Pawn::Promotions::Rook;
				else if (second_y <= hovering_y && hovering_y <= end_y) // Second column -> Knight
					prom = Pawn::Promotions::Knight;
			}
			else if (second_x <= hovering_x && hovering_x <= end_x) // Second row
			{
				if (first_y <= hovering_y && hovering_y <= second_y) // First column -> Bishop
					prom = Pawn::Promotions::Bishop;
				else if (second_y <= hovering_y && hovering_y <= end_y) // Second column -> Queen
					prom = Pawn::Promotions::Queen;
			}

			// TODO: Maybe reset selection and undo move if the promotion is invalid
			if (prom != Pawn::Promotions::Null)
			{
				// Fire the promotion event
				this->m_evtman.PostEvent(IEventPtr(new Event_PawnPromotion(this->m_PawnToPromote, prom))); 
			}
		}
		else
		{
			this->m_Hovering = INVALID;

			// Simply where highlight hovering
			if (first_x <= hovering_x && hovering_x <= second_x) // First row
			{
				if (first_y <= hovering_y && hovering_y <= second_y) // First column -> Rook
					this->m_Hovering = 1;
				else if (second_y <= hovering_y && hovering_y <= end_y) // Second column -> Knight
					this->m_Hovering = 2;
			}
			else if (second_x <= hovering_x && hovering_x <= end_x) // Second row
			{
				if (first_y <= hovering_y && hovering_y <= second_y) // First column -> Bishop
					this->m_Hovering = 3;
				else if (second_y <= hovering_y && hovering_y <= end_y) // Second column -> Queen
					this->m_Hovering = 4;
			}
		}
	}

} // OnInput


void PawnPromoter::OnRender()
{
	//	| R | K |
	//	| B | Q |

	if (this->m_Visible)
	{
		int w, h;
		g_TextureManager.GetTextureSize(w, h);

		Assert(this->m_PawnToPromote);
		this->m_Start_x = GetScreenWidth() / 2 - w;
		this->m_Start_y = GetScreenHeight() / 2 - h;

		Rectangle rect{ (float)this->m_Start_x, (float)this->m_Start_y, (float)(w * 2), (float)(h * 2) };
		DrawRectangleRounded(rect, .5, 100, SKYBLUE);

		float rad = std::min(w, h) * .5f;
		int x, y;
		switch (this->m_Hovering)
		{
		case 1: x = this->m_Start_x + w / 2;		y = this->m_Start_y + h / 2;		break;
		case 2:	x = this->m_Start_x + w / 2;		y = this->m_Start_y + 3 * h / 2;	break;
		case 3:	x = this->m_Start_x + 3 * w / 2;	y = this->m_Start_y + h / 2;		break;
		case 4:	x = this->m_Start_x + 3 * w / 2;	y = this->m_Start_y + 3 * h / 2;	break;
		default: goto Next;
		}
		DrawCircleLines(x, y, rad, GRAY);

Next:
		if (Pieces::White_Pawn_1 < this->m_PawnToPromote->GetPieceID())
		{
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::White_Rook],   this->m_Start_x, this->m_Start_y, WHITE);
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::White_Knight], this->m_Start_x, this->m_Start_y + w, WHITE);
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::White_Bishop], this->m_Start_x + w, this->m_Start_y, WHITE);
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::White_Queen],  this->m_Start_x + w, this->m_Start_y + w, WHITE);
		}
		else
		{
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::Black_Rook],   this->m_Start_x, this->m_Start_y, WHITE);
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::Black_Knight], this->m_Start_x, this->m_Start_y + w, WHITE);
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::Black_Bishop], this->m_Start_x + w, this->m_Start_y, WHITE);
			DrawTexture(g_TextureManager.GetTextures()[(size_t)PieceType::Black_Queen],  this->m_Start_x + w, this->m_Start_y + w, WHITE);
		}
	}
} // OnRender


void PawnPromoter::Reset() noexcept
{
	this->m_Visible = false;
	this->m_PawnToPromote = nullptr;
	this->m_Start_x = INVALID;
	this->m_Start_y = INVALID;
	this->m_Hovering = INVALID;
} // Reset



// ========================================================================================================================================
// Class HumanView Definition
// ========================================================================================================================================
#define PIECE_OFFSET 2

HumanView::HumanView(ChessGame& board)
	: IGameView(board), m_Targets(nullptr), m_textures{}, m_PawnPromoter(board.GetGameEventManager()),
	m_Selected_x(INVALID),
	m_Selected_y(INVALID),
	m_Hovering_x(INVALID),
	m_Hovering_y(INVALID),
	m_BackGround(DARKGREEN),
	m_FirstColor{ 177, 177, 177, 255 },
	m_SecondColor{ 0, 50, 255, 255 },
	m_Offset_Left(0),
	m_Offset_Right(0),
	m_Offset_Top(0),
	m_Offset_Bottom(0),
	m_Square_Side(0),
	m_Window_Width(GetScreenWidth()),
	m_Window_Height(GetScreenHeight())
{
	// Center board on window
	if (this->m_Window_Width >= this->m_Window_Height)
	{
		this->m_Offset_Left = this->m_Offset_Right = (this->m_Window_Width - this->m_Window_Height) / 2;

		// Board has the same height as window
		this->m_Offset_Top  = this->m_Offset_Bottom = 0;
		this->m_Square_Side = this->m_Window_Height / BOARD_SIDE;
	}
	else
	{
		this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Window_Height - this->m_Window_Width) / 2;

		// Board has the same width as window
		this->m_Offset_Left = this->m_Offset_Right = 0;
		this->m_Square_Side = this->m_Window_Width / BOARD_SIDE;
	}

	// Set initial texture size
	g_TextureManager.SetTextureSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Default Constructor


HumanView::HumanView(ChessGame& board, int left, int right, int top, int bottom)
	: IGameView(board), m_Targets(nullptr), m_textures{}, m_PawnPromoter(board.GetGameEventManager()),
	m_Selected_x(INVALID),
	m_Selected_y(INVALID),
	m_Hovering_x(INVALID),
	m_Hovering_y(INVALID),
	m_BackGround(PURPLE),
	m_FirstColor(YELLOW),
	m_SecondColor(GRAY),
	m_Offset_Left(left),
	m_Offset_Right(right),
	m_Offset_Top(top),
	m_Offset_Bottom(bottom),
	m_Square_Side(0),
	m_Window_Width(GetScreenWidth()),
	m_Window_Height(GetScreenHeight())
{
	if (this->m_Offset_Left		< 0)	this->m_Offset_Left		= 0;
	if (this->m_Offset_Right	< 0)	this->m_Offset_Right	= 0;
	if (this->m_Offset_Top		< 0)	this->m_Offset_Top		= 0;
	if (this->m_Offset_Bottom	< 0)	this->m_Offset_Bottom	= 0;

	// Horizontal offsets are too large
	if (this->m_Offset_Left + this->m_Offset_Right >= m_Window_Width - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Window_Width >= this->m_Window_Height)
			this->m_Offset_Left = this->m_Offset_Right = (this->m_Window_Width - this->m_Window_Height) / 2;
		else
			this->m_Offset_Left = this->m_Offset_Right = 0;  // No horizontal offset if width < height
	}

	// Vertical offsets are too large
	if (this->m_Offset_Bottom + this->m_Offset_Top >= m_Window_Height - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Window_Height >= this->m_Window_Width)
			this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Window_Height - this->m_Window_Width) / 2;
		else
			this->m_Offset_Top = this->m_Offset_Bottom = 0; // No vertical offset if height < width
	}

	int width = this->m_Window_Width - this->m_Offset_Left - this->m_Offset_Right;
	int height = this->m_Window_Height - this->m_Offset_Top - this->m_Offset_Bottom;
	if (height >= width)
		this->m_Square_Side = width / BOARD_SIDE;
	else
		this->m_Square_Side = height / BOARD_SIDE;

	// Set initial texture size
	g_TextureManager.SetTextureSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Constructor


HumanView::HumanView(ChessGame& board, Color background, Color first, Color second)
	: IGameView(board), m_Targets(nullptr), m_textures{}, m_PawnPromoter(board.GetGameEventManager()),
	m_Selected_x(INVALID),
	m_Selected_y(INVALID),
	m_Hovering_x(INVALID),
	m_Hovering_y(INVALID),
	m_BackGround(background),
	m_FirstColor(first),
	m_SecondColor(second),
	m_Offset_Left(0),
	m_Offset_Right(0),
	m_Offset_Top(0),
	m_Offset_Bottom(0),
	m_Square_Side(0),
	m_Window_Width(GetScreenWidth()),
	m_Window_Height(GetScreenHeight())
{
	// Center board on window
	if (this->m_Window_Width >= this->m_Window_Height)
	{
		this->m_Offset_Left = this->m_Offset_Right = (this->m_Window_Width - this->m_Window_Height) / 2;

		// Board has the same height as window
		this->m_Offset_Top = this->m_Offset_Bottom = 0;
		this->m_Square_Side = this->m_Window_Height / BOARD_SIDE;
	}
	else
	{
		this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Window_Height - this->m_Window_Width) / 2;

		// Board has the same width as window
		this->m_Offset_Left = this->m_Offset_Right = 0;
		this->m_Square_Side = this->m_Window_Width / BOARD_SIDE;
	}

	// Set initial texture size
	g_TextureManager.SetTextureSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Constructor


HumanView::HumanView(ChessGame& board, Color background, Color first, Color second, int left, int right, int top, int bottom)
	: IGameView(board), m_Targets(nullptr), m_textures{}, m_PawnPromoter(board.GetGameEventManager()),
	m_Selected_x(INVALID),
	m_Selected_y(INVALID),
	m_Hovering_x(INVALID),
	m_Hovering_y(INVALID),
	m_BackGround(background),
	m_FirstColor(first),
	m_SecondColor(second),
	m_Offset_Left(left),
	m_Offset_Right(right),
	m_Offset_Top(top),
	m_Offset_Bottom(bottom),
	m_Square_Side(0),
	m_Window_Width(GetScreenWidth()),
	m_Window_Height(GetScreenHeight())
{
	if (this->m_Offset_Left		< 0)	this->m_Offset_Left		= 0;
	if (this->m_Offset_Right	< 0)	this->m_Offset_Right	= 0;
	if (this->m_Offset_Top		< 0)	this->m_Offset_Top		= 0;
	if (this->m_Offset_Bottom	< 0)	this->m_Offset_Bottom	= 0;

	// Horizontal offsets are too large
	if (this->m_Offset_Left + this->m_Offset_Right >= m_Window_Width - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Window_Width >= this->m_Window_Height)
			this->m_Offset_Left = this->m_Offset_Right = (this->m_Window_Width - this->m_Window_Height) / 2;
		else
			this->m_Offset_Left = this->m_Offset_Right = 0;  // No horizontal offset if width < height
	}

	// Vertical offsets are too large
	if (this->m_Offset_Bottom + this->m_Offset_Top >= m_Window_Height - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Window_Height >= this->m_Window_Width)
			this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Window_Height - this->m_Window_Width) / 2;
		else
			this->m_Offset_Top = this->m_Offset_Bottom = 0; // No vertical offset if height < width
	}

	int width = this->m_Window_Width - this->m_Offset_Left - this->m_Offset_Right;
	int height = this->m_Window_Height - this->m_Offset_Top - this->m_Offset_Bottom;
	if (height >= width)
		this->m_Square_Side = width / BOARD_SIDE;
	else
		this->m_Square_Side = height / BOARD_SIDE;

	// Set initial texture size
	g_TextureManager.SetTextureSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Constructor

void HumanView::VOnInitialize(const std::string& xml_view_settings)
{
	auto& evtman = this->m_GameState.GetGameEventManager();
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnRightClick),			Event_RightButtonDown::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnLeftClick),			Event_LeftButtonDown::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnSelectSquare),		Event_SelectSquare::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnSelectionReset),		Event_SelectionReset::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnStartMovePiece),		Event_StartMovePiece::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnCastle),				Event_Castle::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnPromotePawn),			Event_PromotePawn::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnPawnPromotion),		Event_PawnPromotion::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnCheck),				Event_Check::GetEventType());
	evtman.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnEndMatch),			Event_EndMatch::GetEventType());

	// Load all piece textures. TODO: grab file names from xml_view_settings
	g_TextureManager.LoadPieceTextureFromFile(PieceType::White_Pawn,	"../../assets/white_pawn.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::White_Rook,	"../../assets/white_rook.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::White_Knight,	"../../assets/white_knight.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::White_Bishop,	"../../assets/white_bishop.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::White_Queen,	"../../assets/white_queen.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::White_King,	"../../assets/white_king.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::Black_Pawn,	"../../assets/black_pawn.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::Black_Rook,	"../../assets/black_rook.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::Black_Knight,	"../../assets/black_knight.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::Black_Bishop,	"../../assets/black_bishop.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::Black_Queen,	"../../assets/black_queen.png");
	g_TextureManager.LoadPieceTextureFromFile(PieceType::Black_King,	"../../assets/black_king.png");

	// Set up pieces components with the loaded textures
	for (size_t i = 0; i < this->m_textures.size(); ++i)
	{
		const auto& piece = this->m_GameState.GetPieces()[i];
		const auto piece_pos = piece->GetPiecePos();
		const int x = piece_pos % BOARD_SIDE;
		const int y = piece_pos / BOARD_SIDE;

		// Set starting position of the piece texture to the top-left corner of the square
		const int screen_x = x * this->m_Square_Side + this->m_Offset_Left + PIECE_OFFSET;
		const int screen_y = y * this->m_Square_Side + this->m_Offset_Top + PIECE_OFFSET;
		this->m_textures[i].screen_pos = Vector2{ (float)screen_x, (float)screen_y };

		// Find which texture to assign to the piece
		switch (piece->GetPieceID())
		{
			// White pieces
		case Pieces::White_Rook_King_Side:	case Pieces::White_Rook_Queen_Side: this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::White_Rook);   break;
		case Pieces::White_Knight_1:		case Pieces::White_Knight_2:		this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::White_Knight); break;
		case Pieces::White_Bishop_1:		case Pieces::White_Bishop_2:		this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::White_Bishop); break;
		case Pieces::White_Queen:												this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::White_Queen);  break;
		case Pieces::White_King:												this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::White_King);   break;
		case Pieces::White_Pawn_1: case Pieces::White_Pawn_2: case Pieces::White_Pawn_3: case Pieces::White_Pawn_4:
		case Pieces::White_Pawn_5: case Pieces::White_Pawn_6: case Pieces::White_Pawn_7: case Pieces::White_Pawn_8:
			this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::White_Pawn); break;
			
			// Black pieces
		case Pieces::Black_Rook_King_Side:	case Pieces::Black_Rook_Queen_Side: this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::Black_Rook);   break;
		case Pieces::Black_Knight_1:		case Pieces::Black_Knight_2:		this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::Black_Knight); break;
		case Pieces::Black_Bishop_1:		case Pieces::Black_Bishop_2:		this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::Black_Bishop); break;
		case Pieces::Black_Queen:												this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::Black_Queen);  break;
		case Pieces::Black_King:												this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::Black_King);   break;
		case Pieces::Black_Pawn_1: case Pieces::Black_Pawn_2: case Pieces::Black_Pawn_3: case Pieces::Black_Pawn_4:
		case Pieces::Black_Pawn_5: case Pieces::Black_Pawn_6: case Pieces::Black_Pawn_7: case Pieces::Black_Pawn_8:
			this->m_textures[i].tex = g_TextureManager.GetTexture(PieceType::Black_Pawn); break;

		default:
			THROW_CHESS_EXCEPTION(ErrorCode::InvalidID, "Invalid piece ID: %s", this->m_GameState.GetPieces()[i]->GetPieceID());
		}
	}
} // VOnInitialize

void HumanView::VOnInput()
{
	// Mouse
	const int x = GetMouseX();
	const int y = GetMouseY();

	// If console is showing, send input to its handler
	/*if (this->m_Console.IsActive())
	{
		this->m_Console.HandleKeyboardInput();
		return;
	}*/

	// If app is waiting for user to choose a promotion, send all input to the promoter
	if (this->m_PawnPromoter.m_Visible)
	{
		// Send all input
		this->m_PawnPromoter.OnInput(x, y, IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
		return;
	}

	// Process mouse input only if cursor is m_Hovering on the board
	if (this->m_Window_Width > 0 && this->m_Window_Height > 0 && this->m_Square_Side > 0 &&
		(this->m_Offset_Left < x && x < this->m_Window_Width - this->m_Offset_Right) &&
		(this->m_Offset_Top < y && y < this->m_Window_Height - this->m_Offset_Bottom))
	{
		// Find out which square is cursor hovering on
		this->m_Hovering_x = ((x - this->m_Offset_Left) / this->m_Square_Side);
		this->m_Hovering_y = ((y - this->m_Offset_Top) / this->m_Square_Side);
	}
	else
	{
		this->m_Hovering_x = INVALID;
		this->m_Hovering_y = INVALID;
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		this->m_GameState.GetGameEventManager().PostEvent(IEventPtr(new Event_LeftButtonDown(this->m_Hovering_x, this->m_Hovering_y)));

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		this->m_GameState.GetGameEventManager().PostEvent(IEventPtr(new Event_RightButtonDown(this->m_Hovering_x, this->m_Hovering_y)));

	// Keyboard
	//if (IsKeyPressed(KEY_DOWN))
	//{
	//	// Move hovering one down
	//	if (this->m_Hovering_y != INVALID)
	//		this->m_Hovering_y = (this->m_Hovering_y + 1) % BOARD_SIDE;
	//	else
	//		this->m_Hovering_y = 0;
	//}

	//if (IsKeyPressed(KEY_UP))
	//{
	//	// Move hovering one up
	//	if (this->m_Hovering_y != INVALID)
	//		this->m_Hovering_y = (this->m_Hovering_y - 1 + BOARD_SIDE) % BOARD_SIDE;
	//	else
	//		this->m_Hovering_y = BOARD_SIDE - 1;
	//}

	//if (IsKeyPressed(KEY_RIGHT))
	//{
	//	// Move hovering one right
	//	if (this->m_Hovering_x != INVALID)
	//		this->m_Hovering_x = (this->m_Hovering_x + 1) % BOARD_SIDE;
	//	else
	//		this->m_Hovering_x = 0;
	//}

	//if (IsKeyPressed(KEY_LEFT))
	//{
	//	// Move hovering one up
	//	if (this->m_Hovering_x != INVALID)
	//		this->m_Hovering_x = (this->m_Hovering_x - 1 + BOARD_SIDE) % BOARD_SIDE;
	//	else
	//		this->m_Hovering_x = BOARD_SIDE - 1;
	//}

	if (IsKeyPressed(KEY_ENTER))
	{
		if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))
		{
			ToggleFullscreen();
			this->VOnResize();
		}
		else if (this->m_Hovering_x != INVALID && this->m_Hovering_y != INVALID) // Select hovering
		{
			this->m_GameState.GetGameEventManager().PostEvent(IEventPtr(new Event_SelectSquare(this->m_Hovering_x, this->m_Hovering_y)));
		}
	}

	/*if (IsKeyPressed(KEY_KP_ENTER))
	{
		this->m_Console.SetActive(true);
	}*/
} // VOnInput

void HumanView::VOnResize()
{
	int width  = GetScreenWidth();
	int height = GetScreenHeight();

	// Resize only if needed
	if (width != this->m_Window_Width || height != this->m_Window_Height)
	{
		if (width <= 0 || height <= 0)
		{
			this->m_Window_Width = 0;
			this->m_Window_Height = 0;
			this->m_Square_Side = 0;
		}
		else
		{
			// old_offset : old_size = new_offset : new_size => new_offset = old_offset * new_size / old_size
			if (this->m_Window_Width > 0)
			{
				this->m_Offset_Left = this->m_Offset_Left * width / this->m_Window_Width;
				this->m_Offset_Right = this->m_Offset_Right * width / this->m_Window_Width;
			}

			if (this->m_Window_Height > 0)
			{
				this->m_Offset_Top = this->m_Offset_Top * height / this->m_Window_Height;
				this->m_Offset_Bottom = this->m_Offset_Bottom * height / this->m_Window_Height;
			}

			// Recalculate square size based on the current window's size
			if (width >= height)
				this->m_Square_Side = (height - this->m_Offset_Top - this->m_Offset_Bottom) / BOARD_SIDE;
			else
				this->m_Square_Side = (width - this->m_Offset_Left - this->m_Offset_Right) / BOARD_SIDE;

			this->m_Window_Width = width;
			this->m_Window_Height = height;
		}
	}
} // On Resize


void HumanView::VOnRender()
{
	BeginDrawing();
	ClearBackground(this->m_BackGround);

	// Draw the board
	this->DrawBoard();

	// Draw the piece textures
	// Set up pieces components with the loaded textures
	for (size_t i = 0; i < this->m_textures.size(); ++i)
	{
		const auto& piece = this->m_GameState.GetPieces()[i];
		if (piece->IsAlive())
		{
			DrawTextureEx(*this->m_textures[i].tex, this->m_textures[i].screen_pos, 0.f, 1.f, WHITE);
		}
	}

	// Draw the pawn promoter interface
	this->m_PawnPromoter.OnRender();

	if (this->m_MatchOver != Event_EndMatch::END_STATUS::Playing)
	{
		if (this->m_MatchOver == Event_EndMatch::END_STATUS::White)
		{
			Vector2 vec = MeasureTextEx(GetFontDefault(), "WHITE WON!", 50.f, 1.f);
			DrawText("WHITE WON!", (this->m_Window_Width - this->m_Offset_Left - (int)vec.x) / 2,
				(this->m_Window_Height - this->m_Offset_Top - (int)vec.y) / 2 - this->m_Offset_Top, 50, Color{ 255, 0, 0, 255 });
		}
		else if (this->m_MatchOver == Event_EndMatch::END_STATUS::Black)
		{
			Vector2 vec = MeasureTextEx(GetFontDefault(), "BLACK WON!", 50.f, 1.f);
			DrawText("BLACK WON!", (this->m_Window_Width - this->m_Offset_Left - (int)vec.x) / 2,
				(this->m_Window_Height - this->m_Offset_Top - (int)vec.y) / 2 - this->m_Offset_Top, 50, Color{ 255, 0, 0, 255 });
		}
		else // Stalemate
		{
			Vector2 vec = MeasureTextEx(GetFontDefault(), "STALEMATE!", 50.f, 1.f);
			DrawText("STALEMATE!", (this->m_Window_Width - this->m_Offset_Left - (int)vec.x) / 2,
				(this->m_Window_Height - this->m_Offset_Top - (int)vec.y) / 2, 50, Color{ 255, 0, 0, 255 });
		}
	}

	EndDrawing();
} // VOnRender


void HumanView::VOnUpdate(std::chrono::nanoseconds delta)
{
	this->m_ProcessManager.UpdateProcesses(delta.count());
} // VOnUpdate


void HumanView::DrawBoard() const
{
	// The loops do not execute if m_Square_Side is 0
	const int max = BOARD_SIDE * this->m_Square_Side;

	// Draw board 
	for (int row = 0; row < max; row += this->m_Square_Side)
	{
		for (int col = 0; col < max; col += this->m_Square_Side)
		{
			// We alternate colors just like in a normal chess board
			DrawRectangle(row + this->m_Offset_Left, col + this->m_Offset_Top, this->m_Square_Side, this->m_Square_Side,
				((row + col) / this->m_Square_Side) % 2 == 0 ? this->m_FirstColor : this->m_SecondColor);
		}
	}

	// Highlight the square that contains the cursor
	if (this->m_HighLight_Hovering && this->m_Hovering_x != INVALID && this->m_Hovering_y != INVALID)
	{
		Rectangle rect = {
			float(this->m_Offset_Left + this->m_Hovering_x * this->m_Square_Side),
			float(this->m_Offset_Top + this->m_Hovering_y * this->m_Square_Side),
			float(this->m_Square_Side),
			float(this->m_Square_Side) };

		DrawRectangleLinesEx(rect, 2.5f, BLUE);
	}

	// Highlight the selected piece
	if (this->m_HighLight_SelectedPiece && this->m_Selected_x != INVALID && this->m_Selected_y != INVALID)
	{
		Rectangle rect = {
			float(this->m_Offset_Left + this->m_Selected_x * this->m_Square_Side),
			float(this->m_Offset_Top + this->m_Selected_y * this->m_Square_Side),
			float(this->m_Square_Side),
			float(this->m_Square_Side) };

		DrawRectangleLinesEx(rect, 5.0f, RED);
	}

	// Draw possible targets
	if (this->m_Targets)
	{
		size_t moves = this->m_Targets->size();
		for (size_t i = 0; i < moves; ++i)
		{
			int xCoord = (*this->m_Targets)[i] % BOARD_SIDE;
			int yCoord = (*this->m_Targets)[i] / BOARD_SIDE;
			Rectangle rect = {
				float(this->m_Offset_Left + xCoord * this->m_Square_Side),
				float(this->m_Offset_Top + yCoord * this->m_Square_Side),
				float(this->m_Square_Side),
				float(this->m_Square_Side) };

			DrawRectangleLinesEx(rect, 7.5f, GREEN);
		}
	}
} // DrawBoard


void HumanView::OnLeftClick(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_LeftButtonDown& event = *static_cast<Event_LeftButtonDown*>(pEvent.get());

	// Reset selection only if left-clicking on the same square
	if (event.m_x == INVALID || event.m_y == INVALID || (this->m_Selected_x == event.m_x && this->m_Selected_y == event.m_y))
	{
		this->m_GameState.GetGameEventManager().PostEvent(IEventPtr(new Event_SelectionReset));
	}
	else
	{
		// Select new square
		this->m_GameState.GetGameEventManager().PostEvent(IEventPtr(new Event_SelectSquare(event.m_x, event.m_y)));
	}
} // OnLeftClick Listener


void HumanView::OnRightClick(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Reset selection no matter where cursor is
	this->m_GameState.GetGameEventManager().PostEvent(IEventPtr(new Event_SelectionReset));
} // OnRightClick Listener


void HumanView::OnSelectSquare(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_SelectSquare& event = *static_cast<Event_SelectSquare*>(pEvent.get());
	const auto& game_state = this->m_GameState.GetGameState();
	const int index = event.m_y * BOARD_SIDE + event.m_x; // Coordinates and indeces are kinda reversed
	auto& evtman = this->m_GameState.GetGameEventManager();

	// If a piece was already selected
	if (this->m_Targets)
	{
		// If new selection corresponds to a possible move
		const auto size = this->m_Targets->size();
		for (int i = 0; i < size; ++i)
		{
			if ((*this->m_Targets)[i] == index)
			{
				// Fire an event to move the piece that was selected
				int old_sel_index = this->m_Selected_y * BOARD_SIDE + this->m_Selected_x;
				Piece* selected = game_state.m_Board[old_sel_index];
				evtman.PostEvent(IEventPtr(new Event_StartMovePiece(selected, old_sel_index, index)));
				return;
			}
		}

		// If execution gets here then the selection wasn't a valid move
		if (game_state.m_Board[index] != nullptr) // User selected another piece
		{
			// Fire an event to select new piece
			this->m_Selected_x = event.m_x;
			this->m_Selected_y = event.m_y;
			evtman.PostEvent(IEventPtr(new Event_PieceSelected(index)));

			// Update possible targets for the new selection
			this->m_Targets = game_state.m_Board[index]->GetCachedMoves();
		}
		else // Illegal move
			evtman.PostEvent(IEventPtr(new Event_IllegalMove(index)));
	}
	else if (game_state.m_Board[index] != nullptr) // User selected another piece
	{
		// Fire an event to select new piece
		this->m_Selected_x = event.m_x;
		this->m_Selected_y = event.m_y;
		evtman.PostEvent(IEventPtr(new Event_PieceSelected(index)));

		// Update possible targets for the new selection
		this->m_Targets = game_state.m_Board[index]->GetCachedMoves();
	}
	else
	{
		// User selected a white space - reset selection (shouldn't be necessary)
		evtman.PostEvent(IEventPtr(new Event_SelectionReset));
	}
} // OnSelectSquare Listener


void HumanView::OnSelectionReset(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	this->m_Selected_x = INVALID;
	this->m_Selected_y = INVALID;

	// Reset possible targets
	this->m_Targets = nullptr;
} // OnSelectionReset Listener


void HumanView::OnStartMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_StartMovePiece& event = *static_cast<Event_StartMovePiece*>(pEvent.get());
	const Pieces piece_id = event.m_Piece->GetPieceID();

	int stop_row = this->m_Offset_Top + (event.m_Dest / BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;
	int stop_col = this->m_Offset_Left + (event.m_Dest % BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;

	// Move the image
	auto& tex = this->m_textures[static_cast<int>(piece_id)];
	this->m_ProcessManager.AddProcess(ProcessPtr(new MovePieceProcess(tex, MilliToNano(250), stop_row, stop_col)));
} // OnStartMovePiece Listener


void HumanView::OnCastle(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_Castle& event = *static_cast<Event_Castle*>(pEvent.get());
	const Pieces piece_id = event.m_Piece->GetPieceID();

	int stop_row = this->m_Offset_Top + (event.m_Dest / BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;
	int stop_col = this->m_Offset_Left + (event.m_Dest % BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;

	// Move the image
	auto& tex = this->m_textures[static_cast<int>(piece_id)];

	// Add a delay and then move the image
	auto delay = new DelayProcess(MilliToNano(100));
	delay->AttachChildProcess(ProcessPtr(new MovePieceProcess(tex, MilliToNano(250), stop_row, stop_col)));
	this->m_ProcessManager.AddProcess(ProcessPtr(delay));
} // OnCastle Listener


void HumanView::OnPromotePawn(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_PromotePawn& event = *static_cast<Event_PromotePawn*>(pEvent.get());
	this->m_PawnPromoter.PromotePawn(event.m_Pawn);
} // OnPromotePawn Listener


void HumanView::OnPawnPromotion(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	const Event_PawnPromotion& event = *static_cast<Event_PawnPromotion*>(pEvent.get());
	const auto id = event.m_Pawn->GetPieceID();

	// Reset pawn promotion
	this->m_PawnPromoter.Reset();
	
	// Find the new image
	const Texture2D* new_tex = nullptr;
	switch (event.m_promotion)
	{
	case Pawn::Promotions::Rook:	if (isWhite(id)) new_tex = g_TextureManager.GetTexture(PieceType::White_Rook);	 else new_tex = g_TextureManager.GetTexture(PieceType::Black_Rook);   break;
	case Pawn::Promotions::Knight:	if (isWhite(id)) new_tex = g_TextureManager.GetTexture(PieceType::White_Knight); else new_tex = g_TextureManager.GetTexture(PieceType::Black_Knight); break;
	case Pawn::Promotions::Bishop:	if (isWhite(id)) new_tex = g_TextureManager.GetTexture(PieceType::White_Bishop); else new_tex = g_TextureManager.GetTexture(PieceType::Black_Bishop); break;
	case Pawn::Promotions::Queen:	if (isWhite(id)) new_tex = g_TextureManager.GetTexture(PieceType::White_Queen);  else new_tex = g_TextureManager.GetTexture(PieceType::Black_Queen);  break;

	case Pawn::Promotions::Null:
		THROW_CHESS_EXCEPTION(ErrorCode::InvalidPromotion, "Cannot promote a pawn back to a pawn");
	}

	// Change the pawn image
	this->m_textures[static_cast<int>(id)].tex = new_tex;
} // OnPawnPromotion


void HumanView::OnCheck(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_Check& event = *static_cast<Event_Check*>(pEvent.get());
	this->m_CheckKingIndex = event.m_KingIndex;
} // OnCheck 


void HumanView::OnEndMatch(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_EndMatch& event = *static_cast<Event_EndMatch*>(pEvent.get());
	this->m_MatchOver = event.m_Status;
} // OnEndMatch




// ========================================================================================================================================
// ========================================================================================================================================
// Processes Definitions
// ========================================================================================================================================
// ========================================================================================================================================


// ========================================================================================================================================
// Animate pieces movements Process
// ========================================================================================================================================
MovePieceProcess::MovePieceProcess(TexData& image, nanoseconds executionTime, int stop_row_coord, int stop_col_coord)
	: m_TotalTimeAvailable(executionTime), m_Stop_Row(stop_row_coord), m_Stop_Col(stop_col_coord), m_Image(image)
{
	this->m_TimePassedSoFar = 0;
} // Constructor


void MovePieceProcess::VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta)
{
	int x = static_cast<int>(this->m_Image.screen_pos.x);
	int y = static_cast<int>(this->m_Image.screen_pos.y);

	if (y == this->m_Stop_Row && x == this->m_Stop_Col)
		this->Succeed();
	else
	{
		this->m_TimePassedSoFar += delta;

		// Since nanosec resolution is way to precise for this task, just consider hundredths of seconds
		const int availTime = (int)(NanoToMilli(this->m_TotalTimeAvailable - this->m_TimePassedSoFar) / 10);

		if (availTime <= 0) // Time expired
		{
			this->m_Image.screen_pos.x = static_cast<float>(this->m_Stop_Col);
			this->m_Image.screen_pos.y = static_cast<float>(this->m_Stop_Row);
		}
		else // Still got some time
		{
			
			int next_r_step = 0, next_c_step = 0;
			if (this->m_Stop_Row > y)
			{
				next_r_step = (this->m_Stop_Row - y) / availTime;
				y += next_r_step;
			}
			else
			{
				next_r_step = (y - this->m_Stop_Row) / availTime;
				y -= next_r_step;
			}

			if (this->m_Stop_Col > x)
			{
				next_c_step = (this->m_Stop_Col - x) / availTime;
				x += next_c_step;
			}
			else
			{
				next_c_step = (x - this->m_Stop_Col) / availTime;
				x -= next_c_step;
			}
			
			// Update image
			this->m_Image.screen_pos.x = static_cast<float>(x);
			this->m_Image.screen_pos.y = static_cast<float>(y);
		}
	}
} // VOnUpdate



// ========================================================================================================================================
// Delay Process
// ========================================================================================================================================
DelayProcess::DelayProcess(nanoseconds delayTime)
	: m_Delay(delayTime), m_TimePassedSoFar(0)
{
} // Constructor


void DelayProcess::VOnUpdate(nanoseconds delta)
{
	this->m_TimePassedSoFar += delta;
	if (this->m_TimePassedSoFar >= this->m_Delay)
		this->Succeed();
} // VOnUpdate


#if 0
// ========================================================================================================================================
// ========================================================================================================================================
// Class Console Definition
// ========================================================================================================================================
// ========================================================================================================================================

HumanView::Console::Console()
	: m_bActive(false), m_bExecuteStringOnUpdate(false), m_bCursorOn(true), m_CurrentInputString(""), m_ConsoleInputSize(48),
	m_InputColor{ 255, 255, 255, 255 }, m_OutputColor{ 255, 255, 255, 255 }
{

} // Constructor

void HumanView::Console::AddDisplayText(const std::string& newText)
{
	this->m_CurrentOutputString += newText;
	this->m_CurrentOutputString += '\n';
} // AddDisplayText

void HumanView::Console::SetDisplayText(const std::string& newText)
{
	this->m_CurrentOutputString = newText;
} // SetDisplayText

void HumanView::Console::OnUpdate(milliseconds delta)
{
	// Don't do anything if not active.
	if (this->m_bActive)
	{
		// Do we have a string to execute?
		if (this->m_bExecuteStringOnUpdate)
		{
			const std::string renderedInputString = ">" + this->m_CurrentInputString;
			if (this->m_CurrentInputString.compare(ExitString) == 0)
			{
				this->SetActive(false);
				this->m_CurrentInputString.clear();
			}
			else if (this->m_CurrentInputString.compare(ClearString) == 0)
			{
				this->m_CurrentOutputString = renderedInputString;
				this->m_CurrentInputString.clear();
			}
			else
			{
				// Put the input string into the output window.
				this->AddDisplayText(renderedInputString);

				// Attempt to execute the current input string...
				if (!this->m_CurrentInputString.empty())
					LuaStateManager::Get()->VExecuteString(this->m_CurrentInputString.c_str());

				// Clear the input string
				this->m_CurrentInputString.clear();
			}

			// We're accepting input again.
			this->m_bExecuteStringOnUpdate = false;
			this->SetActive(false);
		}

		// Update the cursor blink timer...
		this->m_CursorBlinkTimer -= delta;

		if (this->m_CursorBlinkTimer < 0)
		{
			this->m_CursorBlinkTimer = 0;

			this->m_bCursorOn = !this->m_bCursorOn;
			this->m_CursorBlinkTimer = CursorBlinkTimeMS;
		}
	} // Active
} // Update


void HumanView::Console::OnRender()
{
	// Don't do anything if not active.
	if (this->m_bActive)
	{		
		// Display the console text at screen top, below the other text displayed.
		const std::string finalInputString = ">" + this->m_CurrentInputString + (this->m_bCursorOn ? '\xa0' : '_');
	
		// Draw black console
		DrawRectangle(this->m_Console.x, this->m_Console.y, this->m_Console.width, this->m_Console.height, BLACK);

		// Print input text
		DrawText(finalInputString.c_str(), this->m_Console.x + 10, this->m_Console.y + 10, 10, RED);

		// Print output text
		DrawText(this->m_CurrentOutputString.c_str(), this->m_Console.x + 20, this->m_Console.y + 20, 10, YELLOW);
	} // Active
} // Render


void HumanView::Console::HandleKeyboardInput()
{
	if (this->m_bExecuteStringOnUpdate)
	{
		// We've already got a string to execute; ignore.
		return;
	}

	// See if it's a valid key press that we care about.
	switch (oemKeyVal)
	{
	case VK_BACK:
	{
		const size_t strSize = m_CurrentInputString.size();
		if (strSize > 0)
		{
			m_CurrentInputString.erase((strSize - 1), 1);
		}
		break;
	}

	case VK_RETURN:
	{
		m_bExecuteStringOnUpdate = true;	//Execute this string.
		break;
	}

	default:
	{
		m_CurrentInputString += (char)keyVal;
		break;
	}
	}
} // HandleKeyboardInput
#endif