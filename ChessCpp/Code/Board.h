#pragma once

#include "..\..\..\..\..\..\INFORMATICA\MiscLibs\raylib\src\raylib.h"

/*
*	The board is inteded as an array of strings formed by FEN notation characters (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation).
*	The array of string pointers is inteded as the rows of the grid and the actual strings themselfs are the columns.
*/
typedef char** Board;

/*
*	Definition of bool type {0 is false, 1 is true}
*/
#if !defined (TRUE) && !defined (FALSE)
typedef int BOOL;
#define TRUE	1
#define FALSE	0
#endif

/*
*	Definitions for the board, we use FEN notation (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
*	to define every piece on the board and an empty space to indicate that no piece is in that position
*/
#define BOARD_SIDE 8
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

/*
*	Allocates in memory the space for a new board and returns it.
*	In case anything goes wrong returns an NULL pointer.
*/
Board InitBoard();

/*
*	Loads into the board the positions from a FEN string.
*	Returns true if the loading is correct, false if it's not
*/
BOOL LoadFenString(Board board, const char* fen_string);

/* prints the current state of a board on the terminal */
void PrintBoardOnTerminal(Board board);

/* prints the current state of a board on the GUI */
void PrintBoard(Board board, Texture2D* textures);

/* deallocates the board */
void DestroyBoard(Board board);
