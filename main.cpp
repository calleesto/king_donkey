#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <cstdlib> 

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

//screen resolution
#define SCREEN_WIDTH			640
#define SCREEN_HEIGHT			480

//level elements
#define PLATFORM_HEIGHT			10
#define LADDER_WIDTH			50
#define LADDER_HEIGHT			113.0
#define GROUND_HEIGHT			30

//main character position speed and dimensions
#define MC_SPAWN_X				SCREEN_WIDTH - 100
#define MC_SPAWN_Y				SCREEN_HEIGHT - 55
#define MC_WIDTH				50
#define MC_HEIGHT				50


#define LADDER_CLIMB_SPEED		200.0 // higher = slower
#define LADDER_SLIDE_SPEED		200.0 // higher = slower


//IMPORTANTE SETTINGS!!!!!!!!!!!

#define ANIMATION_SPEED			0.2 // higher = slower
#define BARREL_ANIMATION_SPEED	0.2
#define MC_SPEED				0.6 // higher = faster
#define BARREL_SPEED			250
#define MC_FALLING_SPEED		100
#define JUMP_SPEED				0.3
#define SENSITIVITY_RADIUS		20
#define JUMP_PEAK				60
#define JUMP_TIME				0.5
#define BARREL_SPAWN_INTERVAL	3




//level one element hitboxes
#define LADDER_COLUMN_ONE_X		SCREEN_WIDTH / 3 * 2
#define LADDER_COLUMN_TWO_X		SCREEN_WIDTH / 2
#define LADDER_COLUMN_THREE_X	SCREEN_WIDTH / 3
#define LADDER_COLUMN_FOUR_X	SCREEN_WIDTH / 5
#define LADDER_ROW_ONE_Y		(SCREEN_HEIGHT - GROUND_HEIGHT) / 4 * 3
#define LADDER_ROW_TWO_Y		(SCREEN_HEIGHT - GROUND_HEIGHT) / 4 * 2
#define LADDER_ROW_THREE_Y		(SCREEN_HEIGHT - GROUND_HEIGHT) / 4
#define PLATFORM_ROW_ONE_Y		(SCREEN_HEIGHT - GROUND_HEIGHT) / 4 * 3
#define PLATFORM_ROW_TWO_Y		(SCREEN_HEIGHT - GROUND_HEIGHT) / 4 * 2
#define PLATFORM_ROW_THREE_Y		(SCREEN_HEIGHT - GROUND_HEIGHT) / 4
#define PLATFORM_LENGHT			SCREEN_WIDTH / 7 * 6
#define PLATFORM_ONE_X			SCREEN_WIDTH / 7
#define PLATFORM_TWO_X			0
#define GROUND_Y				SCREEN_HEIGHT - GROUND_HEIGHT

//barrel parameters
#define BARREL_WIDTH			30
#define BARREL_HEIGHT			30

#define BARREL_SPAWN_X			SCREEN_WIDTH
#define BARREL_SPAWN_Y			97

#define ANTAGONIST_SPAWN_X		SCREEN_WIDTH - 100
#define ANTAGONIST_SPAWN_Y		PLATFORM_ROW_THREE_Y - BARREL_HEIGHT



#define MAX_BARRELS				1000
#define JUMP_VELOCITY			-40
#define GRAVITY					9.8

// y coordinates for mc when on a certain platform
#define MC_ON_GROUND_Y			425	
#define MC_ON_PLATFORM_ONE_Y	312
#define MC_ON_PLATFORM_TWO_Y	199	
#define MC_ON_PLATFORM_THREE_Y	86


void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset);
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void drawLadder(SDL_Surface* screen, int x, int y, int blue);
void drawPlatform(SDL_Surface* screen, int x, int y, int l, int blue);
int checkCollisionWithLadder(int charX, int charY, int charWidth, int charHeight, struct Hitbox* ladderHitbox);
void drawLevelOne(SDL_Surface* screen, int green, int blue, int red, int morro, int grey, int light_purple);
void barrelSpinLeft(struct Barrel* barrel, struct Player* mainchar, struct GameState* gameState, SDL_Surface* screen, struct Elements* element);
void barrelSpinRight(struct Barrel* barrel, struct Player* mainchar, struct GameState* gameState, SDL_Surface* screen, struct Elements* element);
void animations(SDL_Surface* screen, struct Player* mainchar, struct KeyboardInstructions* instructions, struct GameState* gameState, struct Elements* element, struct Barrel* barrel);
void UpdateBarrel(struct Barrel* barrel, struct Player* mainchar, SDL_Surface* screen, SDL_Surface* barrelbmp, struct GameState* gameState, struct Elements* element);
void SpawnBarrel(struct Barrel* barrel);
int loadAllImages(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, struct Player* mainchar, struct Elements* element);
int initializeSDL(struct Player* mainchar, struct Elements* element, struct Colors* colors, SDL_Event* event, SDL_Surface** screen, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer);
void printAllVisuals(SDL_Surface* screen, struct Colors* colors, struct KeyboardInstructions* instructions, struct Elements* element, struct Player* mainchar, struct GameState* gameState, struct Barrel barrel[MAX_BARRELS]);
void spawnBarrels(struct Player* mainchar, SDL_Surface* screen, struct Barrel barrels[MAX_BARRELS], struct Elements* element, struct GameState* gameState, struct Barrel* barrel);
void mcMovement(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS], SDL_Surface* screen, struct GameState* gameState);
void updateRenderer(SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer);
void fallingOffPlatform(struct Player* mainchar, struct GameState* gameState);
void updateRenderer(SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer);
void closingGame(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, struct Elements* element);
void ifVictory(struct Player* mainchar, struct KeyboardInstructions* instructions, SDL_Surface* screen, struct GameState* gameState, struct Colors* colors, struct Elements* element);
void printLadderCords();
void updateCharacterMovement(struct Hitbox ladders[6], SDL_Surface* screen, struct Player* mainchar, struct KeyboardInstructions* instructions, struct GameState* gameState, struct Hitbox* ladderHitbox);
void barrelAnimations(struct Barrel* barrel, struct Player* mainchar, struct GameState* gameState, SDL_Surface* screen, struct Elements* element);

struct KeyboardInstructions {
	SDL_Event event;
	int quit;
	double worldTime;
	int onLadder;
	bool keys[SDL_NUM_SCANCODES] = { false };
};

struct Hitbox {
	double x;
	double y;
	double width;
	double height;
};

struct Barrel {
	double x;
	double y;
	bool isAlive;
};

//main character surfaces as well as state checkers
struct Player {
	double mcX;
	double mcY;
	SDL_Surface* mc;
	SDL_Surface* mc_jump;
	SDL_Surface* mc_jump1;
	SDL_Surface* mc_jump2;
	SDL_Surface* mc_left1;
	SDL_Surface* mc_left2;
	SDL_Surface* mc_right1;
	SDL_Surface* mc_right2;
	SDL_Surface* mc_dead;
	SDL_Surface* mc_climb1;
	SDL_Surface* mc_climb2;
	SDL_Surface* mc_slide;
	SDL_Surface* activeImage;
	bool isAlive;
	bool isJumping;
	bool isJumpingLeft;
	bool isJumpingRight;
	bool movingRight;
	bool movingLeft;
	bool isClimbing;
	bool isDescending;
	bool falling;
};

