// Milan, 10th December 2021
// 

#include "../Libraries/Logger.h"
#include "Chess.h"
#include "ChessEvents.h"
#include "GameViews.h"
using namespace ENGINE_NAMESPACE;

#define PIECE_OFFSET 0
#define RENDER_COMPONENT_NAME "Render_Component"

int TextureLoader::tex_width = 10;
int TextureLoader::tex_height = 10;

std::array<std::pair<std::string, Texture2D>, (size_t)PieceType::Num_Types> TextureLoader::textures;

ENGINE_NAMESPACE::EventManager& IGameView::s_GlobalEventManager = GetEventManager();

void TextureLoader::SetTextureStartingSize(int new_width, int new_height)
{
	Assert(new_width >= 0 && new_height >= 0 && "Invalid texture size");

	TextureLoader::tex_width = new_width;
	TextureLoader::tex_height = new_height;
} // SetTextureStartingSize

const Texture2D* TextureLoader::GetPieceTextureAndName(PieceType typeID, std::string& outFileName)
{
	const auto& ret = TextureLoader::textures[(size_t)typeID];
	outFileName = ret.first;
	return &ret.second; 
} // GetPieceTextureAndName

const Texture2D* TextureLoader::LoadPieceTextureFromFile(PieceType typeID, const std::string& fileName)
{
	// Create an image from a file in memory
	Image image = LoadImage(fileName.c_str());

	// Resize image to the desired width and height
	ImageResize(&image, TextureLoader::tex_width, TextureLoader::tex_height);

	// then create a texture in vram from said image and sore it in the texture array
	Texture2D texture = LoadTextureFromImage(image);

	// Dealocate the image
	UnloadImage(image);

	TextureLoader::textures[(size_t)typeID] = std::make_pair(fileName, texture);

	return &TextureLoader::textures[(size_t)typeID].second;
} // LoadPieceTextureFromFile


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
		const auto first_x = this->m_Start_x;
		const auto first_y = this->m_Start_y;
		const auto second_x = this->m_Start_x + TextureLoader::tex_width;
		const auto second_y = this->m_Start_y + TextureLoader::tex_height;
		const auto end_x = this->m_Start_x + TextureLoader::tex_width * 2;
		const auto end_y = this->m_Start_y + TextureLoader::tex_height * 2;

		if (selected)
		{
			if (first_x <= hovering_x && hovering_x <= second_x) // First row
			{
				if (first_y <= hovering_y && hovering_y <= second_y) // First column -> Rook
					this->m_PawnToPromote->Promote(Pawn::Promotions::Rook);
				else if (second_y <= hovering_y && hovering_y <= end_y) // Second column -> Knight
					this->m_PawnToPromote->Promote(Pawn::Promotions::Knight);

				else; // TODO: Reset selection and undo move
			}
			else if (second_x <= hovering_x && hovering_x <= end_x) // Second row
			{
				if (first_y <= hovering_y && hovering_y <= second_y) // First column -> Bishop
					this->m_PawnToPromote->Promote(Pawn::Promotions::Bishop);
				else if (second_y <= hovering_y && hovering_y <= end_y) // Second column -> Queen
					this->m_PawnToPromote->Promote(Pawn::Promotions::Queen);

				else; // TODO: Reset selection and undo move
			}

			else; // TODO: Reset selection and undo move

			// We're no more useful
			this->m_PawnToPromote = nullptr;
			this->m_Visible = false;
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
		Assert(this->m_PawnToPromote);
		this->m_Start_x = GetScreenWidth() / 2 - TextureLoader::tex_width;
		this->m_Start_y = GetScreenHeight() / 2 - TextureLoader::tex_height;

		Rectangle rect{ (float)this->m_Start_x, (float)this->m_Start_y, TextureLoader::tex_width * 2.f, TextureLoader::tex_height * 2.f };
		DrawRectangleRounded(rect, .5, 100, SKYBLUE);

		float rad = std::min(TextureLoader::tex_width, TextureLoader::tex_height) * .5f;
		int x, y;
		switch (this->m_Hovering)
		{
		case 1: x = this->m_Start_x + TextureLoader::tex_width / 2;		y = this->m_Start_y + TextureLoader::tex_height / 2;		break;
		case 2:	x = this->m_Start_x + TextureLoader::tex_width / 2;		y = this->m_Start_y + 3 * TextureLoader::tex_height / 2;	break;
		case 3:	x = this->m_Start_x + 3 * TextureLoader::tex_width / 2;	y = this->m_Start_y + TextureLoader::tex_height / 2;		break;
		case 4:	x = this->m_Start_x + 3 * TextureLoader::tex_width / 2;	y = this->m_Start_y + 3 * TextureLoader::tex_height / 2;	break;
		default: goto Next;
		}
		DrawCircleLines(x, y, rad, GRAY);

Next:
		if (Pieces::White_Pawn_1 < this->m_PawnToPromote->GetPieceID())
		{
			DrawTexture(TextureLoader::textures[(size_t)PieceType::White_Rook].second, this->m_Start_x, this->m_Start_y, WHITE);
			DrawTexture(TextureLoader::textures[(size_t)PieceType::White_Knight].second, this->m_Start_x, this->m_Start_y + TextureLoader::tex_width, WHITE);
			DrawTexture(TextureLoader::textures[(size_t)PieceType::White_Bishop].second, this->m_Start_x + TextureLoader::tex_width, this->m_Start_y, WHITE);
			DrawTexture(TextureLoader::textures[(size_t)PieceType::White_Queen].second, this->m_Start_x + TextureLoader::tex_width, this->m_Start_y + TextureLoader::tex_width, WHITE);
		}
		else
		{
			DrawTexture(TextureLoader::textures[(size_t)PieceType::Black_Rook].second, this->m_Start_x, this->m_Start_y, WHITE);
			DrawTexture(TextureLoader::textures[(size_t)PieceType::Black_Knight].second, this->m_Start_x, this->m_Start_y + TextureLoader::tex_width, WHITE);
			DrawTexture(TextureLoader::textures[(size_t)PieceType::Black_Bishop].second, this->m_Start_x + TextureLoader::tex_width, this->m_Start_y, WHITE);
			DrawTexture(TextureLoader::textures[(size_t)PieceType::Black_Queen].second, this->m_Start_x + TextureLoader::tex_width, this->m_Start_y + TextureLoader::tex_width, WHITE);
		}
	}
} // OnRender

