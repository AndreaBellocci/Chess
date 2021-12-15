#include <stdio.h>
#include <stdlib.h>
#include "board.h"


Board InitBoard()
{
	/* allocates the rows */
	char** new_board = (char**)malloc(sizeof(char*) * BOARD_SIDE);
	if (new_board == NULL)
		return NULL;

	/* allocates the columns for every row */
	for (int i = 0; i < BOARD_SIDE; i++)
	{	  
		*(new_board + i) = (char*)malloc(sizeof(char) * BOARD_SIDE);
		if (*(new_board + i) == NULL)
		return NULL;
	}
	return new_board;
}

BOOL LoadFenString(Board board, const char* fen_string)
{
	int index = 0; /* used to read from the FEN string */

	int row = 0; /* column and row of the game board */
	int col = 0;

	while (row < BOARD_SIDE)
	{ /* stop reading after we read che end of the last row */

		char current = *(fen_string + index); /* get the current fen string character according to the index */

		if ((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z'))
		{ 
			/* if the char is a letter then it must be a piece */
			*(*(board + row) + col) = current; /* since we use the same notation as FEN strings we can just put the char in the right spot */
			col++;
		}
		else if (current == '/')
		{ /* now we are targeting the next row */
			row++;
			col = 0;
		}
		else if (current >= '0' && current <= '9') 
		{
			/* if the char is a number then it must incate a number of empty spots */
			for (int i = 0; i < current - '0'; i++)
			{
				*(*(board + row) + col) = EMPTY;
				col++;
			}
		}
		else /* if we reach a space we are done */
			return TRUE;

		index++; /* index of the next char */
	}

	return FALSE; /* should not be ever reached */
}

void PrintBoardOnTerminal(Board board)
{
	for (int row = 0; row < BOARD_SIDE; row++)
	{
		for (int col = 0; col < BOARD_SIDE; col++)
			printf(" %c ", *(*(board + row) + col));

		printf("\n");
	}
}

void PrintBoard(Board board, Texture2D* textures)
{
	int step = GetScreenHeight() / BOARD_SIDE; /* step is the lenght of the side of each square */
	int start = (GetScreenWidth() - (step * 8)) / 2; /* start is an offset we calculate to center the board in the window */

	for (int row = 0; row < BOARD_SIDE; row++)
	{
		for (int col = 0; col < BOARD_SIDE; col++)
		{
			int x_pos = (col * step) + start; /* x coorinates of the top right corner of each square */
			int y_pos = row * step; /* y coorinates of the top right corner of each square */

			DrawRectangle(x_pos, y_pos, step, step, (row + col) % 2 == 0 ? GRAY : YELLOW); /* we alternate colors just like in a normal ches board */

			if (*(*(board + row) + col) != ' ')
			{
				/* if the quare is not empty we need to draw a texture */
				x_pos += 10; /* offsets to center the texture */
				y_pos += 10;

				/* texture are saved is a very specific order (bknpqrBKNPQR) by the loader*/
				switch (*(*(board + row) + col))
				{
				case 'b':
					DrawTexture(textures[0], x_pos, y_pos, WHITE);
					break;
				case 'k':
					DrawTexture(textures[1], x_pos, y_pos, WHITE);
					break;
				case 'n':
					DrawTexture(textures[2], x_pos, y_pos, WHITE);
					break;
				case 'p':
					DrawTexture(textures[3], x_pos, y_pos, WHITE);
					break;
				case 'q':
					DrawTexture(textures[4], x_pos, y_pos, WHITE);
					break;
				case 'r':
					DrawTexture(textures[5], x_pos, y_pos, WHITE);
					break;
				case 'B':
					DrawTexture(textures[6], x_pos, y_pos, WHITE);
					break;
				case 'K':
					DrawTexture(textures[7], x_pos, y_pos, WHITE);
					break;
				case 'N':
					DrawTexture(textures[8], x_pos, y_pos, WHITE);
					break;
				case 'P':
					DrawTexture(textures[9], x_pos, y_pos, WHITE);
					break;
				case 'Q':
					DrawTexture(textures[10], x_pos, y_pos, WHITE);
					break;
				case 'R':
					DrawTexture(textures[11], x_pos, y_pos, WHITE);
					break;
				default:
					printf("Error Unknown Piece"); /* GOD PLEASE NEVER MAKE THIS EVER HAPPEN AGAIN */
				}
			}
		}
	}

}

void DestroyBoard(Board board)
{
	for (int i = 0; i < BOARD_SIDE; i++)
		free(*(board + i));

	free(board);
}