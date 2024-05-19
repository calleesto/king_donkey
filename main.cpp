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
#define PLATFORM_HEIGHT			30
#define LADDER_WIDTH			50
#define LADDER_HEIGHT			113
#define GROUND_HEIGHT			30

//main character position speed and dimensions
#define CHARACTER_WIDTH			50
#define CHARACTER_HEIGHT		50
#define MC_SPAWN_X				SCREEN_WIDTH - 100
#define MC_SPAWN_Y				SCREEN_HEIGHT - 55
#define MC_SPEED				10

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
#define BARREL_SPEED			100
#define BARREL_SPAWN_X			SCREEN_WIDTH - 100
#define BARREL_SPAWN_Y			PLATFORM_ROW_THREE_Y - BARREL_HEIGHT
#define SENSITIVITY_RADIUS		40
#define BARREL_SPAWN_INTERVAL	3
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
void drawLevelOne(SDL_Surface* screen, int green, int blue, int red);
void animations(SDL_Surface* screen, struct Player* mainchar);
void UpdateBarrel(struct Barrel* barrel, struct Player* mainchar, SDL_Surface* explosion, SDL_Surface* screen, SDL_Surface* barrelbmp, struct GameState* gameState);
void SpawnBarrel(struct Barrel* barrel);
int loadAllImages(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, struct Player* mainchar, struct Elements* element);
int initializeSDL(struct Player* mainchar, struct Elements* element, struct Colors* colors, SDL_Event* event, SDL_Surface** screen, SDL_Texture** scrtex, SDL_Window** window, SDL_Renderer** renderer);
void printAllVisuals(SDL_Surface* screen, struct Colors* colors, struct KeyboardInstructions* instructions, struct Elements* element, struct Player* mainchar, struct GameState* gameState);
void spawnBarrels(struct Player* mainchar, SDL_Surface* screen, struct Barrel barrels[MAX_BARRELS], struct Elements* element, struct GameState* gameState);
void mcAliveControls(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS], SDL_Surface* screen, struct GameState* gameState);
void mcDeadControls(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS]);
void updateRenderer(SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer);
void fallingOffPlatform(struct Player* mainchar);
void updateRenderer(SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer);
void ladderMovement(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Hitbox* ladderHitbox, struct Hitbox ladders[6]);
void gameLoop(struct Hitbox ladders[], struct KeyboardInstructions* instructions, struct Player* mainchar, struct Barrel barrel, struct Barrel barrels[], SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer, struct GameState* gameState, struct Elements* element, struct Colors* colors, struct Hitbox* ladderHitbox, SDL_Event event, SDL_Window* window);
void closingGame(SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* screen, SDL_Texture* scrtex, struct Elements* element);
void ifVictory(struct Player* mainchar, struct KeyboardInstructions* instructions, SDL_Surface* screen, struct GameState* gameState, struct Colors* colors, struct Elements* element);

struct KeyboardInstructions {
	SDL_Event event;
	int quit;
	double worldTime;
	int onLadder;
};

struct Hitbox {
	int x;
	int y;
	int width;
	int height;
};

struct Barrel {
	double x;
	double y;
	bool isAlive;
};

//main character surfaces as well as state checkers
struct Player {
	int mcX;
	int mcY;
	SDL_Surface* mc;
	SDL_Surface* mc_jump;
	SDL_Surface* mc_left;
	SDL_Surface* mc_right;
	SDL_Surface* mc_dead;
	SDL_Surface* activeImage;
	bool isAlive;
	bool isJumping;
	bool movingRight;
	bool movingLeft;
	bool isClimbing;
	bool isDescending;
};

//the rest of the surface elements like ladder platform etc
struct Elements {
	SDL_Surface* charset;
	SDL_Surface* barrelbmp;
	SDL_Surface* explosion;
	SDL_Surface* antagonist;
	SDL_Surface* antagonist2;
	SDL_Surface* eti;
};

struct Colors {
	int black;
	int red; 
	int green;
	int blue;
	char text[128];
};

struct GameState {
	double delta;
	double epsilon;
	double timeSinceLastSpawn;
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
	return (charX < ladderHitbox->x + ladderHitbox->width && charX + charWidth > ladderHitbox->x &&
		charY < ladderHitbox->y + ladderHitbox->height && charY + charHeight > ladderHitbox->y);
}