// ========================================================================================================================================
// Class HumanView Definition
// ========================================================================================================================================

HumanView::HumanView(const Board& board, const PossibleMovesIndeces& targets)
	: IGameView(board), m_Targets(targets),
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
	m_Screen_Width(GetScreenWidth()),
	m_Screen_Height(GetScreenHeight())
{
	// Center board on window
	if (this->m_Screen_Width >= this->m_Screen_Height)
	{
		this->m_Offset_Left = this->m_Offset_Right = (this->m_Screen_Width - this->m_Screen_Height) / 2;

		// Board has the same height as window
		this->m_Offset_Top = this->m_Offset_Bottom = 0;
		this->m_Square_Side = this->m_Screen_Height / BOARD_SIDE;
	}
	else
	{
		this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Screen_Height - this->m_Screen_Width) / 2;

		// Board has the same width as window
		this->m_Offset_Left = this->m_Offset_Right = 0;
		this->m_Square_Side = this->m_Screen_Width / BOARD_SIDE;
	}

	// Set initial texture size
	TextureLoader::SetTextureStartingSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Default Constructor

HumanView::HumanView(const Board& board, const PossibleMovesIndeces& targets, int left, int right, int top, int bottom)
	: IGameView(board), m_Targets(targets), 
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
	m_Screen_Width(GetScreenWidth()),
	m_Screen_Height(GetScreenHeight())
{
	if (this->m_Offset_Left		< 0)	this->m_Offset_Left		= 0;
	if (this->m_Offset_Right	< 0)	this->m_Offset_Right	= 0;
	if (this->m_Offset_Top		< 0)	this->m_Offset_Top		= 0;
	if (this->m_Offset_Bottom	< 0)	this->m_Offset_Bottom	= 0;

	// Horizontal offsets are too large
	if (this->m_Offset_Left + this->m_Offset_Right >= m_Screen_Width - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Screen_Width >= this->m_Screen_Height)
			this->m_Offset_Left = this->m_Offset_Right = (this->m_Screen_Width - this->m_Screen_Height) / 2;
		else
			this->m_Offset_Left = this->m_Offset_Right = 0;  // No horizontal offset if width < height
	}

	// Vertical offsets are too large
	if (this->m_Offset_Bottom + this->m_Offset_Top >= m_Screen_Height - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Screen_Height >= this->m_Screen_Width)
			this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Screen_Height - this->m_Screen_Width) / 2;
		else
			this->m_Offset_Top = this->m_Offset_Bottom = 0; // No vertical offset if height < width
	}

	int width = this->m_Screen_Width - this->m_Offset_Left - this->m_Offset_Right;
	int height = this->m_Screen_Height - this->m_Offset_Top - this->m_Offset_Bottom;
	if (height >= width)
		this->m_Square_Side = width / BOARD_SIDE;
	else
		this->m_Square_Side = height / BOARD_SIDE;

	// Set initial texture size
	TextureLoader::SetTextureStartingSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Constructor

