#include "loader.h"
#include <stdio.h>

Texture2D* LoadAssets(int desired_side_len)
{
	desired_side_len -= 20; /* if the pieces were as wide and as tall as a square it would look ugly */

	/* create the texture array  */
	Texture2D* loaded_textures = (Texture2D*)MemAlloc(sizeof(Texture2D) * 12);
	if (loaded_textures == NULL)
		return NULL;

	/* just watch the printfs */
	printf("\n=== STARTING TO LOAD ASSETS ===\n\n");

	printf("\n=== BLACK BISHOP === \n");
	Image black_bishop_image = LoadImage(BLACK_BISHOP_IMAGE_PATH); /* create an image from a file in memory */
	ImageResizeNN(&black_bishop_image, desired_side_len, desired_side_len); /* resize said image to the desired width and height */
	*(loaded_textures + 0) = LoadTextureFromImage(black_bishop_image); /* then create a texture in vram from said image and sore it in the texture array */
	UnloadImage(black_bishop_image); /* dealocate the image */

	printf("\n=== BLACK KING ===\n");
	Image black_king_image = LoadImage(BLACK_KING_IMAGE_PATH);
	ImageResizeNN(&black_king_image, desired_side_len, desired_side_len);
	*(loaded_textures + 1) = LoadTextureFromImage(black_king_image);
	UnloadImage(black_king_image);

	printf("\n=== BLACK KNIGHT ===\n");
	Image black_knight_image = LoadImage(BLACK_KNIGHT_IMAGE_PATH);
	ImageResizeNN(&black_knight_image, desired_side_len, desired_side_len);
	*(loaded_textures + 2) = LoadTextureFromImage(black_knight_image);
	UnloadImage(black_knight_image);

	printf("\n=== BLACK PAWN ===\n");
	Image black_pawn_image = LoadImage(BLACK_PAWN_IMAGE_PATH);
	ImageResizeNN(&black_pawn_image, desired_side_len, desired_side_len);
	*(loaded_textures + 3) = LoadTextureFromImage(black_pawn_image);
	UnloadImage(black_pawn_image);

	printf("\n=== BLACK QUEEN ===\n");
	Image black_queen_image = LoadImage(BLACK_QUEEN_IMAGE_PATH);
	ImageResizeNN(&black_queen_image, desired_side_len, desired_side_len);
	*(loaded_textures + 4) = LoadTextureFromImage(black_queen_image);
	UnloadImage(black_queen_image);

	printf("\n=== BLACK ROOK ===\n");
	Image black_rook_image = LoadImage(BLACK_ROOK_IMAGE_PATH);
	ImageResizeNN(&black_rook_image, desired_side_len, desired_side_len);
	*(loaded_textures + 5) = LoadTextureFromImage(black_rook_image);
	UnloadImage(black_rook_image);

	printf("\n=== WHITE BISHOP ===\n");
	Image white_bishop_image = LoadImage(WHITE_BISHOP_IMAGE_PATH);
	ImageResizeNN(&white_bishop_image, desired_side_len, desired_side_len);
	*(loaded_textures + 6) = LoadTextureFromImage(white_bishop_image);
	UnloadImage(white_bishop_image);

	printf("\n=== WHITE KING ===\n");
	Image white_king_image = LoadImage(WHITE_KING_IMAGE_PATH);
	ImageResizeNN(&white_king_image, desired_side_len, desired_side_len);
	*(loaded_textures + 7) = LoadTextureFromImage(white_king_image);
	UnloadImage(white_king_image);

	printf("\n=== WHITE KNIGHT ===\n");
	Image white_knight_image = LoadImage(WHITE_KNIGHT_IMAGE_PATH);
	ImageResizeNN(&white_knight_image, desired_side_len, desired_side_len);
	*(loaded_textures + 8) = LoadTextureFromImage(white_knight_image);
	UnloadImage(white_knight_image);

	printf("\n=== WHITE PAWN === \n");
	Image white_pawn_image = LoadImage(WHITE_PAWN_IMAGE_PATH);
	ImageResizeNN(&white_pawn_image, desired_side_len, desired_side_len);
	*(loaded_textures + 9) = LoadTextureFromImage(white_pawn_image);
	UnloadImage(white_pawn_image);

	printf("\n=== WHITE QUEEN ===\n");
	Image white_queen_image = LoadImage(WHITE_QUEEN_IMAGE_PATH);
	ImageResizeNN(&white_queen_image, desired_side_len, desired_side_len);
	*(loaded_textures + 10) = LoadTextureFromImage(white_queen_image);
	UnloadImage(white_queen_image);

	printf("\n=== WHITE ROOK ===\n");
	Image white_rook_image = LoadImage(WHITE_ROOK_IMAGE_PATH);
	ImageResizeNN(&white_rook_image, desired_side_len, desired_side_len);
	*(loaded_textures + 11) = LoadTextureFromImage(white_rook_image);
	UnloadImage(white_rook_image);

	printf("=== FINISHED LOADING ASSETS ===\n\n");
	return loaded_textures;
}

void DestroyAssets(Texture2D* textures)
{
	MemFree(textures); /* BOOOORING */
}