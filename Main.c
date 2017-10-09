#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define GAME_TITLE "ChargeGame"
#define SCREEN_WIDTH 640
int gCurrentWidth = SCREEN_WIDTH;
#define SCREEN_HEIGHT 480
int gCurrentHeight = SCREEN_HEIGHT;

#define MAX_FILE_NAME 512

const char N_TEXTURE_BACKGROUND[] = "./images/background.png";
const char N_TEXTURE_PARTICLE[] = "./images/particle.png";
const char N_TEXTURE_CHARGE[] = "./images/charge.png";
const char N_TEXTURE_CHARGE2[] = "./images/charge2.png";
const char N_TEXTURE_CHARGE3[] = "./images/charge3.png";
const char N_TEXTURE_NEGCHARGE[] = "./images/negcharge.png";
const char N_TEXTURE_NEGCHARGE2[] = "./images/negcharge2.png";
const char N_TEXTURE_NEGCHARGE3[] = "./images/negcharge3.png";

SDL_Texture * LoadImage(const char * name);

bool gIsRunning = true;
SDL_Renderer* gSdlRenderer = NULL;
SDL_Texture * gSdlTBackground = NULL;
SDL_Window* gSdlWindow = NULL;

SDL_Texture * gtPARTICLE = NULL;
SDL_Texture * gtCHARGE1 = NULL;
SDL_Texture * gtCHARGE2 = NULL;
SDL_Texture * gtCHARGE3 = NULL;
SDL_Texture * gtNEGCHARGE1 = NULL;
SDL_Texture * gtNEGCHARGE2 = NULL;
SDL_Texture * gtNEGCHARGE3 = NULL;

typedef enum EChargeState
{
	EChargeState_Unknown,
	EChargeState_Particle,
	EChargeState_ppp,
	EChargeState_pp,
	EChargeState_p,
	EChargeState_m,
	EChargeState_mm,
	EChargeState_mmm
}EChargeState;

typedef struct particle_t
{
	/** \brief position in the x axis, between 0 and 100 */
	double x;
	/** \brief position in the x axis, between 0 and 100 */
	double y;
	/** \brief position in the x axis, between 0 and 100 */
	double dx;
	/** \brief position in the x axis, between 0 and 100 */
	double dy;
	SDL_Texture * t;
}particle_t;

//The button
typedef struct charge_t
{
	/** \brief position in the x axis, between 0 and 100 */
	double x;
	/** \brief position in the x axis, between 0 and 100 */
	double y;
	/** \brief force that this charge apply in newton */
	double f;
	EChargeState s;
	SDL_Texture * t;
}charge_t;

charge_t * gdragCharge = NULL;

SDL_Texture * LoadCorrectImageParticle(enum EChargeState e)
{
	switch (e)
	{
	case EChargeState_Particle:return gtPARTICLE; break;
	case EChargeState_ppp:return gtCHARGE3; break;
	case EChargeState_pp:return gtCHARGE2; break;
	case EChargeState_p: return gtCHARGE1; break;
	case EChargeState_m: return gtNEGCHARGE1; break;
	case EChargeState_mm: return gtNEGCHARGE2; break;
	case EChargeState_mmm: return gtNEGCHARGE3; break;
	default: return gtPARTICLE;
	}
}

void ch_init(charge_t * this, double x, double y)
{
	this->x = x;
	this->y = y;
	this->f = 50;
	this->s = EChargeState_p;
}

void ch_handleEvent()
{
	
}

void ch_render(charge_t * this)
{
	SDL_Rect dest;

	if (this->x >= 0 && this->y >= 0 && this->x <= 100 && this->y <= 100)
	{
		dest.w = 32;
		dest.h = 32;
		dest.x = (int)(this->x*gCurrentWidth / 100 - 16);
		dest.y = (int)(this->y*gCurrentHeight / 100 - 16);
		this->t = LoadCorrectImageParticle(this->s);

		SDL_RenderCopy(gSdlRenderer, this->t, NULL, &dest);
	}	
}

typedef struct da_elm_t
{
	struct da_elm_t * next;
	void * elt;
} da_elm_t;

typedef struct da_t
{
	struct da_elm_t * first;
	size_t size;
} da_t;

void da_init(da_t * this)
{
	if(this) memset(this, 0, sizeof(da_t));
}