HumanView::HumanView(const Board& board, const PossibleMovesIndeces& targets, Color background, Color first, Color second)
	: IGameView(board), m_Targets(targets), 
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
	m_Screen_Width(GetScreenWidth()),
	m_Screen_Height(GetScreenHeight())
{
	// Center board on window
	if (this->m_Screen_Width >= this->m_Screen_Height)
	{
		this->m_Offset_Left = this->m_Offset_Right = (this->m_Screen_Width - this->m_Screen_Height) / 2;

		// Board has the same height as window
		this->m_Offset_Top = this->m_Offset_Bottom = 0;
		this->m_Square_Side = this->m_Screen_Height / BOARD_SIDE;
	}
	else
	{
		this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Screen_Height - this->m_Screen_Width) / 2;

		// Board has the same width as window
		this->m_Offset_Left = this->m_Offset_Right = 0;
		this->m_Square_Side = this->m_Screen_Width / BOARD_SIDE;
	}

	// Set initial texture size
	TextureLoader::SetTextureStartingSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Constructor

HumanView::HumanView(const Board& board, const PossibleMovesIndeces& targets, Color background, Color first, Color second, int left, int right, int top, int bottom)
	: IGameView(board), m_Targets(targets), 
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
	m_Screen_Width(GetScreenWidth()),
	m_Screen_Height(GetScreenHeight())
{
	if (this->m_Offset_Left		< 0)	this->m_Offset_Left		= 0;
	if (this->m_Offset_Right	< 0)	this->m_Offset_Right	= 0;
	if (this->m_Offset_Top		< 0)	this->m_Offset_Top		= 0;
	if (this->m_Offset_Bottom	< 0)	this->m_Offset_Bottom	= 0;

	// Horizontal offsets are too large
	if (this->m_Offset_Left + this->m_Offset_Right >= m_Screen_Width - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Screen_Width >= this->m_Screen_Height)
			this->m_Offset_Left = this->m_Offset_Right = (this->m_Screen_Width - this->m_Screen_Height) / 2;
		else
			this->m_Offset_Left = this->m_Offset_Right = 0;  // No horizontal offset if width < height
	}

	// Vertical offsets are too large
	if (this->m_Offset_Bottom + this->m_Offset_Top >= m_Screen_Height - 4 * BOARD_SIDE)
	{
		// Center board on window
		if (this->m_Screen_Height >= this->m_Screen_Width)
			this->m_Offset_Top = this->m_Offset_Bottom = (this->m_Screen_Height - this->m_Screen_Width) / 2;
		else
			this->m_Offset_Top = this->m_Offset_Bottom = 0; // No vertical offset if height < width
	}

	int width = this->m_Screen_Width - this->m_Offset_Left - this->m_Offset_Right;
	int height = this->m_Screen_Height - this->m_Offset_Top - this->m_Offset_Bottom;
	if (height >= width)
		this->m_Square_Side = width / BOARD_SIDE;
	else
		this->m_Square_Side = height / BOARD_SIDE;

	// Set initial texture size
	TextureLoader::SetTextureStartingSize(this->m_Square_Side - 2 * PIECE_OFFSET, this->m_Square_Side - 2 * PIECE_OFFSET);
} // Constructor