void drawLevelOne(SDL_Surface* screen, int green, int blue, int red) {
	DrawRectangle(screen, PLATFORM_TWO_X, GROUND_Y, SCREEN_WIDTH, GROUND_HEIGHT, green, green);

	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_ONE_Y, PLATFORM_LENGHT, blue);
	drawPlatform(screen, PLATFORM_TWO_X, PLATFORM_ROW_TWO_Y, PLATFORM_LENGHT, blue);
	drawPlatform(screen, PLATFORM_ONE_X, PLATFORM_ROW_THREE_Y, PLATFORM_LENGHT, blue);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, red);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, red);

	drawLadder(screen, LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, red);
	drawLadder(screen, LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, red);

	drawLadder(screen, LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, red);
	drawLadder(screen, LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, red);
}

void animations(SDL_Surface* screen, struct Player* mainchar) {
	if (mainchar->isAlive == 1) {
		mainchar->activeImage = mainchar->mc;
	}
	else {
		mainchar->activeImage = mainchar->mc_dead;
	}
	if (mainchar->movingRight) {
		mainchar->activeImage=mainchar->mc_right;
	}
	if (mainchar->movingLeft) {
		mainchar->activeImage=mainchar->mc_left;
	}
	if (mainchar->isDescending) {
		mainchar->activeImage = mainchar->mc_jump;
	}
	if (mainchar->isClimbing) {
		mainchar->activeImage = mainchar->mc_jump;
	}
	DrawSurface(screen, mainchar->activeImage, mainchar->mcX, mainchar->mcY);
}

