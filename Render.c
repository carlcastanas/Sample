#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>

#include "Game.h"

static SDL_Window* gWindow;
static SDL_Renderer* gRenderer;
static TTF_Font* gFont;

static SDL_Texture* background;
static SDL_Texture* player;
static SDL_Texture* ownerBanner;
static SDL_Texture* diceSheet;
static SDL_Texture* house;
static SDL_Texture* cross;

static int SCREEN_WIDTH = 800;
static int SCREEN_HEIGHT = 800;

static void renderEverything();
static SDL_Texture* loadTexture(char *, SDL_Renderer*);

static int xCross = -1000;
static int yCross = -1000;

static int init() {
	int returnValue = 0;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		returnValue = 1;
	}
	else	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Monopoly", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			returnValue = 1;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				returnValue = 1;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
			}
		}
	}

	if( TTF_Init() == -1 )
	{
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
		returnValue = 1;
	}

	gFont = TTF_OpenFont( "fonts/UbuntuMono-R.ttf", 32 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        returnValue = 1;
    }

	background = loadTexture( "images/monopoly.bmp", gRenderer);
	player = loadTexture("images/player.bmp", gRenderer);
	ownerBanner = loadTexture("images/ownerbanner.bmp", gRenderer);
	diceSheet = loadTexture("images/dice.bmp", gRenderer);
	house = loadTexture("images/house.bmp", gRenderer);
	cross = loadTexture("images/cross.bmp", gRenderer);

	return returnValue;
}

static int close() {
	/*SDL_DestroyTexture( gTexture );
	gTexture = NULL;*/

	SDL_DestroyTexture(background);
	SDL_DestroyTexture(player);
	SDL_DestroyTexture(ownerBanner);
	SDL_DestroyTexture(diceSheet);
	SDL_DestroyTexture(house);
	SDL_DestroyTexture(cross);

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems	
	SDL_Quit();
}

static SDL_Texture* loadTexture(const char* file, SDL_Renderer *const renderer){
	SDL_Surface *loadedImage = NULL;
	SDL_Texture *texture = NULL;
	loadedImage = SDL_LoadBMP(file);
	if (loadedImage != NULL){
		SDL_SetColorKey(loadedImage, 1, 0x323232);
		texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
		
		SDL_FreeSurface(loadedImage);
	}
	else
		return NULL;
		/*logSDLError("Image could not be loaded!");*/
	return texture;
}

static void renderText(const char * text, int x, int y, SDL_Color color) {
	//SDL_Color color = {0, 255, 0};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color
	SDL_Rect Message_rect; //create a rect	
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(gFont, text, color);	
	SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage); //now you can convert it into a texture
	SDL_FreeSurface(surfaceMessage);
	
	Message_rect.x = x;  //controls the rect's x coordinate 
	Message_rect.y = y; // controls the rect's y coordinte
	/*Message_rect.w = 100; // controls the width of the rect
	Message_rect.h = 10; // controls the height of the rect*/
	
	SDL_QueryTexture(Message, NULL, NULL, &Message_rect.w, &Message_rect.h);
	SDL_RenderCopy(gRenderer, Message, NULL, &Message_rect); //
	SDL_DestroyTexture(Message);
}

static void renderTexture(SDL_Texture* tex, int x, int y) {
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopy(gRenderer, tex, NULL, &pos);
}

static void renderTextureRot(SDL_Texture* tex, int x, int y, double degrees) {
	SDL_Rect pos;
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
	SDL_RenderCopyEx(gRenderer, tex, NULL, &pos, degrees, NULL, SDL_FLIP_NONE);
}

