#pragma once
#include <exception>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <malloc.h>

#include "Types.h"

enum ErrorCode
{
	InvalidID,
	InvalidSelection,
	InvalidPromotion,
	DataCorrupted,
	UnknownMove,
	KingNotFound,
	KingNotSelected,
	CantOpenXML,
};


class ChessException : public std::exception
{
public:
	ChessException(ErrorCode code, const std::string& file, const std::string& function, int line, const std::string& message)
		: m_Code(code), m_File(file), m_Function(function), m_Line(line)
	{
		this->m_Message = "Error code: " + std::to_string(static_cast<int>(code)) + "\n" +
			"Message: "  + message + "\n" +
			"File: "     + file + "\n" +
			"Function: " + function + "\n" +
			"Line: "     + std::to_string(line);
	} // Constructor

	ChessException(ErrorCode code, const char* file, const char* function, int line, const char* message, ...)
		: m_Code(code), m_File(file), m_Function(function), m_Line(line)
	{
		// Process variadic arguments
		va_list args;
		va_start(args, message);

		// Calculate the length of the formatted message and allocate a buffer
		const auto len = _vscprintf(message, args) + 1; // _vscprintf doesn't count terminating '\0'
		char* buffer = (char*)malloc(len * sizeof(char));
		if (NULL != buffer)
		{
			vsprintf_s(buffer, len, message, args);
			
			this->m_Message = "Error code: " + std::to_string(static_cast<int>(code)) + "\n" +
				"Message: "  + std::string(buffer) + "\n" +
				"File: "     + file + "\n" +
				"Function: " + function + "\n" +
				"Line: "     + std::to_string(line);
			free(buffer);
		}
		else
		{
			this->m_Message = "Error code: " + std::to_string(static_cast<int>(code)) + "\n" +
				"Message: "  + std::string(message) + "(cannot parse var args)\n" +
				"File: "     + file + "\n" +
				"Function: " + function + "\n" +
				"Line: "     + std::to_string(line);
		}
		va_end(args);
	} // Constructor with variadic arguments

	virtual ~ChessException() = default;
	virtual const char* what() const noexcept override { return this->m_Message.c_str(); }

	// Getters
	ErrorCode GetErrorCode() const noexcept			{ return this->m_Code; }
	const std::string& GetFile() const noexcept		{ return this->m_File; }
	const std::string& GetFunction() const noexcept { return this->m_Function; }
	int GetLine() const noexcept					{ return this->m_Line; }


private:
	const ErrorCode m_Code;
	const std::string m_File;
	const std::string m_Function;
	const int m_Line;
	std::string m_Message;
}; // ChessException

#define THROW_CHESS_EXCEPTION(code, message, ...) throw ChessException(code, __FILE__, __func__, __LINE__, message, __VA_ARGS__)


#if _DEBUG
#define CASE(x) case Pieces::x: str = #x; break;
__forceinline static void DebugHelper(Pieces id, int from, int to, const char* action, bool ok)
{
	// Map id to string
	const char* str = nullptr;
	switch (id)
	{
		CASE(Black_Rook_King_Side);  // 1
		CASE(Black_Rook_Queen_Side); // 2
		CASE(Black_Knight_1);		 // 3
		CASE(Black_Knight_2);		 // 4
		CASE(Black_Bishop_1);		 // 5
		CASE(Black_Bishop_2);		 // 6
		CASE(Black_Queen);			 // 7
		CASE(Black_King);			 // 8
		CASE(Black_Pawn_1);			 // 9
		CASE(Black_Pawn_2);			 // 10
		CASE(Black_Pawn_3);			 // 11
		CASE(Black_Pawn_4);			 // 12
		CASE(Black_Pawn_5);			 // 13
		CASE(Black_Pawn_6);			 // 14
		CASE(Black_Pawn_7);			 // 15
		CASE(Black_Pawn_8);			 // 16
		CASE(White_Rook_King_Side);  // 17
		CASE(White_Rook_Queen_Side); // 18
		CASE(White_Knight_1);		 // 19
		CASE(White_Knight_2);		 // 20
		CASE(White_Bishop_1);		 // 21
		CASE(White_Bishop_2);		 // 22
		CASE(White_Queen);			 // 23
		CASE(White_King);			 // 24
		CASE(White_Pawn_1);			 // 25
		CASE(White_Pawn_2);			 // 26
		CASE(White_Pawn_3);			 // 27
		CASE(White_Pawn_4);			 // 28
		CASE(White_Pawn_5);			 // 29
		CASE(White_Pawn_6);			 // 30
		CASE(White_Pawn_7);			 // 31
		CASE(White_Pawn_8);			 // 32
	default: str = "Unknown piece";
	} // switch

	const int src_row = from / BOARD_SIDE;
	const int src_col = from % BOARD_SIDE;
	const int dest_row = to / BOARD_SIDE;
	const int dest_col = to % BOARD_SIDE;

	// Create debug string
	printf("%s from (%d %d) %s to (%d %d) - (%s)\n", str, src_row, src_col, (ok ? "can" : "can't"), dest_row, dest_col, action);
} // DebugHelper
#undef CASE
#else
#define DebugHelper(id, i, j, b)	{ void(0); }
#endif