//set path for the barrels and if statement for contact with main character
void UpdateBarrel(struct Barrel* barrel, struct Player* mainchar, SDL_Surface* explosion, SDL_Surface* screen, SDL_Surface* barrelbmp, struct GameState* gameState) {

	if (barrel->x >= 50 && barrel->y == 82) {
		barrel->x -= (BARREL_SPEED * gameState->delta);
		if (mainchar->isAlive == 1) {
			DrawSurface(screen, barrelbmp, barrel->x, barrel->y);
		}
		if (mainchar->isAlive == 0) {
			DrawSurface(screen, explosion, barrel->x, barrel->y);
		}
	}

	if (barrel->x < 50 && barrel->y == BARREL_SPAWN_Y) {
		barrel->y += PLATFORM_ROW_THREE_Y;
	}

	if (barrel->x <= BARREL_SPAWN_X && barrel->y == 194) {
		barrel->x += (BARREL_SPEED * gameState->delta);
		if (mainchar->isAlive == 1) {
			DrawSurface(screen, barrelbmp, barrel->x, barrel->y);
		}
		if (mainchar->isAlive == 0) {
			DrawSurface(screen, explosion, barrel->x, barrel->y);
		}
	}

	if (barrel->x > BARREL_SPAWN_X && barrel->y == 194) {
		barrel->y += PLATFORM_ROW_THREE_Y;
	}

	if (barrel->x >= 50 && barrel->y == 306) {
		barrel->x -= (BARREL_SPEED * gameState->delta);
		if (mainchar->isAlive == 1) {
			DrawSurface(screen, barrelbmp, barrel->x, barrel->y);
		}
		if (mainchar->isAlive == 0) {
			DrawSurface(screen, explosion, barrel->x, barrel->y);
		}
	}

	if (barrel->x < 50 && barrel->y == 306) {
		barrel->y += PLATFORM_ROW_THREE_Y;
	}

	if (barrel->x <= BARREL_SPAWN_X && barrel->y == 418) {
		barrel->x += (BARREL_SPEED * gameState->delta);
		if (mainchar->isAlive == 1) {
			DrawSurface(screen, barrelbmp, barrel->x, barrel->y);
		}
		if (mainchar->isAlive == 0) {
			DrawSurface(screen, explosion, barrel->x, barrel->y);
		}
	}
	if (barrel->x >= BARREL_SPAWN_X && barrel->y == 418) {
		barrel->isAlive = false;
	}

	if (fabs(barrel->x - mainchar->mcX) <= gameState->epsilon && fabs(barrel->y - mainchar->mcY) <= gameState->epsilon) {
		mainchar->isAlive = 0;
		barrel->isAlive = false;
		mainchar->isDescending = false;
		mainchar->isClimbing = false;
		mainchar->movingLeft = false;
		mainchar->movingRight = false;
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

	mainchar->mc_left = SDL_LoadBMP("./gojo_left.bmp");
	if (mainchar->mc == NULL) {
		printf("SDL_LoadBMP(gojo_left.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(element->charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};

	mainchar->mc_right = SDL_LoadBMP("./gojo_right.bmp");
	if (mainchar->mc == NULL) {
		printf("SDL_LoadBMP(gojo_right.bmp) error: %s\n", SDL_GetError());
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

	element->barrelbmp = SDL_LoadBMP("./barrel.bmp");
	if (element->barrelbmp == NULL) {
		printf("SDL_LoadBMP(barrel.bmp) error: %s\n", SDL_GetError());
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

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		window, renderer);
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
	colors->green = SDL_MapRGB((*screen)->format, 0x00, 0xFF, 0x00);
	colors->red = SDL_MapRGB((*screen)->format, 0xFF, 0x00, 0x00);
	colors->blue = SDL_MapRGB((*screen)->format, 0x11, 0x11, 0xCC);
}

void printAllVisuals(SDL_Surface* screen, struct Colors* colors, struct KeyboardInstructions* instructions, struct Elements* element, struct Player* mainchar, struct GameState* gameState) {
	SDL_FillRect(screen, NULL, colors->black);

	drawLevelOne(screen, colors->green, colors->blue, colors->red);

	gameState->fpsTimer += gameState->delta;
	if (gameState->fpsTimer > 0.5) {
		gameState->fps = gameState->frames * 2;
		gameState->frames = 0;
		gameState->fpsTimer -= 0.5;
	};

	DrawLine(screen, 0, 0, SCREEN_WIDTH, 1, 0, colors->green);
	DrawLine(screen, 0, 0, SCREEN_HEIGHT, 0, 1, colors->green);
	DrawLine(screen, SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT, 0, 1, colors->green);
	DrawLine(screen, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, 1, 0, colors->green);

	DrawRectangle(screen, 6, 6, SCREEN_WIDTH - 12, 68, colors->black, colors->red);

	sprintf(colors->text, "RUNNING TIME = %.1lf s  %.0lf frames / s", instructions->worldTime, gameState->fps);
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 10, colors->text, element->charset);

	sprintf(colors->text, "Esc - EXIT PROGRAM, n - NEW GAME");
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 26, colors->text, element->charset);

	sprintf(colors->text, "W - CLIMB LADDER, A - GO LEFT, S - DESCEND LADDER,  D - GO RIGHT");
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 42, colors->text, element->charset);

	sprintf(colors->text, "IMPLEMENTED ELEMENTS: 1,2,3,4,C; MC is alive: %d", mainchar->isAlive);
	DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 58, colors->text, element->charset);

	DrawSurface(screen, element->antagonist2, BARREL_SPAWN_X + BARREL_WIDTH, BARREL_SPAWN_Y);
}

void spawnBarrels(struct Player* mainchar, SDL_Surface* screen, struct Barrel barrels[MAX_BARRELS], struct Elements* element, struct GameState* gameState) {
	if (gameState->timeSinceLastSpawn >= BARREL_SPAWN_INTERVAL && mainchar->isAlive) {
		for (int i = 0; i < MAX_BARRELS; i++) {
			if (!barrels[i].isAlive) {
				DrawSurface(screen, element->antagonist, BARREL_SPAWN_X + BARREL_WIDTH, BARREL_SPAWN_Y);
				SpawnBarrel(&barrels[i]);
				break;
			}
		}
		gameState->timeSinceLastSpawn = 0.0;
	}

	for (int i = 0; i < MAX_BARRELS; i++) {
		UpdateBarrel(&barrels[i], mainchar, element->explosion, screen, element->barrelbmp, gameState);
	}
}