void HumanView::VOnInitialize()
{
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnRightClick),		Event_RightButtonDown::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnLeftClick),			Event_LeftButtonDown::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnSelectSquare),		Event_SelectSquare::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnSelectionReset),	Event_SelectionReset::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnStartMovePiece),	Event_StartMovePiece::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnCastle),			Event_Castle::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnPawnPromotion),		Event_PawnPromotion::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnCheck),				Event_Check::GetEventType());
	s_GlobalEventManager.AddListener(fastdelegate::MakeDelegate(this, &HumanView::OnEndMatch),			Event_EndMatch::GetEventType());


	// All pieces' images must be set on the correct starting position based on where they are on the board
	int col_coord = this->m_Offset_Left;
	int row_coord = this->m_Offset_Top;
	Piece* curr = nullptr;
	for (size_t i = 0; i < BOARD_SIDE; ++i) // Find all pieces
	{
		for (size_t j = 0; j < BOARD_SIDE; ++j) // Find all pieces
		{
			curr = this->m_Board.m_Board[i * BOARD_SIDE + j];

			// Set coordinates
			if (curr)
			{
				auto comp = dynamic_cast<Piece2DRenderComponent*>(curr->GetComponent(RENDER_COMPONENT_NAME));
				Assert(comp);

				// Coordinates and row/col indeces are kinda reversed
				comp->SetCoordinates(col_coord + PIECE_OFFSET, row_coord + PIECE_OFFSET - 3);
			}

			// Advance col coordinates
			col_coord += this->m_Square_Side;
		}

		// Advance row coordinates
		row_coord += this->m_Square_Side;
		col_coord = this->m_Offset_Left;
	} // outer for
} // VOnInitialize

void HumanView::VOnInput()
{
	this->TranslateAndDispachInput();
} // VOnInput

void HumanView::VOnResize()
{
	int width = GetScreenWidth();
	int height = GetScreenHeight();

	// Resize only if needed
	if (width != this->m_Screen_Width || height != this->m_Screen_Height)
	{
		if (width <= 0 || height <= 0)
		{
			this->m_Screen_Width = 0;
			this->m_Screen_Height = 0;
			this->m_Square_Side = 0;
		}
		else
		{
			// old_offset : old_size = new_offset : new_size => new_offset = old_offset * new_size / old_size
			if (this->m_Screen_Width > 0)
			{
				this->m_Offset_Left = this->m_Offset_Left * width / this->m_Screen_Width;
				this->m_Offset_Right = this->m_Offset_Right * width / this->m_Screen_Width;
			}

			if (this->m_Screen_Height > 0)
			{
				this->m_Offset_Top = this->m_Offset_Top * height / this->m_Screen_Height;
				this->m_Offset_Bottom = this->m_Offset_Bottom * height / this->m_Screen_Height;
			}

			// Recalculate square size based on the current window's size
			if (width >= height)
				this->m_Square_Side = (height - this->m_Offset_Top - this->m_Offset_Bottom) / BOARD_SIDE;
			else
				this->m_Square_Side = (width - this->m_Offset_Left - this->m_Offset_Right) / BOARD_SIDE;

			this->m_Screen_Width = width;
			this->m_Screen_Height = height;
		}
	}
} // On Resize