//the rest of the surface elements like ladder platform etc
struct Elements {
	SDL_Surface* charset;
	SDL_Surface* barrelbmp;
	SDL_Surface* explosion;
	SDL_Surface* antagonist;
	SDL_Surface* antagonist2;
	SDL_Surface* sukuna1;
	SDL_Surface* sukuna2;
	SDL_Surface* sukuna3;
	SDL_Surface* eti;
	SDL_Surface* barrel1;
	SDL_Surface* barrel2;
	SDL_Surface* barrel3;
	SDL_Surface* barrel4;
};

struct Colors {
	int black;
	int red; 
	int green;
	int blue;
	int white;
	int purple;
	int yellow;
	int cyan;
	int morro;
	int light_purple;
	int grey;
	char text[128];
};

struct GameState {
	double delta;
	double epsilon;
	double timeSinceLastSpawn;
	double timeTracker;
	double barrelTimeTracker;
	double timerOne;
	double jumpTimer;
	double jumpTimerLeft;
	double jumpTimerRight;
	double fpsTimer;
	double fps;
	int frames;
};

//default sdl functions
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};

void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

//custom functions
void drawLadder(SDL_Surface* screen, int x, int y, int blue) {
	DrawRectangle(screen, x, y, LADDER_WIDTH, LADDER_HEIGHT, blue, blue);
}

void drawPlatform(SDL_Surface* screen, int x, int y, int l, int blue) {
	DrawRectangle(screen, x, y, l, PLATFORM_HEIGHT, blue, blue);
}

int checkCollisionWithLadder(int charX, int charY, int charWidth, int charHeight, struct Hitbox* ladderHitbox) {
	return (
		charX >= ladderHitbox->x &&
		charX <= ladderHitbox->x + ladderHitbox->width &&
		charY >= ladderHitbox->y - (charHeight) &&
		charY <= ladderHitbox->y + ladderHitbox->height
		);
}

void drawLevelOne(SDL_Surface* screen, int green, int blue, int red, int morro, int grey, int light_purple) {
	DrawRectangle(screen, PLATFORM_TWO_X, GROUND_Y, SCREEN_WIDTH, GROUND_HEIGHT, morro, morro);

	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_ONE_Y, PLATFORM_LENGHT, morro);
	drawPlatform(screen, PLATFORM_TWO_X, PLATFORM_ROW_TWO_Y, PLATFORM_LENGHT, morro);
	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_THREE_Y, PLATFORM_LENGHT, morro);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, grey);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, grey);

	drawLadder(screen, LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, grey);
	drawLadder(screen, LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, grey);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, grey);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, grey);
}

void animations(SDL_Surface* screen, struct Player* mainchar, struct KeyboardInstructions* instructions, struct GameState* gameState, struct Elements* element, struct Barrel* barrel) {
	// MAIN CHARACTER ANIMATIONS


	// MC DEAD 
	if (mainchar->isAlive == 0) {
		mainchar->activeImage = mainchar->mc_dead;
	}
	// MOVING RIGHT 1
	else if (mainchar->isAlive && mainchar->movingRight && gameState->timeTracker < ANIMATION_SPEED) {
		mainchar->activeImage=mainchar->mc_right1;
	}
	// MOVING RIGHT 2
	else if (mainchar->isAlive && mainchar->movingRight && gameState->timeTracker >= ANIMATION_SPEED) {
		mainchar->activeImage = mainchar->mc_right2;
		if (gameState->timeTracker >= 2 * ANIMATION_SPEED) {
			gameState->timeTracker = 0;
		}
	}
	// MOVING LEFT 1
	else if (mainchar->isAlive && mainchar->movingLeft && gameState->timeTracker < ANIMATION_SPEED) {
		mainchar->activeImage=mainchar->mc_left1;
	}
	// MOVING LEFT 2
	else if (mainchar->isAlive && mainchar->movingLeft && gameState->timeTracker >= ANIMATION_SPEED) {
		mainchar->activeImage = mainchar->mc_left2;
		if (gameState->timeTracker >= 2 * ANIMATION_SPEED) {
			gameState->timeTracker = 0;
		}
	}
	//if (mainchar->isDescending) {
	//	mainchar->activeImage = mainchar->mc_jump;
	//}
	// CLIMBING 1
		// STANDING STILL
	else if (mainchar->isAlive && mainchar->isJumpingLeft) {
		mainchar->activeImage = mainchar->mc_jump1;
	}
	else if (mainchar->isAlive && mainchar->isJumpingRight) {
		mainchar->activeImage = mainchar->mc_jump2;
	}

	else if (mainchar->isAlive && gameState->timeTracker < ANIMATION_SPEED && mainchar->isClimbing) {
		mainchar->activeImage = mainchar->mc_climb1;
	}
	// CLIMBING 2
	else if (mainchar->isAlive && gameState->timeTracker >= ANIMATION_SPEED && mainchar->isClimbing) {
		mainchar->activeImage = mainchar->mc_climb2;
		if (gameState->timeTracker >= 2 * ANIMATION_SPEED) {
			gameState->timeTracker = 0;
		}
	}
	else if (mainchar->isAlive && mainchar->isJumping && instructions->onLadder) {
		mainchar->activeImage = mainchar->mc_jump;
	}
	else if (mainchar->isAlive && mainchar->isDescending) {
		mainchar->activeImage = mainchar->mc_slide;
	}
	else if (mainchar->isAlive && instructions->onLadder) {
		mainchar->activeImage = mainchar->mc_climb1;
	}
	else if (mainchar->isAlive) {
		mainchar->activeImage = mainchar->mc;
	}
	DrawSurface(screen, mainchar->activeImage, mainchar->mcX, mainchar->mcY);



	//ANTAGONIST ANIMATIONS

	if (gameState->timeSinceLastSpawn < BARREL_SPAWN_INTERVAL && mainchar->isAlive) {
		DrawSurface(screen, element->sukuna1, ANTAGONIST_SPAWN_X + BARREL_WIDTH, ANTAGONIST_SPAWN_Y);
	}
	if (gameState->timeSinceLastSpawn >= BARREL_SPAWN_INTERVAL - 2 && mainchar->isAlive) {
		DrawSurface(screen, element->sukuna2, ANTAGONIST_SPAWN_X + BARREL_WIDTH, ANTAGONIST_SPAWN_Y);
	}
	if (gameState->timeSinceLastSpawn >= BARREL_SPAWN_INTERVAL - 1.5 && mainchar->isAlive) {
		DrawSurface(screen, element->sukuna1, ANTAGONIST_SPAWN_X + BARREL_WIDTH, ANTAGONIST_SPAWN_Y);
	}
	if (gameState->timeSinceLastSpawn >= BARREL_SPAWN_INTERVAL - 1 && mainchar->isAlive) {
		DrawSurface(screen, element->sukuna2, ANTAGONIST_SPAWN_X + BARREL_WIDTH, ANTAGONIST_SPAWN_Y);
	}
	if (gameState->timeSinceLastSpawn >= BARREL_SPAWN_INTERVAL - 0.5 && mainchar->isAlive) {
		DrawSurface(screen, element->sukuna1, ANTAGONIST_SPAWN_X + BARREL_WIDTH, ANTAGONIST_SPAWN_Y);
	}
	if (mainchar->isAlive == 0) {
		DrawSurface(screen, element->sukuna3, ANTAGONIST_SPAWN_X + BARREL_WIDTH, ANTAGONIST_SPAWN_Y);
	}
}