void renderPlayerTextureAtPos(SDL_Texture* player_texture, int game_position, int playerid) {
	const int PROP_WIDTH = 66;
	const int PROP_HEIGHT = 80;
	const int GO_WIDTH = 103;	
	const int PLAYER_SIZE = 30;
	int xPos, yPos;		

	if (playerid == 0)
		SDL_SetTextureColorMod(player, 255, 0, 0);
	if (playerid == 1)
		SDL_SetTextureColorMod(player, 0, 0, 255);

	if (game_position <= 10) {
		xPos = SCREEN_WIDTH - (GO_WIDTH / 2 + PROP_WIDTH * (game_position + 1) - PLAYER_SIZE);
		yPos = SCREEN_HEIGHT - PROP_HEIGHT;

	}
	else if (game_position >= 11 && game_position <= 19)  {
		xPos = SCREEN_WIDTH - (GO_WIDTH / 2 + PROP_WIDTH * 11 - PLAYER_SIZE);
		yPos = SCREEN_HEIGHT - (GO_WIDTH / 2 + PROP_WIDTH * (game_position - 9) - PLAYER_SIZE);
		
	}
	else if (game_position >= 20 && game_position <= 30) {
		xPos = SCREEN_WIDTH - (GO_WIDTH / 2 + PROP_WIDTH * (31 - game_position ) - PLAYER_SIZE);
		yPos = PROP_HEIGHT - PLAYER_SIZE;
	}
	else if (game_position >= 31 && game_position <= 39) {
		xPos = SCREEN_WIDTH - (GO_WIDTH / 2 + PROP_WIDTH - PLAYER_SIZE);
		yPos = SCREEN_HEIGHT - (GO_WIDTH / 2 + PROP_WIDTH * (41 - game_position) - PLAYER_SIZE);
		
	}
		
	renderTexture(player_texture, xPos, yPos);
}

void renderPropOwnerAtPos(SDL_Texture* banner, int game_position, int playerid, int mortaged, int level) {
	const int PROP_WIDTH = 66;
	const int PROP_HEIGHT = 80;
	const int PROP_HEIGHT_FULL = 104;
	const int GO_WIDTH = 103;	
	const int PLAYER_SIZE = 30;
	const int BANNER_HEIGHT = 10;
	int xPos, yPos;
	double degrees = 0;
	int xPosH = 400, yPosH = 400;

	if (playerid == 0)
		SDL_SetTextureColorMod(ownerBanner, 255, 0, 0);
	if (playerid == 1)
		SDL_SetTextureColorMod(ownerBanner, 0, 0, 255);

	if (game_position % 10 == 0) return;

	if (game_position <= 10) {
		xPos = SCREEN_WIDTH - (GO_WIDTH + PROP_WIDTH * (game_position));
		yPos = SCREEN_HEIGHT - PROP_HEIGHT_FULL - BANNER_HEIGHT/2;
		xPosH = xPos;
		yPosH = yPos + 90;
	}
	else if (game_position >= 11 && game_position <= 19)  {
		xPos = PROP_HEIGHT - BANNER_HEIGHT;
		yPos = SCREEN_HEIGHT - (GO_WIDTH + PROP_WIDTH * (game_position - 10)) + 27;
		degrees = 90;
		xPosH = xPos - 70;
		yPosH = yPos - 27;
	}
	else if (game_position >= 20 && game_position <= 30) {
		xPos = SCREEN_WIDTH - (GO_WIDTH + PROP_WIDTH * (30 - game_position));
		yPos = PROP_HEIGHT_FULL - BANNER_HEIGHT/2;
		degrees = 180;
		xPosH = xPos;
		yPosH = yPos - 100;
	}
	else if (game_position >= 31 && game_position <= 39) {
		xPos = SCREEN_WIDTH - (PROP_HEIGHT_FULL + BANNER_HEIGHT/2 + 27);		
		yPos = SCREEN_HEIGHT - (GO_WIDTH + PROP_WIDTH * (40 - game_position)) + 27;
		degrees = 270;
		xPosH = xPos + 120;
		yPosH = yPos - 27;
	}

	renderTextureRot(banner, xPos, yPos, degrees);

	if (mortaged) {
		SDL_Rect pos;
		SDL_SetTextureColorMod(ownerBanner, 0, 0, 0);
		pos.x = xPos;
		pos.y = yPos;
		SDL_QueryTexture(banner, NULL, NULL, &pos.w, &pos.h);
		pos.w = 0.5 * pos.w;
		if (degrees == 90 || degrees == 270) {
			pos.x += 16;
			pos.y -= 17;
		}
		SDL_RenderCopyEx(gRenderer, banner, NULL, &pos, degrees, NULL, SDL_FLIP_NONE);
	}
	
	if (level > 0 && level <= 4) {
		int offset = 16;
		int i;
		for (i = 0; i < level; i++) {
			SDL_Rect r;
			if (degrees == 0 || degrees == 180) {			
				r.x = xPosH + offset * i;
				r.y = yPosH;
			}
			else {
				r.x = xPosH;
				r.y = yPosH + offset * i;
			}
			r.w = 16;
			r.h = 16;
			SDL_SetTextureColorMod(house, 0, 255, 0);

			SDL_RenderCopyEx(gRenderer, house, NULL, &r, degrees, NULL, SDL_FLIP_NONE);
		}
		//renderTextureRot(house, xPos, yPos, degrees);
	}

	if (level == 5) {
		SDL_SetTextureColorMod(house, 255, 0, 0);
		if (degrees == 0) {
			xPosH += 16;
			yPosH -= 16;
		}
		if (degrees == 90) {			
			yPosH += 16;
		}
		if (degrees == 180) {			
			xPosH += 16;
		}
		if (degrees == 270) {
			xPosH -= 16;
			yPosH += 16;
		}
		renderTextureRot(house, xPosH, yPosH, degrees);
	}
}