void HumanView::VOnRender()
{
	BeginDrawing();
	ClearBackground(this->m_BackGround);

	this->DrawBoard();
	this->m_PawnPromoter.OnRender();

	if (this->m_MatchOver != Event_EndMatch::END_STATUS::Playing)
	{
		if (this->m_MatchOver == Event_EndMatch::END_STATUS::White)
		{
			Vector2 vec = MeasureTextEx(GetFontDefault(), "WHITE WON!", 50.f, 1.f);
			DrawText("WHITE WON!", (this->m_Screen_Width - this->m_Offset_Left - (int)vec.x) / 2,
				(this->m_Screen_Height - this->m_Offset_Top - (int)vec.y) / 2 - this->m_Offset_Top, 50, Color{ 255, 0, 0, 255 });
		}
		else if (this->m_MatchOver == Event_EndMatch::END_STATUS::Black)
		{
			Vector2 vec = MeasureTextEx(GetFontDefault(), "BLACK WON!", 50.f, 1.f);
			DrawText("BLACK WON!", (this->m_Screen_Width - this->m_Offset_Left - (int)vec.x) / 2,
				(this->m_Screen_Height - this->m_Offset_Top - (int)vec.y) / 2 - this->m_Offset_Top, 50, Color{ 255, 0, 0, 255 });
		}
		else // Stalemate
		{
			Vector2 vec = MeasureTextEx(GetFontDefault(), "STALEMATE!", 50.f, 1.f);
			DrawText("STALEMATE!", (this->m_Screen_Width - this->m_Offset_Left - (int)vec.x) / 2,
				(this->m_Screen_Height - this->m_Offset_Top - (int)vec.y) / 2, 50, Color{ 255, 0, 0, 255 });
		}
	}

	EndDrawing();
} // VOnRender

void HumanView::VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta)
{
	this->m_ProcessManager.UpdateProcesses(delta);
} // VOnUpdate


void HumanView::TranslateAndDispachInput()
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
	if (this->m_Screen_Width > 0 && this->m_Screen_Height > 0 && this->m_Square_Side > 0 &&
		(this->m_Offset_Left < x && x < this->m_Screen_Width - this->m_Offset_Right) &&
		(this->m_Offset_Top < y && y < this->m_Screen_Height - this->m_Offset_Bottom))
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
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_LeftButtonDown(this->m_Hovering_x, this->m_Hovering_y)));

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_RightButtonDown(this->m_Hovering_x, this->m_Hovering_y)));

	// Keyboard - the only keys relevant are arrows end enters; TODO: alt enters and menu accelerators
	if (IsKeyPressed(KEY_DOWN))
	{
		// Move hovering one down
		if (this->m_Hovering_y != INVALID)
			this->m_Hovering_y = (this->m_Hovering_y + 1) % BOARD_SIDE;
		else
			this->m_Hovering_y = 0;
	}

	if (IsKeyPressed(KEY_UP))
	{
		// Move hovering one up
		if (this->m_Hovering_y != INVALID)
			this->m_Hovering_y = (this->m_Hovering_y - 1 + BOARD_SIDE) % BOARD_SIDE;
		else
			this->m_Hovering_y = BOARD_SIDE - 1;
	}

	if (IsKeyPressed(KEY_RIGHT))
	{
		// Move hovering one right
		if (this->m_Hovering_x != INVALID)
			this->m_Hovering_x = (this->m_Hovering_x + 1) % BOARD_SIDE;
		else
			this->m_Hovering_x = 0;
	}

	if (IsKeyPressed(KEY_LEFT))
	{
		// Move hovering one up
		if (this->m_Hovering_x != INVALID)
			this->m_Hovering_x = (this->m_Hovering_x - 1 + BOARD_SIDE) % BOARD_SIDE;
		else
			this->m_Hovering_x = BOARD_SIDE - 1;
	}

	if (IsKeyPressed(KEY_ENTER))
	{
		if (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT))
		{
			ToggleFullscreen();
			this->VOnResize();
		}
		else if (this->m_Hovering_x!= INVALID && this->m_Hovering_y!= INVALID) // Select hovering
		{
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectSquare(this->m_Hovering_x, this->m_Hovering_y)));
		}
	}	

	/*if (IsKeyPressed(KEY_KP_ENTER))
	{
		this->m_Console.SetActive(true);
	}*/
} // TranslateAndDispachInput




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

	// Draw pieces
	auto& set = Piece::GetSet();
	for (int i = 0; i < set.size(); ++i)
	{
		if (set[i]->IsAlive())
		{
			auto comp = dynamic_cast<Piece2DRenderComponent*>(set[i]->GetComponent(RENDER_COMPONENT_NAME));
			Assert(comp);

			// Coordinates and row/col indeces are kinda reversed
			comp->VOnDraw();
		}
	}

	// Draw possible targets
	size_t moves = this->m_Targets.size();
	for (size_t i = 0; i < moves; ++i)
	{
		int xCoord = this->m_Targets[i] % BOARD_SIDE;
		int yCoord = this->m_Targets[i] / BOARD_SIDE;
		Rectangle rect = {
			float(this->m_Offset_Left + xCoord * this->m_Square_Side),
			float(this->m_Offset_Top + yCoord * this->m_Square_Side),
			float(this->m_Square_Side),
			float(this->m_Square_Side) };

		DrawRectangleLinesEx(rect, 7.5f, GREEN);
	}

