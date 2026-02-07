// Milan, 11ht December 2021
//	Completed on December, 11
//

#pragma once
#include <raylib.h>
#include "Libraries/EngineUtility.h"

// Forward declarations
enum class Pieces : char;

enum class PieceType : char
{
	Invalid = -1,

	Black_Rook,
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
};

// This function is like a factory object, only it's a function and does not create objects
bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);


class IPieceComponent : public ENGINE_NAMESPACE::IDrawable
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
public:
	IPieceComponent(const std::string& fileName, const std::string& itemName)
		: m_FileName(fileName), m_ItemName(itemName) {} // Constructor

	virtual ~IPieceComponent() = default;

	// Used on pawn promotion to change pawn attributes without changing its heart
	virtual void VChangePieceComponent(PieceType id) = 0;

protected:
	const std::string m_FileName;
	std::string m_ItemName;
}; // End Interface IPieceComponent


class Piece2DRenderComponent : public IPieceComponent
{
	friend bool SetUpPieceFromFile(const std::string& xmlName, Pieces pieceID, const std::string& pieceName);
public:
	Piece2DRenderComponent(const std::string& fileName, const std::string& itemName, const std::string& path, PieceType type)
		: IPieceComponent(fileName, itemName), m_Path(path), m_TypeID(type) {} // Constructor

	virtual void VOnDraw() override final;

	void SetCoordinates(int x, int y);
	void GetCoordinates(int& x, int& y) const;

	virtual void VChangePieceComponent(PieceType type) override final;

protected:
	std::string m_Path;
	PieceType m_TypeID;	
	const Texture2D* m_Image = nullptr;

	Color m_Color		= WHITE;
	float m_Rotation	= 0.0f;
	float m_Scale		= 1.0f;
	
private:
	int m_X = INVALID, m_Y = INVALID;	
}; // End class Piece2DRenderComponent Declaration