void renderDices(SDL_Texture* dice, int face1, int face2) {
	SDL_Rect pos, crop;
	int face[2] = {face1, face2};
	int i;
	for (i = 0; i < 2; i++) {
		pos.x = 300 + i * 80;
		pos.y = 280;
		pos.w = 192 / 3;
		pos.h = 128 / 2;
		crop.w = pos.w;
		crop.h = pos.h;	
		crop.x = face[i] % 3 == 0 ? 128 : (face[i] % 3 - 1) * 64;
		crop.y = face[i] > 3 ? 64 : 0;	
		//SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h);
		SDL_RenderCopy(gRenderer, dice, &crop, &pos);
	}	
}

static int parsePropFromPos(int x, int y) {
	const int PROP_WIDTH = 66;
	const int PROP_HEIGHT = 80;
	const int PROP_HEIGHT_FULL = 104;
	const int GO_WIDTH = 103;		

	if (y >= SCREEN_HEIGHT - GO_WIDTH) {		
		return 9 - (x - GO_WIDTH) / PROP_WIDTH;
	}

	if (y <= GO_WIDTH) {
		
		return (x - GO_WIDTH) / PROP_WIDTH + 21;
	}

	if (x <= GO_WIDTH) {
		return 19 - (y - GO_WIDTH) / PROP_WIDTH;
	}

	if (x >= SCREEN_WIDTH - GO_WIDTH) {
		return 31 + (y - GO_WIDTH) / PROP_WIDTH;
	}

	return -1;
}

static void processEventsAndRender() {
	int quit = 0;
	SDL_Event e;
	
	while( !quit )
	{
		renderEverything();
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = 1;
			}

			if ( e.type == SDL_KEYDOWN) 
			{
				Game_receiveinput(e.key.keysym.sym);

				/*switch( e.key.keysym.sym )
				{
				case SDLK_SPACE:
					Game_cycle();
					break;									
				}*/
			}

			if ( e.type == SDL_MOUSEBUTTONDOWN )
			{
				int x, y, s;
				SDL_GetMouseState(&x, &y);
				s = parsePropFromPos(x,y);				
				printf("%i %i %i\n", x,y, s);
				if (s >= 0) {
					xCross = x;
					yCross = y;
					Game_selectProperty(s);
				}
				else {
					xCross = -1000;
					yCross = -1000;
				}
					
			}
		}
	}
}

int main(int argc, char* argv[]) {	
		
	init();	

	Game_init();	

	processEventsAndRender();
	
	close();
}

void renderEverything() {
	SDL_Color color = {0, 0, 0};
	int i, j;
	char * text;
	SDL_RenderClear( gRenderer );
	renderTexture(background, 0, 0);

	Game_getLastRoll(&i, &j);

	renderDices(diceSheet, i, j);
	i = 0;
	while ((text = Game_getText(i)) != NULL) {
		renderText(text, 130, 440 + 40 * i, color);
		i++;
	}

	renderTextureRot(cross, xCross - 25, yCross - 25, 0);

	//renderTexture(diceSheet, 200, 200);

	for (i = 0; i < 2; i++) {
		char * reply = Game_getFormattedStatus(i);
		renderText(reply, 450, 120 + 40 * i, color);
		free(reply);
	}

	for (i = 0; i < Game_getTotalPlayers(); i++) {
		renderPlayerTextureAtPos(player, Game_getPlayerPosition(i), i);
	}

	for (i = 0; i < 40; i++) {
		j = Game_getPropOwner(i);
		if (j >= 0)
			renderPropOwnerAtPos(ownerBanner, i, j, Game_getPropMortageStatus(i), Game_getPropLevel(i));
	}

	SDL_RenderPresent( gRenderer );
}