#if 0
	// Draw the interface for choosing promotion on board centre
	if (this->m_PawnToPromote != NULL)
	{
		// Find board centre
		int xCoord = this->m_Offset_Left + (BOARD_SIDE * this->m_Square_Side) / 2;
		int yCoord = this->m_Offset_Top + (BOARD_SIDE * this->m_Square_Side) / 2;

		// Draw images on a 2x2 square
		if (this->m_Turn) // White pieces
		{
			this->m_Set[(int)Pieces::White_Rook_1]->GetImage().OnDraw(xCoord - 100, yCoord + 100, this->m_Set[(int)Pieces::White_Rook_1]->GetColor());
			this->m_Set[(int)Pieces::White_Knight_1]->GetImage().OnDraw(xCoord - 100, yCoord - 100, this->m_Set[(int)Pieces::White_Knight_1]->GetColor());
			this->m_Set[(int)Pieces::White_Bishop_1]->GetImage().OnDraw(xCoord, yCoord, this->m_Set[(int)Pieces::White_Bishop_1]->GetColor());
			this->m_Set[(int)Pieces::White_Queen]->GetImage().OnDraw(xCoord, yCoord - 100, this->m_Set[(int)Pieces::White_Queen]->GetColor());
		}
		else // Black pieces
		{
			this->m_Set[(int)Pieces::Black_Rook_1]->GetImage().OnDraw(xCoord, yCoord, this->m_Set[(int)Pieces::Black_Rook_1]->GetColor());
			this->m_Set[(int)Pieces::Black_Knight_1]->GetImage().OnDraw(xCoord, yCoord, this->m_Set[(int)Pieces::Black_Knight_1]->GetColor());
			this->m_Set[(int)Pieces::Black_Bishop_1]->GetImage().OnDraw(xCoord, yCoord, this->m_Set[(int)Pieces::Black_Bishop_1]->GetColor());
			this->m_Set[(int)Pieces::Black_Queen]->GetImage().OnDraw(xCoord, yCoord, this->m_Set[(int)Pieces::Black_Queen]->GetColor());
		}
	}
#endif
} // DrawBoard



void HumanView::OnLeftClick(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_LeftButtonDown& event = *static_cast<Event_LeftButtonDown*>(pEvent.get());

	// Reset selection only if left-clicking on the same square */
	if (event.m_x == INVALID || event.m_y == INVALID || (this->m_Selected_x == event.m_x && this->m_Selected_y == event.m_y))
	{
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectionReset));
	}
	else
	{
		// Select new square
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectSquare(event.m_x, event.m_y)));
	}
} // OnLeftClick Listener

void HumanView::OnRightClick(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Reset selection no matter where cursor is
	s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectionReset));
} // OnRightClick Listener