void mcAliveControls(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS], SDL_Surface* screen, struct GameState* gameState) {
	double jumpVelocity = JUMP_VELOCITY;
	double gravity = GRAVITY;
	if (mainchar->isAlive == 1) {
		while (SDL_PollEvent(&(instructions->event))) {
			switch (instructions->event.type) {
			case SDL_QUIT:
				instructions->quit = 1;
				break;
			case SDL_KEYDOWN:
				switch (instructions->event.key.keysym.sym) {
				case SDLK_ESCAPE:
					instructions->quit = 1;
					break;
				case SDLK_n:
					instructions->worldTime = 0;
					mainchar->mcX = MC_SPAWN_X;
					mainchar->mcY = MC_SPAWN_Y;
					for (int i = 0; i < MAX_BARRELS; ++i) {
						barrels[i].x = BARREL_SPAWN_X;
						barrels[i].y = BARREL_SPAWN_Y;
						barrels[i].isAlive = false;
					}
					mainchar->isAlive = 1;
					break;
				case SDLK_1:
					instructions->worldTime = 0;
					mainchar->mcX = MC_SPAWN_X;
					mainchar->mcY = MC_SPAWN_Y;
					for (int i = 0; i < MAX_BARRELS; ++i) {
						barrels[i].x = BARREL_SPAWN_X;
						barrels[i].y = BARREL_SPAWN_Y;
						barrels[i].isAlive = false;
					}

					break;
				case SDLK_2:
					instructions->worldTime = 0;
					mainchar->mcX = MC_SPAWN_X;
					mainchar->mcY = MC_SPAWN_Y;
					for (int i = 0; i < MAX_BARRELS; ++i) {
						barrels[i].x = BARREL_SPAWN_X;
						barrels[i].y = BARREL_SPAWN_Y;
						barrels[i].isAlive = false;
					}

					break;
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
				case SDLK_a:
					if (mainchar->mcX - (CHARACTER_WIDTH / 2) > 0) {
						mainchar->mcX -= MC_SPEED;
					}
					mainchar->movingLeft = true;
					mainchar->movingRight = false;
					mainchar->isClimbing = false;
					mainchar->isDescending = false;

					break;
				case SDLK_d:
					if (mainchar->mcX < SCREEN_WIDTH - (CHARACTER_WIDTH / 2)) {
						mainchar->mcX += MC_SPEED;
					}
					mainchar->movingRight = true;
					mainchar->movingLeft = false;
					mainchar->isClimbing = false;
					mainchar->isDescending = false;

					break;
				case SDLK_SPACE:
					mainchar->isJumping = true;
					if (mainchar->mcY == MC_ON_GROUND_Y) {
						while (jumpVelocity < 0 || mainchar->mcY <= MC_ON_GROUND_Y) {
							mainchar->mcY += jumpVelocity * gameState->delta + 0.5 * gravity * gameState->delta * gameState->delta;

							jumpVelocity += gravity * gameState->delta;



						}

						mainchar->mcY = MC_ON_GROUND_Y;
						jumpVelocity = JUMP_VELOCITY;
					}
					if (mainchar->mcY == MC_ON_PLATFORM_ONE_Y) {
						while (jumpVelocity < 0 || mainchar->mcY <= MC_ON_PLATFORM_ONE_Y) {
							mainchar->mcY += jumpVelocity * gameState->delta + 0.5 * gravity * gameState->delta * gameState->delta;


							jumpVelocity += gravity * gameState->delta;


							DrawSurface(screen, mainchar->mc_jump, mainchar->mcX, mainchar->mcY);
						}
						mainchar->mcY = MC_ON_PLATFORM_ONE_Y;
						jumpVelocity = JUMP_VELOCITY;
					}
					if (mainchar->mcY == MC_ON_PLATFORM_TWO_Y) {
						while (jumpVelocity < 0 || mainchar->mcY <= MC_ON_PLATFORM_TWO_Y) {
							mainchar->mcY += jumpVelocity * gameState->delta + 0.5 * GRAVITY * gameState->delta * gameState->delta;

							jumpVelocity += gravity * gameState->delta;


							DrawSurface(screen, mainchar->mc_jump, mainchar->mcX, mainchar->mcY);
						}
						mainchar->mcY = MC_ON_PLATFORM_TWO_Y;
						jumpVelocity = JUMP_VELOCITY;
					}
					if (mainchar->mcY == MC_ON_PLATFORM_THREE_Y) {
						while (jumpVelocity < 0 || mainchar->mcY <= MC_ON_PLATFORM_THREE_Y) {
							mainchar->mcY += jumpVelocity * gameState->delta + 0.5 * GRAVITY * gameState->delta * gameState->delta;


							jumpVelocity += gravity * gameState->delta;


							DrawSurface(screen, mainchar->mc_jump, mainchar->mcX, mainchar->mcY);
						}
						mainchar->mcY = MC_ON_PLATFORM_THREE_Y;
						jumpVelocity = JUMP_VELOCITY;
					}
					break;
				}
			}
		}
	}
}

