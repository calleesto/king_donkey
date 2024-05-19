#include "drawLevels.h"



void drawLevelTwo(SDL_Surface* screen, int green, int blue, int red) {
	DrawRectangle(screen, PLATFORM_TWO_X, GROUND_Y, SCREEN_WIDTH, GROUND_HEIGHT, blue, blue);

	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_ONE_Y, PLATFORM_LENGHT, red);
	drawPlatform(screen, PLATFORM_TWO_X, PLATFORM_ROW_TWO_Y, PLATFORM_LENGHT, red);
	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_THREE_Y, PLATFORM_LENGHT, red);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, green);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, green);

	drawLadder(screen, LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, green);
	drawLadder(screen, LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, green);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, green);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, green);
}

void drawLevelThree(SDL_Surface* screen, int green, int blue, int red) {
	DrawRectangle(screen, PLATFORM_TWO_X, GROUND_Y, SCREEN_WIDTH, GROUND_HEIGHT, red, red);

	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_ONE_Y, PLATFORM_LENGHT, green);
	drawPlatform(screen, PLATFORM_TWO_X, PLATFORM_ROW_TWO_Y, PLATFORM_LENGHT, green);
	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_THREE_Y, PLATFORM_LENGHT, green);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, blue);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, blue);

	drawLadder(screen, LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, blue);
	drawLadder(screen, LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, blue);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, blue);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, blue);
}