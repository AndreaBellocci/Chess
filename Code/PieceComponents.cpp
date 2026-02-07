// Milan, 11ht December 2021
//	Completed on December, 11
//

#include <array>
#include "../Libraries/ThirdParty/TinyXML2/tinyxml2.h"
#include "../Libraries/Logger.h"
#include "PieceComponents.h"
#include "GameViews.h"
#include "Pieces.h"

using namespace ENGINE_NAMESPACE;

bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName)
{
	// Open file TODO: use resource cache module, do not open and close file every time but cache it
	tinyxml2::XMLDocument file;

	auto ret = file.LoadFile(xmlName.c_str());
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
			auto thisPiece = piecesElem->FirstChildElement(pieceName.c_str());
			if (thisPiece)
			{
				auto rendering = thisPiece->FirstChildElement("Rendering");
				if (rendering)
				{
					std::string path;
					float scale, rotation;
					unsigned char r, g, b, a;

					auto attr = rendering->FindAttribute("ImagePath");
					Assert(attr && "Cannot find path to image file");
					path = attr->Value();
					Assert(!path.empty() && "Invalid data");

					attr = rendering->FindAttribute("Rotation");	if (attr) rotation = attr->FloatValue(); else rotation = 0.0f;
					attr = rendering->FindAttribute("Scale");		if (attr) scale = attr->FloatValue(); else scale = 1.0f;
					attr = rendering->FindAttribute("r");			if (attr) r = (unsigned char)attr->UnsignedValue(); else r = 255;
					attr = rendering->FindAttribute("g");			if (attr) g = (unsigned char)attr->UnsignedValue(); else g = 255;
					attr = rendering->FindAttribute("b");			if (attr) b = (unsigned char)attr->UnsignedValue(); else b = 255;
					attr = rendering->FindAttribute("a");			if (attr) a = (unsigned char)attr->UnsignedValue(); else a = 255;

					PieceType id;
					switch (pieceID)
					{
					case Pieces::Black_Rook_1:		case Pieces::Black_Rook_2:		id = PieceType::Black_Rook;		break;
					case Pieces::Black_Knight_1:	case Pieces::Black_Knight_2:	id = PieceType::Black_Knight;	break;
					case Pieces::Black_Bishop_1:	case Pieces::Black_Bishop_2:	id = PieceType::Black_Bishop;	break;
					case Pieces::Black_Queen:										id = PieceType::Black_Queen;	break;
					case Pieces::Black_King:										id = PieceType::Black_King;		break;
					case Pieces::Black_Pawn_1: case Pieces::Black_Pawn_2: case Pieces::Black_Pawn_3: case Pieces::Black_Pawn_4:
					case Pieces::Black_Pawn_5: case Pieces::Black_Pawn_6: case Pieces::Black_Pawn_7: case Pieces::Black_Pawn_8:
						id = PieceType::Black_Pawn; break;

					case Pieces::White_Rook_1:		case Pieces::White_Rook_2:		id = PieceType::White_Rook;		break;
					case Pieces::White_Knight_1:	case Pieces::White_Knight_2:	id = PieceType::White_Knight;	break;
					case Pieces::White_Bishop_1:	case Pieces::White_Bishop_2:	id = PieceType::White_Bishop;	break;
					case Pieces::White_Queen:										id = PieceType::White_Queen;	break;
					case Pieces::White_King:										id = PieceType::White_King;		break;
					case Pieces::White_Pawn_1: case Pieces::White_Pawn_2: case Pieces::White_Pawn_3: case Pieces::White_Pawn_4:
					case Pieces::White_Pawn_5: case Pieces::White_Pawn_6: case Pieces::White_Pawn_7: case Pieces::White_Pawn_8:
						id = PieceType::White_Pawn; break;
					default: Assert(false && "Invalid piece id");
					} // end switch

					// Create piece - its constructor will insert it into the piece set
					Piece* piece = nullptr;
					switch (id)
					{
					case PieceType::Black_Rook:		case PieceType::White_Rook:		piece = ENGINE_NEW Rook(pieceID, pieceName);	break;
					case PieceType::Black_Knight:	case PieceType::White_Knight:	piece = ENGINE_NEW Knight(pieceID, pieceName);	break;
					case PieceType::Black_Bishop:	case PieceType::White_Bishop:	piece = ENGINE_NEW Bishop(pieceID, pieceName);	break;
					case PieceType::Black_Queen:	case PieceType::White_Queen:	piece = ENGINE_NEW Queen(pieceID, pieceName);	break;
					case PieceType::Black_King:		case PieceType::White_King:		piece = ENGINE_NEW King(pieceID, pieceName);	break;
					case PieceType::Black_Pawn:		case PieceType::White_Pawn:		piece = ENGINE_NEW Pawn(pieceID, pieceName);	break;
					default: Assert(false && "Invalid piece id");
					} // switch

					Assert(piece && "Out of memory");

					// Create components
					Piece2DRenderComponent* render = ENGINE_NEW Piece2DRenderComponent(xmlName, "Rendering", path, id);
					Assert(render && "Out of memory");
					render->m_Color = Color{ r, g, b, a };
					render->m_Rotation = rotation;
					render->m_Scale = scale;
					render->m_Image = TextureLoader::LoadPieceTextureFromFile(id, path);
					Assert(render->m_Image && "No image found");

					// Insert components
					piece->m_Components["Render_Component"] = std::shared_ptr<IPieceComponent>(render);
					//HumanView::GetHumanView().AddObjectToDraw(piece->m_Components["Render_Component"]);
					return true; // All other cases will shut down the program
				}
			}
		}
	}

	return false;
} // End function CreatePieceFromFile

void Piece2DRenderComponent::VOnDraw()
{
	DrawTextureEx(*this->m_Image, Vector2{ (float)this->m_X, (float)this->m_Y }, this->m_Rotation, this->m_Scale, this->m_Color);
} // OnDraw

void Piece2DRenderComponent::VChangePieceComponent(PieceType id)
{
	this->m_Image = TextureLoader::GetPieceTextureAndName(id, this->m_ItemName);
	Assert(this->m_Image && "No image found");
} // VChangePieceComponent


void Piece2DRenderComponent::SetCoordinates(int x, int y)
{
	if (x == INVALID || y == INVALID)
	{
		this->m_X = INVALID;
		this->m_Y = INVALID;
	}
	else
	{
		this->m_X = x;
		this->m_Y = y;
	}
} // SetCoordinates

void Piece2DRenderComponent::GetCoordinates(int& x, int& y) const
{
	x = this->m_X;
	y = this->m_Y;
} // GetCoordinates