void mcDeadControls(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Barrel barrels[MAX_BARRELS]) {
	if (mainchar->isAlive == 0) {
		while (SDL_PollEvent(&(instructions->event))) {
			switch (instructions->event.type) {
			case SDL_QUIT:
				instructions->quit = 1;
				break;
			case SDL_KEYDOWN:
				switch (instructions->event.key.keysym.sym) {
				case SDLK_ESCAPE:
					instructions->quit = 1;
					break;
				case SDLK_n:
					instructions->worldTime = 0;
					mainchar->mcX = MC_SPAWN_X;
					mainchar->mcY = MC_SPAWN_Y;
					for (int i = 0; i < MAX_BARRELS; ++i) {
						barrels[i].x = BARREL_SPAWN_X;
						barrels[i].y = BARREL_SPAWN_Y;
						barrels[i].isAlive = false;
					}
					mainchar->isAlive = 1;
					break;
				case SDLK_1:
					instructions->worldTime = 0;
					mainchar->mcX = MC_SPAWN_X;
					mainchar->mcY = MC_SPAWN_Y;
					for (int i = 0; i < MAX_BARRELS; ++i) {
						barrels[i].x = BARREL_SPAWN_X;
						barrels[i].y = BARREL_SPAWN_Y;
						barrels[i].isAlive = false;
					}

					break;
				case SDLK_2:
					instructions->worldTime = 0;
					mainchar->mcX = MC_SPAWN_X;
					mainchar->mcY = MC_SPAWN_Y;
					for (int i = 0; i < MAX_BARRELS; ++i) {
						barrels[i].x = BARREL_SPAWN_X;
						barrels[i].y = BARREL_SPAWN_Y;
						barrels[i].isAlive = false;
					}

					break;
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
			}
		}
	}
}

void fallingOffPlatform(struct Player* mainchar) {
	if (mainchar->mcX > 0 && mainchar->mcX < SCREEN_WIDTH - PLATFORM_LENGHT && mainchar->mcY == MC_ON_PLATFORM_ONE_Y) {
		mainchar->mcY = MC_ON_GROUND_Y;
	}

	if (mainchar->mcX > PLATFORM_LENGHT && mainchar->mcX < SCREEN_WIDTH && mainchar->mcY == MC_ON_PLATFORM_TWO_Y) {
		mainchar->mcY = MC_ON_PLATFORM_ONE_Y;
	}

	if (mainchar->mcX > 0 && mainchar->mcX < SCREEN_WIDTH - PLATFORM_LENGHT && mainchar->mcY == MC_ON_PLATFORM_THREE_Y) {
		mainchar->mcY = MC_ON_PLATFORM_TWO_Y;
	}
}

void updateRenderer(SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer) {
	SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void ladderMovement(struct Player* mainchar, struct KeyboardInstructions* instructions, struct Hitbox* ladderHitbox, struct Hitbox ladders[6]) {

	ladders[0] = { LADDER_COLUMN_ONE_X, LADDER_ROW_ONE_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[1] = { LADDER_COLUMN_THREE_X, LADDER_ROW_ONE_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[2] = { LADDER_COLUMN_TWO_X, LADDER_ROW_TWO_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[3] = { LADDER_COLUMN_FOUR_X, LADDER_ROW_TWO_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[4] = { LADDER_COLUMN_ONE_X, LADDER_ROW_THREE_Y, LADDER_WIDTH, LADDER_HEIGHT };
	ladders[5] = { LADDER_COLUMN_THREE_X, LADDER_ROW_THREE_Y, LADDER_WIDTH, LADDER_HEIGHT };

	if (mainchar->isAlive) {
		for (int i = 0; i < 6; ++i) {
			if (checkCollisionWithLadder(mainchar->mcX, mainchar->mcY, CHARACTER_WIDTH, CHARACTER_HEIGHT, &ladders[i])) {
				instructions->onLadder = 1;
				ladderHitbox = &ladders[i];
				break;
			}
		}

		if (instructions->onLadder) {
			if (mainchar->mcY < ladderHitbox->y) {
				if (instructions->event.key.keysym.sym == SDLK_s) {
					mainchar->isDescending = true;
					mainchar->isClimbing = false;
					mainchar->movingLeft = false;
					mainchar->movingRight = false;
					mainchar->mcY += LADDER_HEIGHT;
				}
			}
			else if (mainchar->mcY + CHARACTER_HEIGHT > ladderHitbox->y + ladderHitbox->height) {
				if (instructions->event.key.keysym.sym == SDLK_w) {
					mainchar->isClimbing = true;
					mainchar->isDescending = false;
					mainchar->movingLeft = false;
					mainchar->movingRight = false;
					mainchar->mcY -= LADDER_HEIGHT;
				}
			}
			else {
				switch (instructions->event.key.keysym.sym) {
				case SDLK_w:
					while (mainchar->mcY < LADDER_ROW_ONE_Y + (CHARACTER_HEIGHT / 2)) {
						mainchar->isClimbing = true;
						mainchar->isDescending = false;
						mainchar->movingLeft = false;
						mainchar->movingRight = false;
						mainchar->mcY -= LADDER_HEIGHT;
					}
					break;
				case SDLK_s:
					mainchar->isDescending = true;
					mainchar->isClimbing = false;
					mainchar->movingLeft = false;
					mainchar->movingRight = false;
					mainchar->mcY += LADDER_HEIGHT;
					break;
				}
			}
		}
	}
}

void gameLoop(struct Hitbox ladders[], struct KeyboardInstructions* instructions, struct Player* mainchar, struct Barrel barrel, struct Barrel barrels[], SDL_Texture* scrtex, SDL_Surface* screen, SDL_Renderer* renderer, struct GameState* gameState, struct Elements* element, struct Colors* colors, struct Hitbox* ladderHitbox, SDL_Event event, SDL_Window* window) {
	
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
	if (mainchar->mcX == BARREL_SPAWN_X + BARREL_WIDTH) {
		instructions->quit = 1;
		SDL_FillRect(screen, NULL, colors->black);
		sprintf(colors->text, "VICTORY!!!!!!!!!!!");
		DrawString(screen, screen->w / 2 - strlen(colors->text) * 8 / 2, 10, colors->text, element->charset);
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
	gameState.epsilon = SENSITIVITY_RADIUS;
	gameState.timeSinceLastSpawn = 0.0;
	for (int i = 0; i < MAX_BARRELS; ++i) {
		barrels[i].isAlive = false;
	}

	while (!instructions.quit) {
		t2 = SDL_GetTicks();
		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplyna� od ostatniego narysowania ekranu
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

		instructions.onLadder = 0;

		ladderMovement(&mainchar, &instructions, &ladderHitbox, ladders);

		//print background
		printAllVisuals(screen, &colors, &instructions, &element, &mainchar, &gameState);

		//spawn barrels
		spawnBarrels(&mainchar, screen, barrels, &element, &gameState);

		// controls for when mc is dead
		mcDeadControls(&mainchar, &instructions, barrels);

		//controls for when mc is alive
		mcAliveControls(&mainchar, &instructions, barrels, screen, &gameState);

		// logic for when mc falls off the edge of a platform
		fallingOffPlatform(&mainchar);

		animations(screen, &mainchar);

		ifVictory(&mainchar, &instructions, screen, &gameState, &colors, &element);

		updateRenderer(scrtex, screen, renderer);

		gameState.frames++;
	}
	closingGame(window, renderer, screen, scrtex, &element);
	return 0;
	};