void HumanView::OnSelectSquare(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_SelectSquare& event = *static_cast<Event_SelectSquare*>(pEvent.get());

	int index = event.m_y * BOARD_SIDE + event.m_x; // Coordinates and indeces are kinda reversed

	// If a piece was already selected
	size_t size = this->m_Targets.size();
	if (size != 0)
	{
		// If new selection corresponds to a possible move
		for (int i = 0; i < size; ++i)
		{
			if (this->m_Targets[i] == index)
			{
				// Fire an event to move the piece that was selected
				int old_sel_index = this->m_Selected_y * BOARD_SIDE + this->m_Selected_x;
				Piece* selected = this->m_Board.m_Board[old_sel_index];
				s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_StartMovePiece(selected, old_sel_index, index)));
				return;
			}
		}

		// If execution gets here then the selection wasn't a valid move
		if (this->m_Board.m_Board[index] != nullptr) // User selected another piece
		{
			// Fire an event to select new piece
			this->m_Selected_x = event.m_x;
			this->m_Selected_y = event.m_y;
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_PieceSelected(index)));
		}
		else // Illegal move
			s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_IllegalMove(index)));
	}
	else if (this->m_Board.m_Board[index] != nullptr) // User selected another piece
	{
		// Fire an event to select new piece
		this->m_Selected_x = event.m_x;
		this->m_Selected_y = event.m_y;
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_PieceSelected(index)));
	}
	else
	{
		// User selected a white space - reset selection (shouldn't be necessary)
		s_GlobalEventManager.TriggerEvent(IEventPtr(ENGINE_NEW Event_SelectionReset));
	}
} // OnSelectSquare Listener

void HumanView::OnSelectionReset(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	this->m_Selected_x = INVALID;
	this->m_Selected_y = INVALID;
} // OnSelectionReset Listener


void HumanView::OnStartMovePiece(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_StartMovePiece& event = *static_cast<Event_StartMovePiece*>(pEvent.get());

	int stop_row = this->m_Offset_Top + (event.m_Dest / BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;
	int stop_col = this->m_Offset_Left + (event.m_Dest % BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;

	auto comp = static_cast<Piece2DRenderComponent*>(event.m_Piece->GetComponent(RENDER_COMPONENT_NAME));
	Assert(comp);

	this->m_ProcessManager.AddProcess(ProcessPtr(
		ENGINE_NEW MovePieceProcess(*comp, MilliToNano(250), stop_row, stop_col)));
} // OnStartMovePiece Listener

void HumanView::OnCastle(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_Castle& event = *static_cast<Event_Castle*>(pEvent.get());

	int stop_row = this->m_Offset_Top + (event.m_Dest / BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;
	int stop_col = this->m_Offset_Left + (event.m_Dest % BOARD_SIDE) * this->m_Square_Side + PIECE_OFFSET;

	auto comp = static_cast<Piece2DRenderComponent*>(event.m_Piece->GetComponent(RENDER_COMPONENT_NAME));
	Assert(comp);

	auto delay = ENGINE_NEW DelayProcess(MilliToNano(100));
	delay->AttachChildProcess(ProcessPtr(ENGINE_NEW MovePieceProcess(*comp, MilliToNano(250), stop_row, stop_col)));
	this->m_ProcessManager.AddProcess(ProcessPtr(delay));
} // OnCastle Listener


void HumanView::OnPawnPromotion(const ENGINE_NAMESPACE::IEventPtr& pEvent)
{
	// Get event
	const Event_PawnPromotion& event = *static_cast<Event_PawnPromotion*>(pEvent.get());
	this->m_PawnPromoter.PromotePawn(event.m_Pawn);
} // OnPawnPromotion Listener


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
MovePieceProcess::MovePieceProcess(Piece2DRenderComponent& image, nanoseconds executionTime, int stop_row_coord, int stop_col_coord)
	: m_TotalTimeAvailable(executionTime), m_Stop_Row(stop_row_coord), m_Stop_Col(stop_col_coord), m_Image(image)
{
	this->m_TimePassedSoFar = 0;
} // Constructor


void MovePieceProcess::VOnUpdate(ENGINE_NAMESPACE::nanoseconds delta)
{
	int x, y;
	this->m_Image.GetCoordinates(x, y);

	if (y == this->m_Stop_Row && x == this->m_Stop_Col)
		this->Succeed();
	else
	{
		this->m_TimePassedSoFar += delta;

		// Since nanosec resolution is way to precise for this task, just consider hundredths of seconds
		const int availTime = (int)(NanoToMilli(this->m_TotalTimeAvailable - this->m_TimePassedSoFar) / 10);

		if (availTime <= 0) // Time expired
		{
			this->m_Image.SetCoordinates(this->m_Stop_Col, this->m_Stop_Row);
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
			this->m_Image.SetCoordinates(x, y);
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