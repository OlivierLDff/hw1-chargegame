#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define GAME_TITLE "ChargeGame"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define MAX_FILE_NAME 512

const char N_TEXTURE_BACKGROUND[] = "./images/background.png";
const char N_TEXTURE_PARTICLE[] = "./images/particle.png";
const char N_TEXTURE_CHARGE[] = "./images/charge.png";
const char N_TEXTURE_NEGCHARGE[] = "./images/negcharge.png";

bool gIsRunning = true;

/** 
 * \brief a charge can be placed inside a map to attract or repulse any balls.
 */
typedef struct charge_t
{
	/** \brief position in the x axis, between 0 and 100 */
	double x; 
	/** \brief position in the x axis, between 0 and 100 */
	double y; 
	/** \brief force that this charge apply in newton */
	double f; 
}charge_t;

SDL_Renderer* gSdlRenderer = NULL;
SDL_Texture * gSdlTBackground = NULL; 
SDL_Window* gSdlWindow = NULL;

SDL_Texture * LoadImage(const char * name)
{
	SDL_Surface * loadedImage = NULL;
	SDL_Texture * texture = NULL;
	loadedImage = IMG_Load(name);

	if (loadedImage != NULL)
	{
		texture = SDL_CreateTextureFromSurface(gSdlRenderer, loadedImage);

		SDL_FreeSurface(loadedImage);
		loadedImage = NULL;
	}
	else
	{
		fprintf(stderr, "Error : Can't load image in %s \n", SDL_GetError());
		return NULL;
	}
	return texture;
}

void InitBackgroundRender()
{
	gSdlTBackground = LoadImage(N_TEXTURE_BACKGROUND);
}


void DrawBackground()
{
	SDL_Rect dest;

	dest.w = SCREEN_WIDTH;
	dest.h = SCREEN_HEIGHT;
	dest.x = 0;
	dest.y = 0;

	SDL_RenderCopy(gSdlRenderer, gSdlTBackground, NULL, &dest);
}

void LimitTo60Fps(unsigned int LastUpdateTick)
{
	uint32_t ticks = SDL_GetTicks();

	if (LastUpdateTick < ticks)
		return;

	if (ticks - LastUpdateTick <= 16)
		SDL_Delay(16 - (ticks - LastUpdateTick));
}

void PollInput()
{
	// event handling
	SDL_Event e;
	while (SDL_PollEvent(&e)) //Poll every input
	{
		if (e.type == SDL_QUIT) gIsRunning = false;
			//break; 
		else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE) gIsRunning = false;
			//break;
	}
}

void UpdateGame(uint32_t delta)
{
	
}

void RenderGame()
{
	SDL_RenderClear(gSdlRenderer);
	DrawBackground();
	SDL_RenderPresent(gSdlRenderer);
}

void ClearSDL()
{
	SDL_DestroyWindow(gSdlWindow);
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	unsigned int LastTick = SDL_GetTicks() + 16;
	SDL_Init(SDL_INIT_EVERYTHING);

	//Initialize PNG loading 
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
		return false;
	}

	/*Open the window, by default the window is centered*/
	gSdlWindow = SDL_CreateWindow(GAME_TITLE,
	                 SDL_WINDOWPOS_CENTERED,
	                 SDL_WINDOWPOS_CENTERED,
	                 SCREEN_WIDTH, SCREEN_HEIGHT,
	                 SDL_WINDOW_SHOWN);

	gSdlRenderer = SDL_CreateRenderer(gSdlWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(gSdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gSdlRenderer);
	SDL_RenderPresent(gSdlRenderer);

	atexit(ClearSDL);
	
	InitBackgroundRender();

	while (gIsRunning)
	{
		uint32_t CurrentTick = SDL_GetTicks();
		PollInput();
		UpdateGame(CurrentTick - LastTick);
		RenderGame();

		LimitTo60Fps(CurrentTick);
		LastTick = CurrentTick;
	}

	return 0;
}