void da_push(da_t * this, void * elt)
{
	if(!this || !elt) return;
	da_elm_t * nElm = malloc(sizeof(da_elm_t));
	memset(nElm, 0, sizeof(da_elm_t));
	nElm->elt = elt;
	
	if (this->first == NULL)
		this->first = nElm;
	else
	{
		da_elm_t * it = this->first;
		while (it->next) it = it->next;
		it->next = nElm;
	}
	++this->size;
}

void da_insert(da_t * this, void * elt, size_t pos)
{
	if (!(pos >= 0 && pos < this->size)) return; //outofbound
	da_elm_t * nElm = malloc(sizeof(da_elm_t)); nElm->elt = elt;
	memset(nElm, 0, sizeof(da_elm_t));
	if (pos == 0)
	{
		nElm->next = this->first;
		this->first = nElm;
	}
	else
	{
		da_elm_t * it = this->first;
		size_t i = 0;
		while (it)
		{
			if (i == pos)
			{
				da_elm_t * tmp = it->next;
				it->next = nElm;
				nElm->next = tmp;		
				break;
			}
			++i;
			it = it->next;
		}
	}
	++this->size;
}

void * da_removeat(da_t * this, void * elt)
{
	void * eltRs;
	if (this)
	{
		da_elm_t * it = this->first;
		while (it)
		{
			if (it->elt == elt)
			{
				if(it == this->first)
				{
					this->first = this->first->next;
					eltRs = it->elt;
					free(it);
					--this->size;
					return eltRs;
				}
				da_elm_t * it2 = this->first;
				while (it2)
				{
					if (it2->next == it)
					{
						it2->next = it->next;
						eltRs = it->elt;
						free(it);
						--this->size;
						return eltRs;
					}
					it2 = it2->next;
				}
					
			}		
			it = it->next;
		}
	}
	return NULL;
}

void * da_remove(da_t * this)
{
	if(this)
	{
		da_elm_t * it = this->first;
		while (it && it->next) it = it->next;
		return it ? da_removeat(this, it->elt) : NULL;
	}
	return NULL;
}

da_t da_ch;
particle_t * gmp = NULL;

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

void InitParticle()
{
	if (gmp == NULL) gmp = malloc(sizeof(particle_t));	
	if (gmp)
		memset(gmp, 0, sizeof(struct particle_t));
	else
		fprintf(stderr, "Fail to allocate memory for gMainParticle\n");

	gmp->t = LoadCorrectImageParticle(EChargeState_Particle);

	gmp->x = 50;
	gmp->y = 50;
}

void DestroyParticle()
{
	if (gmp) free(gmp);
}

void InitBackgroundRender()
{
	gSdlTBackground = LoadImage(N_TEXTURE_BACKGROUND);
}

void DestroyTexture()
{
	if(gSdlTBackground) SDL_DestroyTexture(gSdlTBackground);
}