void barrelSpinLeft(struct Barrel* barrel, struct Player* mainchar, struct GameState* gameState, SDL_Surface* screen, struct Elements* element) {
	if (gameState->barrelTimeTracker < BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker >= 0) {
		DrawSurface(screen, element->barrel1, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker >= BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker <= 2 * BARREL_ANIMATION_SPEED) {
		DrawSurface(screen, element->barrel4, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker > 2 * BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker <= 3 * BARREL_ANIMATION_SPEED) {
		DrawSurface(screen, element->barrel3, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker >= 3 * BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker <= 4 * BARREL_ANIMATION_SPEED) {
		DrawSurface(screen, element->barrel2, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker >= 4 * BARREL_ANIMATION_SPEED) {
		gameState->barrelTimeTracker = 0;
	}
}

void barrelSpinRight(struct Barrel* barrel, struct Player* mainchar, struct GameState* gameState, SDL_Surface* screen, struct Elements* element) {
	if (gameState->barrelTimeTracker < BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker >= 0) {
		DrawSurface(screen, element->barrel1, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker >= BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker <= 2 * BARREL_ANIMATION_SPEED) {
		DrawSurface(screen, element->barrel2, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker > 2 * BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker <= 3 * BARREL_ANIMATION_SPEED) {
		DrawSurface(screen, element->barrel3, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker >= 3 * BARREL_ANIMATION_SPEED && gameState->barrelTimeTracker <= 4 * BARREL_ANIMATION_SPEED) {
		DrawSurface(screen, element->barrel4, barrel->x, barrel->y);
	}
	if (gameState->barrelTimeTracker >= 4 * BARREL_ANIMATION_SPEED) {
		gameState->barrelTimeTracker = 0;
	}
}

void barrelAnimations(struct Barrel* barrel, struct Player* mainchar, struct GameState* gameState, SDL_Surface* screen, struct Elements* element) {
	//HORIZONTAL LEFT 3RD PLATFORM
	if (barrel->y == BARREL_SPAWN_Y) {
		barrelSpinLeft(barrel, mainchar, gameState, screen, element);
	}
	
	//3RD PLATFORM BUMP LEFT 
	if (barrel->x < 50 && barrel->y <= (PLATFORM_ROW_THREE_Y / 2) + BARREL_SPAWN_Y) {
		barrelSpinLeft(barrel, mainchar, gameState, screen, element);
	}

	//3RD PLATFORM BUMP RIGHT TO 2ND PLATFORM
	if (barrel->x < 50 && barrel->y <= PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y && barrel->y >= (PLATFORM_ROW_THREE_Y / 2) + BARREL_SPAWN_Y) {
		barrelSpinRight(barrel, mainchar, gameState, screen, element);
	}

	//2ND PLATFORM HORIZONTAL RIGHT
	if (barrel->y == PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
		barrelSpinRight(barrel, mainchar, gameState, screen, element);
	}

	// 2ND PLATFORM BUMP RIGHT
	if (barrel->x > BARREL_SPAWN_X - 100 && barrel->y <= (3 / 2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y && barrel->y >= PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
		barrelSpinRight(barrel, mainchar, gameState, screen, element);
	}

	//2ND PLATFORM BUMP LEFT TO 1ST PLATFORM
	if (barrel->x >= BARREL_SPAWN_X - 100 && barrel->y > (3 / 2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y && barrel->y < (2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrelSpinLeft(barrel, mainchar, gameState, screen, element);
	}

	// 1ST PLATFORM HORIZONTAL LEFT
	if (barrel->y == (2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrelSpinLeft(barrel, mainchar, gameState, screen, element);
	}
	// 1ND PLATFORM BUMP RIGHT
	if (barrel->x < 50 && barrel->y <= (5 / 2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y && barrel->y >= 2 * PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
		barrelSpinRight(barrel, mainchar, gameState, screen, element);
	}

	//1ND PLATFORM BUMP LEFT TO 1ST PLATFORM
	if (barrel->x < 50 && barrel->y > (5 / 2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y && barrel->y < (3 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrelSpinLeft(barrel, mainchar, gameState, screen, element);
	}

	//GROUND HORIZONTAL RIGHT
	if (barrel->y == (3 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrelSpinRight(barrel, mainchar, gameState, screen, element);
	}
}

//set path for the barrels and if statement for contact with main character
void UpdateBarrel(struct Barrel* barrel, struct Player* mainchar, SDL_Surface* screen, SDL_Surface* barrelbmp, struct GameState* gameState, struct Elements* element) {
	//PLATFORM 3 HORIZONTAL MOVEMENT
	if (barrel->x >= 50 && barrel->y == BARREL_SPAWN_Y) {
		barrel->x -= (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);

	}

	//PLATFORM 3 DROP TO PLATFROM 2
	if (barrel->x < 50) {
		if (barrel->y <= (PLATFORM_ROW_THREE_Y/2) + BARREL_SPAWN_Y) {
			barrel->x -= (BARREL_SPEED * gameState->delta);
			barrel->y += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
		}
		else if (barrel->y < PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y && barrel->y >= (PLATFORM_ROW_THREE_Y / 2) + BARREL_SPAWN_Y) {
			barrel->x += (BARREL_SPEED * gameState->delta);
			barrel->y += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
		}
		else if (barrel->y >= PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
			barrel->y = PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y;
		}
	}

	//PLATFORM 2 HORIZONTAL MOVEMENT
	if (barrel->x <= BARREL_SPAWN_X - 100 && barrel->y == PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
		//barrel->x = 550;
		barrel->x += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
	}

	//PLATFORM 2 DROP TO PLATFROM 1
	if (barrel->x > BARREL_SPAWN_X - 100) {
		if (barrel->y >= PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y && barrel->y < (PLATFORM_ROW_THREE_Y / 2 * 3) + BARREL_SPAWN_Y) {
			barrel->x += (BARREL_SPEED * gameState->delta);
			barrel->y += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
		}
		else if (barrel->y >= (PLATFORM_ROW_THREE_Y / 2 * 3) + BARREL_SPAWN_Y && barrel->y < (2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
			barrel->x -= (BARREL_SPEED * gameState->delta);
			barrel->y += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
		}
		else if (barrel->y >= (2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y - 10) {
			barrel->y = (2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y;
		}
	}
	
	//PLATFORM 1 HORIZONTAL MOVEMENT
	if (barrel->x >= 50 && barrel->y == (2 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrel->x -= (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
	}

	//PLATFORM 1 DROP TO GROUND
	if (barrel->x < 50) {
		if (barrel->y < (PLATFORM_ROW_THREE_Y / 2 * 5) + BARREL_SPAWN_Y && barrel->y >= 2 * PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
			barrel->x -= (BARREL_SPEED * gameState->delta);
			barrel->y += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
		}
		else if (barrel->y < 3 * PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y && barrel->y >= (PLATFORM_ROW_THREE_Y / 2 * 5) + BARREL_SPAWN_Y) {
			barrel->x += (BARREL_SPEED * gameState->delta);
			barrel->y += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
		}
		else if (barrel->y >= 3 * PLATFORM_ROW_THREE_Y + BARREL_SPAWN_Y) {
			barrel->y = (3 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y;
		}

	}

	//GROUND HORIZONTAL MOVEMENT
	if (barrel->x <= SCREEN_WIDTH && barrel->y == (3 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrel->x += (BARREL_SPEED * gameState->delta);
			barrelAnimations(barrel, mainchar, gameState, screen, element);
	}

	//DESPAWN BARREL
	if (barrel->x >= SCREEN_WIDTH && barrel->y == (3 * PLATFORM_ROW_THREE_Y) + BARREL_SPAWN_Y) {
		barrel->isAlive = false;
	}
}

void spawnBarrels(struct Player* mainchar, SDL_Surface* screen, struct Barrel barrels[MAX_BARRELS], struct Elements* element, struct GameState* gameState, struct Barrel* barrel) {
	if (gameState->timeSinceLastSpawn >= BARREL_SPAWN_INTERVAL) {
		for (int i = 0; i < MAX_BARRELS; ++i) {
			if (!barrels[i].isAlive && mainchar->isAlive) {
				//DrawSurface(screen, element->antagonist, BARREL_SPAWN_X + BARREL_WIDTH, BARREL_SPAWN_Y);
				SpawnBarrel(&barrels[i]);
				break;
			}
		}
		gameState->timeSinceLastSpawn = 0.0;
	}

	for (int i = 0; i < MAX_BARRELS; ++i) {
		if (barrels[i].isAlive) {
			UpdateBarrel(&barrels[i], mainchar, screen, element->barrelbmp, gameState, element);
if (fabs(barrels[i].x - mainchar->mcX) <= gameState->epsilon && fabs(barrels[i].y - mainchar->mcY) <= gameState->epsilon) { // fabs is basically absolute value
				mainchar->isAlive = false;
				barrels[i].isAlive = false;
				//DrawSurface(screen, element->explosion, barrel->x, barrel->y);
			}
		}
	}
}

void SpawnBarrel(struct Barrel* barrel) {
	barrel->isAlive = true;
	barrel->x = BARREL_SPAWN_X;
	barrel->y = BARREL_SPAWN_Y;
}

//loads all bmps
int loadAllImages(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, struct Player* mainchar, struct Elements* element) {
	const char* filename = "->/gojo.bmp";
	element->charset = SDL_LoadBMP("./cs8x8.bmp");
	if (element->charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(element->charset, true, 0x000000);

	element->eti = SDL_LoadBMP("./eti.bmp");
	if (element->eti == NULL) {
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc = SDL_LoadBMP("./gojo.bmp");
	if (mainchar->mc == NULL) {
		printf("SDL_LoadBMP(gojo.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_left1 = SDL_LoadBMP("./gojo_left1.bmp");
	if (mainchar->mc_left1 == NULL) {
		printf("SDL_LoadBMP(gojo_left.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_left2 = SDL_LoadBMP("./gojo_left2.bmp");
	if (mainchar->mc_left2 == NULL) {
		printf("SDL_LoadBMP(gojo_left2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_right1 = SDL_LoadBMP("./gojo_right1.bmp");
	if (mainchar->mc_right1 == NULL) {
		printf("SDL_LoadBMP(gojo_right1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_right2 = SDL_LoadBMP("./gojo_right2.bmp");
	if (mainchar->mc_right2 == NULL) {
		printf("SDL_LoadBMP(gojo_right2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_jump = SDL_LoadBMP("./gojo_jump.bmp");
	if (mainchar->mc_jump == NULL) {
		printf("SDL_LoadBMP(gojo_jump.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_jump1 = SDL_LoadBMP("./gojo_jump1.bmp");
	if (mainchar->mc_jump1 == NULL) {
		printf("SDL_LoadBMP(gojo_jump.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_jump2 = SDL_LoadBMP("./gojo_jump2.bmp");
	if (mainchar->mc_jump2 == NULL) {
		printf("SDL_LoadBMP(gojo_jump2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_climb1 = SDL_LoadBMP("./gojo_climb1.bmp");
	if (mainchar->mc_climb1 == NULL) {
		printf("SDL_LoadBMP(gojo_jump.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_climb2 = SDL_LoadBMP("./gojo_climb2.bmp");
	if (mainchar->mc_climb2 == NULL) {
		printf("SDL_LoadBMP(gojo_jump.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_slide = SDL_LoadBMP("./gojo_slide.bmp");
	if (mainchar->mc_slide == NULL) {
		printf("SDL_LoadBMP(gojo_slide.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_dead = SDL_LoadBMP("./mc_dead.bmp");
	if (mainchar->mc_dead == NULL) {
		printf("SDL_LoadBMP(mc_dead.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->barrelbmp = SDL_LoadBMP("./barrel1.bmp");
	if (element->barrelbmp == NULL) {
		printf("SDL_LoadBMP(barrel1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->barrel1 = SDL_LoadBMP("./barrel1.bmp");
	if (element->barrel1 == NULL) {
		printf("SDL_LoadBMP(barrel1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->barrel2 = SDL_LoadBMP("./barrel2.bmp");
	if (element->barrel2 == NULL) {
		printf("SDL_LoadBMP(barrel2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->barrel3 = SDL_LoadBMP("./barrel3.bmp");
	if (element->barrel3 == NULL) {
		printf("SDL_LoadBMP(barrel3.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->barrel4 = SDL_LoadBMP("./barrel4.bmp");
	if (element->barrel4 == NULL) {
		printf("SDL_LoadBMP(barrel4.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->explosion = SDL_LoadBMP("./explosion.bmp");
	if (element->explosion == NULL) {
		printf("SDL_LoadBMP(explosion.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->antagonist = SDL_LoadBMP("./antagonist.bmp");
	if (element->antagonist == NULL) {
		printf("SDL_LoadBMP(antagonist.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->antagonist2 = SDL_LoadBMP("./antagonist2.bmp");
	if (element->antagonist2 == NULL) {
		printf("SDL_LoadBMP(antagonist2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->sukuna1 = SDL_LoadBMP("./sukuna1.bmp");
	if (element->sukuna1 == NULL) {
		printf("SDL_LoadBMP(sukuna1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->sukuna2 = SDL_LoadBMP("./sukuna2.bmp");
	if (element->sukuna2 == NULL) {
		printf("SDL_LoadBMP(sukuna2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	element->sukuna3 = SDL_LoadBMP("./sukuna3.bmp");
	if (element->sukuna3 == NULL) {
		printf("SDL_LoadBMP(sukuna3.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	/*mainchar->activeImage = SDL_LoadBMP(filename);
	if (mainchar->activeImage == NULL) {
		printf("SDL_LoadBMP(filename) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};*/
}

int initializeSDL(struct Player* mainchar, struct Elements* element, struct Colors* colors, SDL_Event* event, SDL_Surface** screen, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer) {
	int rc;

	//initialize sdl
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe³noekranowy / fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, window, renderer);

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, window, renderer);

	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(*renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(*window, "Szablon do zdania drugiego 2017");

	(*screen) = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	(*scrtex) = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	loadAllImages(*window, *renderer, *screen, *scrtex, mainchar, element);

	//initialize text
	
	//initialize colors for map
	colors->black = SDL_MapRGB((*screen)->format, 0x00, 0x00, 0x00);
	colors->white = SDL_MapRGB((*screen)->format, 0xFF, 0xFF, 0xFF);
	colors->green = SDL_MapRGB((*screen)->format, 0x00, 0xFF, 0x00);
	colors->red = SDL_MapRGB((*screen)->format, 0xFF, 0x00, 0x00);
	colors->blue = SDL_MapRGB((*screen)->format, 0x11, 0x11, 0xCC);
	colors->purple = SDL_MapRGB((*screen)->format, 0x77, 0x11, 0xCC);
	colors->yellow = SDL_MapRGB((*screen)->format, 0xFF, 0xFF, 0x00);
	colors->cyan = SDL_MapRGB((*screen)->format, 0x00, 0xFF, 0xFF);
	colors->light_purple = SDL_MapRGB((*screen)->format, 0xB4, 0xAC, 0xFF);
	colors->morro = SDL_MapRGB((*screen)->format, 0x33, 0x33, 0x00);
	colors->grey = SDL_MapRGB((*screen)->format, 0x55, 0x55, 0x55);
}

void printAllVisuals(SDL_Surface* screen, struct Colors* colors, struct KeyboardInstructions* instructions, struct Elements* element, struct Player* mainchar, struct GameState* gameState, struct Barrel barrels[MAX_BARRELS]) {
	SDL_FillRect(screen, NULL, colors->black);

	drawLevelOne(screen, colors->green, colors->blue, colors->red, colors->morro, colors->grey, colors->light_purple);

	gameState->fpsTimer += gameState->delta;
	if (gameState->fpsTimer > 0.5) {
		gameState->fps = gameState->frames * 2;
		gameState->frames = 0;
		gameState->fpsTimer -= 0.5;
	};

	DrawLine(screen, 0, 0, SCREEN_WIDTH, 1, 0, colors->grey);
	DrawLine(screen, 0, 0, SCREEN_HEIGHT, 0, 1, colors->grey);
	DrawLine(screen, SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT, 0, 1, colors->grey);
	DrawLine(screen, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, 1, 0, colors->grey);

	DrawRectangle(screen, 3, 3, SCREEN_WIDTH - 6, 51, colors->black, colors->grey);
	
	sprintf(colors->text, "RUNNING TIME = %.1lf s  %.0lf frames / s", instructions->worldTime, gameState->fps);
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 10, colors->text, element->charset);

	sprintf(colors->text, "Esc - EXIT PROGRAM, n - NEW GAME");
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 26, colors->text, element->charset);

	//sprintf(colors->text, "W - CLIMB LADDER, A - GO LEFT, S - DESCEND LADDER,  D - GO RIGHT");
	//DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 42, colors->text, element->charset);

	//sprintf(colors->text, "IMPLEMENTED ELEMENTS: 1,2,3,4,C; MC is alive: %d", mainchar->isAlive);
	//DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 58, colors->text, element->charset);


	//sprintf(colors->text, "barrelTimeTracker %f", gameState->barrelTimeTracker);
	//DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 42, colors->text, element->charset);
	
	
	sprintf(colors->text, "barrelX %f, barrelY %f", barrels[0].x, barrels[0].y);
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 42, colors->text, element->charset);

	
	sprintf(colors->text, "mcX: %f, mcY: %f", mainchar->mcX, mainchar->mcY);
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 58, colors->text, element->charset);

}

/*
void falling() {
	if (gameState->timerOne == 0) {
		mainchar->mcY += MC_FALLING_SPEED;
	}
	else if (gameState->timerOne == 1) {
		gameState->timerOne = 0;
	}
}
*/
void fallingOffPlatform(struct Player* mainchar, struct GameState* gameState) {
	if (mainchar->mcX > 0 && mainchar->mcX < SCREEN_WIDTH - PLATFORM_LENGHT && mainchar->mcY == 311) {
		mainchar->mcY = 425;
	}

	if (mainchar->mcX > PLATFORM_LENGHT && mainchar->mcX < SCREEN_WIDTH && mainchar->mcY == 199) {
		mainchar->mcY = 312;
	}

	if (mainchar->mcX > 0 && mainchar->mcX < SCREEN_WIDTH - PLATFORM_LENGHT && mainchar->mcY == 87) {
		mainchar->mcY = 199;
	}
}

void updateRenderer(SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer) {
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void closingGame(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, struct Elements* element) {
	SDL_FreeSurface(element->charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void ifVictory(struct Player* mainchar, struct KeyboardInstructions* instructions, SDL_Surface* screen, struct GameState* gameState, struct Colors* colors, struct Elements* element) {
	if (mainchar->mcX == BARREL_SPAWN_X + BARREL_WIDTH && mainchar->mcY <= BARREL_SPAWN_Y + 20) {
		instructions->quit = 1;
		SDL_FillRect(screen, NULL, colors->black);
		sprintf(colors->text, "VICTORY!!!!!!!!!!!");
		DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 10, colors->text, element->charset);
	}
}

void printLadderCords() {
	printf("  X   Y  W   H\n");
	printf("%d %d %d %f\n", LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, LADDER_WIDTH, LADDER_HEIGHT);
	printf("%d %d %d %f\n", LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, LADDER_WIDTH, LADDER_HEIGHT);
	printf("%d %d %d %f\n", LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, LADDER_WIDTH, LADDER_HEIGHT);
	printf("%d %d %d %f\n", LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, LADDER_WIDTH, LADDER_HEIGHT);
	printf("%d %d %d %f\n", LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, LADDER_WIDTH, LADDER_HEIGHT);
	printf("%d %d %d %f\n", LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, LADDER_WIDTH, LADDER_HEIGHT);
}

void mcMovement(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS], SDL_Surface* screen, struct GameState* gameState) {
	while (SDL_PollEvent(&(instructions->event))) {
		switch (instructions->event.type) {
		case SDL_QUIT:
			instructions->quit = 1;
			break;
		case SDL_KEYDOWN:
			instructions->keys[instructions->event.key.keysym.scancode] = true;
			switch (instructions->event.key.keysym.sym) {
			case SDLK_ESCAPE:
				instructions->quit = 1;
				break;
			case SDLK_q:
				instructions->quit = 1;
				break;
			case SDLK_n:
				instructions->worldTime = 0;
				mainchar->mcX = MC_SPAWN_X;
				mainchar->mcY = MC_SPAWN_Y; 
				mainchar->isJumping = false;
				mainchar->isJumpingLeft = false;
				mainchar->isJumpingRight = false;
				for (int i = 0; i < MAX_BARRELS; ++i) {
					if (barrels[i].isAlive) {
						barrels[i].isAlive = false;
						barrels[i].x = BARREL_SPAWN_X+1;
						barrels[i].y = BARREL_SPAWN_Y+1;
					}
				}
				mainchar->isAlive = 1;
				break;
			case SDLK_1:
			case SDLK_2:
			case SDLK_3:
				instructions->worldTime = 0;
				mainchar->mcX = MC_SPAWN_X;
				mainchar->mcY = MC_SPAWN_Y;
				for (int i = 0; i < MAX_BARRELS; ++i) {
					barrels[i].x = BARREL_SPAWN_X;
					barrels[i].y = BARREL_SPAWN_Y;
					barrels[i].isAlive = false;
				}
				break;
			}
			break;
		case SDL_KEYUP:
			instructions->keys[instructions->event.key.keysym.scancode] = false;
			break;
		}
	}

}



void updateCharacterMovement(struct Hitbox ladders[6], SDL_Surface* screen, struct Player* mainchar, struct KeyboardInstructions* instructions, struct GameState* gameState, struct Hitbox* ladderHitbox) {
	double jumpVelocity = JUMP_VELOCITY;
	double gravity = GRAVITY;
	ladders[0] = { LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[1] = { LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[2] = { LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[3] = { LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[4] = { LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[5] = { LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, LADDER_WIDTH, LADDER_HEIGHT };

	for (int i = 0; i < 6; ++i) {
		if (checkCollisionWithLadder(mainchar->mcX, mainchar->mcY, MC_WIDTH, MC_HEIGHT, &ladders[i])) {
			instructions->onLadder = 1;
			ladderHitbox = &ladders[i];
			break;
		}
	}

	// climbing ladder
	if (instructions->keys[SDL_SCANCODE_W] && mainchar->mcY > ladderHitbox->y - MC_HEIGHT / 2 && instructions->onLadder && mainchar->isAlive == 1) {
		mainchar->isClimbing = true;
		if (mainchar->mcY - LADDER_HEIGHT / LADDER_CLIMB_SPEED > ladderHitbox->y - MC_HEIGHT / 2) {
			mainchar->mcY -= LADDER_HEIGHT / LADDER_CLIMB_SPEED;
		}
		else {
			double diff = mainchar->mcY - ladderHitbox->y + MC_HEIGHT / 2;
			mainchar->mcY -= diff;
		}
	}
	else {
		mainchar->isClimbing = false;
	}

	// moving left
	if (instructions->keys[SDL_SCANCODE_A] && mainchar->isAlive == 1 && (mainchar->mcY == 425 || mainchar->mcY == 424 || mainchar->mcY == 311 || mainchar->mcY == 199 || mainchar->mcY == 87 || mainchar->mcY == 312 || mainchar->mcY == 200)) {
		if (mainchar->mcX - (MC_WIDTH / 2) > 0) {
			mainchar->mcX -= MC_SPEED;
		}
		mainchar->movingLeft = true;
	}
	else {
		mainchar->movingLeft = false;
	}

	// sliding down ladder
	if (instructions->keys[SDL_SCANCODE_S] && mainchar->mcY < ladderHitbox->y + ladderHitbox->height - MC_HEIGHT / 2 && instructions->onLadder && mainchar->isAlive == 1) {
		mainchar->isDescending = true;
		if (mainchar->mcY + LADDER_HEIGHT / LADDER_SLIDE_SPEED < ladderHitbox->y + ladderHitbox->height - MC_HEIGHT / 2) {
			mainchar->mcY += LADDER_HEIGHT / LADDER_SLIDE_SPEED;
		}
		else {
			double diff = LADDER_HEIGHT - mainchar->mcY + ladderHitbox->y - MC_HEIGHT / 2;
			mainchar->mcY += diff;
			/*
			if (mainchar->mcY == 424) {
				mainchar->mcY = 425;
			}
			else if (mainchar->mcY == 312) {
				mainchar->mcY = 311;
			}
			else if (mainchar->mcY == 311.565) {
				mainchar->mcY = 311;
			}
			else if (mainchar->mcY == 199.565) {
				mainchar->mcY = 199;
			}
			else if (mainchar->mcY == 200) {
				mainchar->mcY = 199;
			}
			*/
		}
	}
	else {
		mainchar->isDescending = false;
	}

	// moving right
	if (instructions->keys[SDL_SCANCODE_D] && mainchar->isAlive == 1 && (mainchar->mcY == 425 || mainchar->mcY == 424 || mainchar->mcY == 311 || mainchar->mcY == 199 || mainchar->mcY == 87 || mainchar->mcY == 312 || mainchar->mcY == 200)) {
		if (mainchar->mcX < SCREEN_WIDTH - (MC_WIDTH / 2)) {
			mainchar->mcX += MC_SPEED;
		}
		mainchar->movingRight = true;
	}
	else {
		mainchar->movingRight = false;
	}


	// jumping horizontally
	//if (instructions->keys[SDL_SCANCODE_SPACE] && mainchar->isAlive == 1) {
		//mainchar->isJumping = true;
	//}

	// jumping diagonally left
	if (instructions->keys[SDL_SCANCODE_SPACE] && instructions->keys[SDL_SCANCODE_A] && mainchar->isAlive == 1) {
		mainchar->isJumpingLeft = true;
	}

	// jumping diagonally right
	if (instructions->keys[SDL_SCANCODE_SPACE] && instructions->keys[SDL_SCANCODE_D] && mainchar->isAlive == 1) {
		mainchar->isJumpingRight = true;
	}

	if (mainchar->isJumpingLeft) {
		mainchar->isJumpingRight = false;
	}
	if (mainchar->isJumpingRight) {
		mainchar->isJumpingLeft = false;
	}

	if (instructions->keys[SDL_SCANCODE_SPACE] && instructions->keys[SDL_SCANCODE_D] && instructions->keys[SDL_SCANCODE_A] && mainchar->isAlive == 1) {
		if (mainchar->mcY <= (425 + 10) && mainchar->mcY > (311 + 10)) {
			mainchar->mcY = 425;
		}
		if (mainchar->mcY <= (311 + 10) && mainchar->mcY > (199 + 10)) {
			mainchar->mcY = 311;
		}
		if (mainchar->mcY <= (199 + 10) && mainchar->mcY > (87 + 10)) {
			mainchar->mcY = 199;
		}
		if (mainchar->mcY <= (87 + 10) && mainchar->mcY > (-27 + 10)) {
			mainchar->mcY = 87;
		}
		gameState->jumpTimerLeft = 0.0;
		gameState->jumpTimerRight = 0.0;
		mainchar->isJumpingRight = false;
		mainchar->isJumpingLeft = false;
	}
}

/*
void jumping(struct Player* mainchar, struct GameState* gameState) {
	if (mainchar->isJumping == true) {
		//int count = gameState->jumpTimer / gameState->delta;
		// logic for going up
		if (gameState->jumpTimer <= 0.1 && gameState->jumpTimer > 0.0) { // od 0 do 0.1
			mainchar->mcY -= JUMP_SPEED;
			mainchar->mcX += JUMP_SPEED;
		}
		if (gameState->jumpTimer <= 0.2 && gameState->jumpTimer > 0.1) {
			mainchar->mcY -= JUMP_SPEED;
			mainchar->mcX += JUMP_SPEED;
		}
		if (gameState->jumpTimer <= 1 && gameState->jumpTimer > 0.9) {
			mainchar->mcY += JUMP_SPEED;
			mainchar->mcX += JUMP_SPEED;
		}
		if (gameState->jumpTimer <= 1.1 && gameState->jumpTimer > 1) {
			mainchar->mcY += JUMP_SPEED;
			mainchar->mcX += JUMP_SPEED;
			if (mainchar->mcY <= 425 && mainchar->mcY > 311) {
				mainchar->mcY = 425;
			}
			if (mainchar->mcY <= 311 && mainchar->mcY > 199) {
				mainchar->mcY = 311;
			}
			if (mainchar->mcY <= 199 && mainchar->mcY > 87) {
				mainchar->mcY = 199;
			}
			if (mainchar->mcY <= 87 && mainchar->mcY > -25) {
				mainchar->mcY = 87;
			}
			gameState->jumpTimer = 0.0;
			mainchar->isJumping = false;
		}
	}
}
*/

void jumpingLeft(struct Player* mainchar, struct GameState* gameState) {
	// JUMPING LEFT INSIDE BORDER
	if (mainchar->isJumpingLeft == true && mainchar->mcX - (MC_WIDTH / 2) > 0) {
		//int count = gameState->jumpTimerLeft / gameState->delta;
		// logic for going up
		if (gameState->jumpTimerLeft <= 0.15 && gameState->jumpTimerLeft > 0.0) { // od 0 do 0.1
			if (mainchar->mcX >= SCREEN_WIDTH - PLATFORM_ONE_X && mainchar->mcX < SCREEN_WIDTH && mainchar->mcY <= 311 && mainchar->mcY > 87) {
				mainchar->mcY -= JUMP_SPEED * 3;
			}
			else {
				mainchar->mcY -= JUMP_SPEED;
			}
			mainchar->mcX -= JUMP_SPEED;
		}
		if (gameState->jumpTimerLeft <= 0.3 && gameState->jumpTimerLeft > 0.15) {
			if (mainchar->mcX >= SCREEN_WIDTH - PLATFORM_ONE_X && mainchar->mcX < SCREEN_WIDTH && mainchar->mcY <= 311 && mainchar->mcY > 87) {
				mainchar->mcY -= JUMP_SPEED * 3;
			}
			else {
				mainchar->mcY -= JUMP_SPEED;
			}
			mainchar->mcX -= JUMP_SPEED;
		}
		if (gameState->jumpTimerLeft <= 0.45 && gameState->jumpTimerLeft > 0.3) {
			if (mainchar->mcX >= SCREEN_WIDTH - PLATFORM_ONE_X && mainchar->mcX < SCREEN_WIDTH && mainchar->mcY <= 311 && mainchar->mcY > 87) {
				mainchar->mcY += JUMP_SPEED;
			}
			else {
				mainchar->mcY += JUMP_SPEED;
			}
			mainchar->mcX -= JUMP_SPEED * 2;
		}
		if (gameState->jumpTimerLeft <= 0.6 && gameState->jumpTimerLeft > 0.45) {
			if (mainchar->mcX >= SCREEN_WIDTH - PLATFORM_ONE_X && mainchar->mcX < SCREEN_WIDTH && mainchar->mcY <= 311 && mainchar->mcY > 87) {
				//nothing
			}
			else {
				mainchar->mcY += JUMP_SPEED;
			}
			mainchar->mcX -= JUMP_SPEED * 2;
		}
		if (gameState->jumpTimerLeft > 0.6) {
			if (mainchar->mcY <= (425 + 10) && mainchar->mcY > (311 + 10)) {
				mainchar->mcY = 425;
			}
			if (mainchar->mcY <= (311 + 10) && mainchar->mcY > (199 + 10)) {
				mainchar->mcY = 311;
			}
			if (mainchar->mcY <= (199 + 10) && mainchar->mcY > (87 + 10)) {
				mainchar->mcY = 199;
			}
			if (mainchar->mcY <= (87 + 10) && mainchar->mcY > (-27 + 10)) {
				mainchar->mcY = 87;
			}
			gameState->jumpTimerLeft = 0.0;
			mainchar->isJumpingLeft = false;
		}
	}
	// JUMPING LEFT OUTSIDE BORDER
	else if (mainchar->isJumpingLeft == true && mainchar->mcX - (MC_WIDTH / 2) - JUMP_SPEED < 0) {
		if (gameState->jumpTimerLeft <= 0.45 && gameState->jumpTimerLeft > 0.3 && mainchar->mcY < 425 - JUMP_SPEED / 2 && mainchar->mcY > 311 || mainchar->mcY < 311 - JUMP_SPEED / 2 && mainchar->mcY > 199 || mainchar->mcY < 199 - JUMP_SPEED / 2 && mainchar->mcY > 87 || mainchar->mcY < 87 - JUMP_SPEED / 2 && mainchar->mcY > -27) {
			mainchar->mcY += JUMP_SPEED / 2;
		}
		if (gameState->jumpTimerLeft <= 0.6 && gameState->jumpTimerLeft > 0.45 && mainchar->mcY < 425 - JUMP_SPEED / 2 && mainchar->mcY > 311 || mainchar->mcY < 311 - JUMP_SPEED / 2 && mainchar->mcY > 199 || mainchar->mcY < 199 - JUMP_SPEED / 2 && mainchar->mcY > 87 || mainchar->mcY < 87 - JUMP_SPEED / 2 && mainchar->mcY > -27) {
			mainchar->mcY += JUMP_SPEED / 2;
		}
		if (gameState->jumpTimerLeft > 0.6) {
			if (mainchar->mcY <= (425 + 10) && mainchar->mcY > (311 + 10)) {
				mainchar->mcY = 425;
			}
			if (mainchar->mcY <= (311 + 10) && mainchar->mcY > (199 + 10)) {
				mainchar->mcY = 311;
			}
			if (mainchar->mcY <= (199 + 10) && mainchar->mcY > (87 + 10)) {
				mainchar->mcY = 199;
			}
			if (mainchar->mcY <= (87 + 10) && mainchar->mcY > (-27 + 10)) {
				mainchar->mcY = 87;
			}
			gameState->jumpTimerLeft = 0.0;
			mainchar->isJumpingLeft = false;
		}
	}
}

void jumpingRight(struct Player* mainchar, struct GameState* gameState) {
	// JUMPING RIGHT INSIDE BORDER
	if (mainchar->isJumpingRight == true && mainchar->mcX < SCREEN_WIDTH - (MC_WIDTH / 2)) {
		//GOING UP
		if (gameState->jumpTimerRight <= 0.15 && gameState->jumpTimerRight > 0.0) {
			if (mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X && mainchar->mcY <= 425 && mainchar->mcY > 199 || mainchar->mcY <= 199 && mainchar->mcY > -25 && mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X) {
				mainchar->mcY -= JUMP_SPEED * 3;
			}
			else {
				mainchar->mcY -= JUMP_SPEED;
			}
			mainchar->mcX += JUMP_SPEED;
		}
		if (gameState->jumpTimerRight <= 0.3 && gameState->jumpTimerRight > 0.15) {
			if (mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X && mainchar->mcY <= 425 && mainchar->mcY > 199 || mainchar->mcY <= 199 && mainchar->mcY > -25 && mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X) {
				mainchar->mcY -= JUMP_SPEED * 3;
			}
			else {
				mainchar->mcY -= JUMP_SPEED;
			}
			mainchar->mcX += JUMP_SPEED;
		}

		//GOING DOWN
		if (gameState->jumpTimerRight <= 0.45 && gameState->jumpTimerRight > 0.3) {
			if (mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X && mainchar->mcY <= 425 && mainchar->mcY > 199 || mainchar->mcY <= 199 && mainchar->mcY > -25 && mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X) {
				mainchar->mcY += JUMP_SPEED;
			}
			else {
				mainchar->mcY += JUMP_SPEED;
			}
			mainchar->mcX += JUMP_SPEED * 2;
		}
		if (gameState->jumpTimerRight <= 0.6 && gameState->jumpTimerRight > 0.45) {
			if (mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X && mainchar->mcY <= 425 && mainchar->mcY > 199 || mainchar->mcY <= 199 && mainchar->mcY > -25 && mainchar->mcX >= 0 && mainchar->mcX < PLATFORM_ONE_X) {
				// nothing
			}
			else {
				mainchar->mcY += JUMP_SPEED;
			}
			mainchar->mcX += JUMP_SPEED * 2;
		}

		// RESET ON GROUND
		if (gameState->jumpTimerRight > 0.6) {
			if (mainchar->mcY <= (425 + 10) && mainchar->mcY > (311 + 10)) {
				mainchar->mcY = 425;
			}
			if (mainchar->mcY <= (311 + 10) && mainchar->mcY > (199 + 10)) {
				mainchar->mcY = 311;
			}
			if (mainchar->mcY <= (199 + 10) && mainchar->mcY > (87 + 10)) {
				mainchar->mcY = 199;
			}
			if (mainchar->mcY <= (87 + 10) && mainchar->mcY > (-27 + 10)) {
				mainchar->mcY = 87;
			}
			gameState->jumpTimerRight = 0.0;
			mainchar->isJumpingRight = false;
		}
	}

	// JUMPING RIGHT OUTSIDE BORDER
	else if (mainchar->isJumpingRight == true && mainchar->mcX + JUMP_SPEED > SCREEN_WIDTH - (MC_WIDTH / 2)) {
		// SLIDE DOWN WALL
		if (gameState->jumpTimerRight <= 0.45 && gameState->jumpTimerRight > 0.3 && mainchar->mcY < 425 - JUMP_SPEED / 2 && mainchar->mcY > 311 || mainchar->mcY < 311 - JUMP_SPEED / 2 && mainchar->mcY > 199 || mainchar->mcY < 199 - JUMP_SPEED / 2 && mainchar->mcY > 87 || mainchar->mcY < 87 - JUMP_SPEED / 2 && mainchar->mcY > -27) {
			mainchar->mcY += JUMP_SPEED / 2;
		}
		if (gameState->jumpTimerRight <= 0.6 && gameState->jumpTimerRight > 0.45 && mainchar->mcY < 425 - JUMP_SPEED / 2 && mainchar->mcY > 311 || mainchar->mcY < 311 - JUMP_SPEED / 2 && mainchar->mcY > 199 || mainchar->mcY < 199 - JUMP_SPEED / 2 && mainchar->mcY > 87 || mainchar->mcY < 87 - JUMP_SPEED / 2 && mainchar->mcY > -27) {
			mainchar->mcY += JUMP_SPEED / 2;
		}

		// RESET ON GROUND 
		if (gameState->jumpTimerRight > 0.6) {
			if (mainchar->mcY <= (425 + 10) && mainchar->mcY > (311 + 10)) {
				mainchar->mcY = 425;
			}
			if (mainchar->mcY <= (311 + 10) && mainchar->mcY > (199 + 10)) {
				mainchar->mcY = 311;
			}
			if (mainchar->mcY <= (199 + 10) && mainchar->mcY > (87 + 10)) {
				mainchar->mcY = 199;
			}
			if (mainchar->mcY <= (87 + 10) && mainchar->mcY > (-27 + 10)) {
				mainchar->mcY = 87;
			}
			gameState->jumpTimerRight = 0.0;
			mainchar->isJumpingRight = false;
		}
	}
}


#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	struct KeyboardInstructions instructions;
	struct Player mainchar;
	struct Elements element;
	struct Barrel barrel;
	struct Hitbox ladderHitbox;
	struct Barrel barrels[MAX_BARRELS];
	struct Hitbox ladders[6];
	struct Colors colors;
	struct GameState gameState;

	SDL_Event event{};
	SDL_Surface* screen;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;

	initializeSDL(&mainchar, &element, &colors, &event, &screen, &scrtex, &window, &renderer);
	int t1, t2;
	double distance, etiSpeed;

	t1 = SDL_GetTicks();
	gameState.frames = 0;
	gameState.fpsTimer = 0;
	gameState.fps = 0;
	instructions.quit = 0;
	instructions.worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	mainchar.mcX = MC_SPAWN_X;
	mainchar.mcY = MC_SPAWN_Y;
	barrel.x = BARREL_SPAWN_X;
	barrel.y = BARREL_SPAWN_Y;
	mainchar.isAlive = 1;
	mainchar.isJumping = 0;
	mainchar.isJumpingLeft = 0;
	mainchar.isJumpingRight = 0;
	gameState.epsilon = SENSITIVITY_RADIUS;
	gameState.timeSinceLastSpawn = 0.0;
	gameState.timeTracker = 0.0;
	gameState.barrelTimeTracker = 0.0;
	gameState.timerOne = 0.0;
	gameState.jumpTimer = 0.0;
	gameState.jumpTimerLeft = 0.0;
	gameState.jumpTimerRight = 0.0;
	mainchar.falling = false;
	for (int i = 0; i < MAX_BARRELS; ++i) {
		barrels[i].isAlive = false;
	}
	
	printLadderCords();

	while (!instructions.quit) {
		t2 = SDL_GetTicks();
		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna³ od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		gameState.delta = (t2 - t1) * 0.001;
		t1 = t2;

		if (mainchar.isAlive) {
			instructions.worldTime += gameState.delta;
		}

		distance += etiSpeed * gameState.delta;

		gameState.timeSinceLastSpawn += gameState.delta;
		gameState.timeTracker += gameState.delta;
		gameState.barrelTimeTracker += gameState.delta;
		gameState.timerOne += gameState.delta;
		if (mainchar.isJumping == true) {
			gameState.jumpTimer += gameState.delta;
		}
		if (mainchar.isJumpingLeft == true) {
			gameState.jumpTimerLeft += gameState.delta;
		}
		if (mainchar.isJumpingRight == true) {
			gameState.jumpTimerRight += gameState.delta;
		}

		instructions.onLadder = 0;

		// logic for when mc falls off the edge of a platform
		fallingOffPlatform(&mainchar, &gameState);

		//jumping(&mainchar, &gameState);
		jumpingLeft(&mainchar, &gameState);
		jumpingRight(&mainchar, &gameState);

		//print background
		printAllVisuals(screen, &colors, &instructions, &element, &mainchar, &gameState, barrels);

		//spawn barrels
		spawnBarrels(&mainchar, screen, barrels, &element, &gameState, &barrel);

		//controls for when mc is alive
		mcMovement(&mainchar, &instructions, barrels, screen, &gameState);

		void mcMovement(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS], SDL_Surface * screen, struct GameState* gameState);

		updateCharacterMovement(ladders, screen, &mainchar, &instructions, &gameState, &ladderHitbox);

		animations(screen, &mainchar, &instructions, &gameState, &element, &barrel);

		ifVictory(&mainchar, &instructions, screen, &gameState, &colors, &element);

		updateRenderer(scrtex, screen, renderer);

		gameState.frames++;
	}
	closingGame(window, renderer, screen, scrtex, &element);
	return 0;
	};