void DrawBackground()
{
	SDL_Rect dest;

	dest.w = gCurrentWidth;
	dest.h = gCurrentHeight;
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

bool ch_over(charge_t* elt, double x, double y)
{
	double dx = 16.f * 100 / (double)gCurrentWidth;
	double dy = 16.f * 100 / (double)gCurrentHeight;
	if(elt)
	{
		if (fabs(elt->x - x) <= dx &&fabs(elt->y - y) <= dy)
			return true;
	}
	return false;
}

charge_t* OverCharge(int x, int y)
{
	da_elm_t * it = da_ch.first;
	while(it)
	{
		if (ch_over((charge_t*)it->elt, x * 100/(double)gCurrentWidth, y * 100/(double)gCurrentHeight))
		{
			return (charge_t*)it->elt;
		}
		it = it->next;
	}
	return NULL;
}

void CreateCharge(int x, int y)
{
	charge_t * c = malloc(sizeof(charge_t));
	da_push(&da_ch, c);
	ch_init(c, x*100/(double)gCurrentWidth, y*100/ (double)gCurrentHeight);
}

void HandleLeftClick(bool bUp, int x, int y)
{
	if(bUp)
	{
		if (gdragCharge)
		{
			gdragCharge = NULL;
		}
		else
		{
			CreateCharge(x, y);
		}
	}
	else
	{
		charge_t * c = OverCharge(x, y);
		if (c)
		{
			gdragCharge = c;
		}
	}
}

void HandleRightClick(bool bUp, int x, int y)
{
	if (bUp)
	{
		charge_t * c = OverCharge(x, y);
		if (c)
		{
			void * res = da_removeat(&da_ch, c);
			if (res == c) free(c);
			else fprintf(stderr, "Error removing charge\n");
		}
	}
}

void HandleMiddleClick(bool bUp, int x, int y)
{
	charge_t * c = OverCharge(x, y);
	if(c)
	{
		if(bUp)
		{
			if (c->s == EChargeState_ppp)
				c->s = EChargeState_mmm;
			else
				c->s--;
		}
		else
		{
			if (c->s == EChargeState_mmm)
				c->s = EChargeState_ppp;
			else
				c->s++;
		}
	}
}

void InitWindow()
{
	/*Open the window, by default the window is centered*/
	gSdlWindow = SDL_CreateWindow(GAME_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	gSdlRenderer = SDL_CreateRenderer(gSdlWindow, -1, SDL_RENDERER_ACCELERATED);
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
			//If mouse event happened
		if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
		{
			//Get mouse position
			int x, y;
			SDL_GetMouseState(&x, &y);
			printf("mouse click : %d %d\n", x, y);

			//for all the charge check if it is inside sprite form if so change the charge
			switch (e.button.button)
			{
			case SDL_BUTTON_LEFT:
				HandleLeftClick(e.type == SDL_MOUSEBUTTONUP, x, y);
				break;
			case SDL_BUTTON_RIGHT:
				HandleRightClick(e.type == SDL_MOUSEBUTTONUP, x, y);
				break;
			default: ;
			}
		}
		if(e.type == SDL_MOUSEMOTION)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			if(gdragCharge)
			{
				gdragCharge->x = x * 100 / (double)gCurrentWidth;
				gdragCharge->y = y * 100 / (double)gCurrentHeight;
			}
		}
		if (e.type == SDL_MOUSEWHEEL)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (e.wheel.y == 1) // scroll up
			{
				HandleMiddleClick(true, x, y);
			}
			else if (e.wheel.y == -1) // scroll down
			{
				HandleMiddleClick(false, x, y);
			}
		}
		if (e.type == SDL_WINDOWEVENT) //windows resize
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				gCurrentWidth = e.window.data1;
				gCurrentHeight = e.window.data2;
			}
		}
	}
}

void DrawParticle()
{
	SDL_Rect dest;

	if(gmp->x >= 0 && gmp->y >= 0 && gmp->x <= 100 && gmp->y <= 100)

	dest.w = 32;
	dest.h = 32;
	dest.x = (int)(gmp->x*gCurrentWidth/100 - 16);
	dest.y = (int)(gmp->y*gCurrentHeight / 100 - 16);

	SDL_RenderCopy(gSdlRenderer, gmp->t, NULL, &dest);
}

void UpdateGame(uint32_t delta)
{
	
}

void da_ch_render(da_t* da)
{
	da_elm_t * it = da_ch.first;
	while (it)
	{
		ch_render((charge_t*)it->elt);
		it = it->next;
	}
}

void RenderGame()
{
	SDL_RenderClear(gSdlRenderer);

	DrawBackground();
	DrawParticle();
	da_ch_render(&da_ch);

	SDL_RenderPresent(gSdlRenderer);
}

void ClearSDL()
{
	void * elt;
	while ((elt = da_remove(&da_ch))) free(elt);
	DestroyParticle();
	DestroyTexture();
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

	InitWindow();
	SDL_SetRenderDrawColor(gSdlRenderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gSdlRenderer);
	SDL_RenderPresent(gSdlRenderer);

	gtPARTICLE = LoadImage(N_TEXTURE_PARTICLE);
	gtCHARGE1 = LoadImage(N_TEXTURE_CHARGE);
	gtCHARGE2 = LoadImage(N_TEXTURE_CHARGE2);
	gtCHARGE3 = LoadImage(N_TEXTURE_CHARGE3);
	gtNEGCHARGE1 = LoadImage(N_TEXTURE_NEGCHARGE);
	gtNEGCHARGE2 = LoadImage(N_TEXTURE_NEGCHARGE2);
	gtNEGCHARGE3 = LoadImage(N_TEXTURE_NEGCHARGE3);

	InitBackgroundRender();
	InitParticle();
	da_init(&da_ch);

	while (gIsRunning)
	{
		uint32_t CurrentTick = SDL_GetTicks();
		PollInput();
		UpdateGame(CurrentTick - LastTick);
		RenderGame();

		LimitTo60Fps(CurrentTick);
		LastTick = CurrentTick;
	}

	ClearSDL();

	return 0;
}