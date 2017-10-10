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
const char N_TEXTURE_DRAGCHARGE[] = "./images/chargeDrag.png";
const char N_TEXTURE_SELCHARGE[] = "./images/chargeSelection.png";

SDL_Texture * SDL_LoadImage(const char * name);

bool gIsRunning = true;
SDL_Renderer* gSdlRenderer = NULL;
SDL_Texture * gtBACKGROUND = NULL;
SDL_Window* gSdlWindow = NULL;

SDL_Texture * gtPARTICLE = NULL;
SDL_Texture * gtCHARGE1 = NULL;
SDL_Texture * gtCHARGE2 = NULL;
SDL_Texture * gtCHARGE3 = NULL;
SDL_Texture * gtNEGCHARGE1 = NULL;
SDL_Texture * gtNEGCHARGE2 = NULL;
SDL_Texture * gtNEGCHARGE3 = NULL;

SDL_Texture * gtOVERLAYCHARGE = NULL;
SDL_Texture * gtDRAGCHARGE = NULL;

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


typedef enum EObjectType
{
	EObjectType_Unknown,
	EObjectType_Particle,
	EObjectType_ppp,
	EObjectType_pp,
	EObjectType_p,
	EObjectType_m,
	EObjectType_mm,
	EObjectType_mmm
}EObjectType;

typedef struct UBase
{
	EObjectType s;
	double x;
	double y;
}UBase;

typedef struct UCharge
{
	EObjectType s;
	/** \brief position in the x axis, between 0 and 100 */
	double x;
	/** \brief position in the x axis, between 0 and 100 */
	double y;
	/** \brief force that this charge apply in newton */
	double f;
}UCharge;

typedef struct UParticle
{
	EObjectType s;
	/** \brief position in the x axis, between 0 and 100 */
	double x;
	/** \brief position in the x axis, between 0 and 100 */
	double y;
	/** \brief speed of the particle x axis */
	double dx;
	/** \brief speed of the particle y axis */
	double dy;
}UParticle;

typedef union UObject
{
	UBase b;
	UCharge c;
	UParticle p;
}UObject;

UBase * gdragCharge = NULL;
da_t da_obj;
UParticle * gmp = NULL;

SDL_Texture * eotot(EObjectType e)
{
	switch (e)
	{
	case EObjectType_Particle:return gtPARTICLE; break;
	case EObjectType_ppp:return gtCHARGE3; break;
	case EObjectType_pp:return gtCHARGE2; break;
	case EObjectType_p: return gtCHARGE1; break;
	case EObjectType_m: return gtNEGCHARGE1; break;
	case EObjectType_mm: return gtNEGCHARGE2; break;
	case EObjectType_mmm: return gtNEGCHARGE3; break;
	default: return gtPARTICLE;
	}
}

void ch_init(UCharge * this, double x, double y)
{
	this->x = x;
	this->y = y;
	this->f = 0;
	this->s = EObjectType_p;
}

void ub_render(UBase * this)
{
	SDL_Rect dest;

	if (this->x >= 0 && this->y >= 0 && this->x <= 100 && this->y <= 100)
	{
		dest.w = 32;
		dest.h = 32;
		dest.x = (int)(this->x*gCurrentWidth / 100 - 16);
		dest.y = (int)(this->y*gCurrentHeight / 100 - 16);

		if(gdragCharge == this) 
			SDL_RenderCopy(gSdlRenderer, gtDRAGCHARGE, NULL, &dest);

		SDL_RenderCopy(gSdlRenderer, eotot(this->s), NULL, &dest);
	}	
}

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

SDL_Texture * SDL_LoadImage(const char * name)
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

void p_init()
{
	if (gmp == NULL) gmp = malloc(sizeof(UParticle));
	if (gmp)
		memset(gmp, 0, sizeof(UParticle));
	else
		fprintf(stderr, "Fail to allocate memory for gMainParticle\n");

	gmp->x = 50;
	gmp->y = 50;
	gmp->dx = 0;
	gmp->dy = 0;
	gmp->s = EObjectType_Particle;

	da_push(&da_obj, gmp);
}
/*
void DestroyParticle()
{
	if (gmp) free(gmp);
}*/

void br_init()
{
	gtBACKGROUND = SDL_LoadImage(N_TEXTURE_BACKGROUND);
}

void br_destroy()
{
	if(gtBACKGROUND) SDL_DestroyTexture(gtBACKGROUND);
}

void br_render()
{
	SDL_Rect dest;

	dest.w = gCurrentWidth;
	dest.h = gCurrentHeight;
	dest.x = 0;
	dest.y = 0;

	SDL_RenderCopy(gSdlRenderer, gtBACKGROUND, NULL, &dest);
}

void clamp60fps(unsigned int LastUpdateTick)
{
	uint32_t ticks = SDL_GetTicks();

	if (LastUpdateTick < ticks)
		return;

	if (ticks - LastUpdateTick <= 16)
		SDL_Delay(16 - (ticks - LastUpdateTick));
}

bool ch_over(UCharge* elt, double x, double y)
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

UCharge* findCharge(int x, int y)
{
	da_elm_t * it = da_obj.first;
	while(it)
	{
		EObjectType e = it->elt ? ((UBase*)it->elt)->s : EObjectType_Unknown;
		if (e >= EObjectType_Particle && e <= EObjectType_mmm && ch_over((UCharge*)it->elt, x * 100 / (double)gCurrentWidth, y * 100 / (double)gCurrentHeight))
			return (UCharge*)it->elt;
		it = it->next;
	}
	return NULL;
}

void ch_create(int x, int y)
{
	UCharge * c = malloc(sizeof(UCharge));
	da_push(&da_obj, c);
	ch_init(c, x*100/(double)gCurrentWidth, y*100/ (double)gCurrentHeight);
}

void SDL_HandleLeftClick(bool bUp, int x, int y)
{
	if(bUp)
	{
		if (gdragCharge)
		{
			gdragCharge = NULL;
		}
		else
		{
			ch_create(x, y);
		}
	}
	else
	{
		UCharge * c = findCharge(x, y);
		if (c)
		{
			gdragCharge = c;
		}
	}
}

void SDL_HandleRightClick(bool bUp, int x, int y)
{
	if (bUp)
	{
		UCharge * c = findCharge(x, y);
		if (c)
		{
			EObjectType e = c ? ((UBase*)c)->s : EObjectType_Unknown;
			if (e >= EObjectType_ppp && e <= EObjectType_mmm)
			{
				void * res = da_removeat(&da_obj, c);
				if (res == c) free(c);
				else fprintf(stderr, "Error removing charge\n");
			}		
		}
	}
}

void SDL_HandleMiddleClick(bool bUp, int x, int y)
{
	UCharge * c = findCharge(x, y);
	EObjectType e = c ? ((UBase*)c)->s : EObjectType_Unknown;
	if(c && e >= EObjectType_ppp && e <= EObjectType_mmm)
	{	
		if (bUp)
		{
			if (c->s == EObjectType_ppp)
				c->s = EObjectType_mmm;
			else
				c->s--;
		}
		else
		{
			if (c->s == EObjectType_mmm)
				c->s = EObjectType_ppp;
			else
				c->s++;
		}
	}
}

void m_init()
{
	/*Open the window, by default the window is centered*/
	gSdlWindow = SDL_CreateWindow(GAME_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!gSdlWindow) fprintf(stderr, "Can't create gSdlWindow\n");

	gSdlRenderer = SDL_CreateRenderer(gSdlWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!gSdlRenderer) fprintf(stderr, "Can't create gSdlRenderer\n");

	gtPARTICLE = SDL_LoadImage(N_TEXTURE_PARTICLE);
	gtCHARGE1 = SDL_LoadImage(N_TEXTURE_CHARGE);
	gtCHARGE2 = SDL_LoadImage(N_TEXTURE_CHARGE2);
	gtCHARGE3 = SDL_LoadImage(N_TEXTURE_CHARGE3);
	gtNEGCHARGE1 = SDL_LoadImage(N_TEXTURE_NEGCHARGE);
	gtNEGCHARGE2 = SDL_LoadImage(N_TEXTURE_NEGCHARGE2);
	gtNEGCHARGE3 = SDL_LoadImage(N_TEXTURE_NEGCHARGE3);
	gtDRAGCHARGE = SDL_LoadImage(N_TEXTURE_DRAGCHARGE);
	gtOVERLAYCHARGE = SDL_LoadImage(N_TEXTURE_SELCHARGE);

	da_init(&da_obj);
	br_init();
	p_init();
}

void SDL_PollInput()
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
				SDL_HandleLeftClick(e.type == SDL_MOUSEBUTTONUP, x, y);
				break;
			case SDL_BUTTON_RIGHT:
				SDL_HandleRightClick(e.type == SDL_MOUSEBUTTONUP, x, y);
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
				SDL_HandleMiddleClick(true, x, y);
			}
			else if (e.wheel.y == -1) // scroll down
			{
				SDL_HandleMiddleClick(false, x, y);
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

void p_render()
{
	ub_render((UBase*)gmp);
}

double getffeo(EObjectType object)
{
	double f = 0;
	switch(object)
	{
	case EObjectType_Unknown: break;
	case EObjectType_Particle: break;
	case EObjectType_ppp: f = 3.f; break;
	case EObjectType_pp: f = 2.f; break;
	case EObjectType_p:  f = 1.f; break;
	case EObjectType_m: f = -1.f; break;
	case EObjectType_mm: f = -2.f; break;
	case EObjectType_mmm: f = -3.f; break;
	default: ;
	}
	return f;
}
void clamp(double* p, double from, double to)
{
	if (!p) return;
	if (from < to)
	{
		if (*p < from) *p = from;
		if (*p > to) *p = to;
	}
	else
	{
		if (*p > from) *p = from;
		if (*p < to) *p = to;
	}
}

const double MAX_ACCELERATION = 0.0005f;
const double MAX_SPEED = 0.1f;
const double PARTICLE_WEIGHT_CONST = 0.005f;
const double AIR_FRICTION = 0.001f;
bool ftoa(UCharge * c, double* fx, double* fy, double x, double y)
{
	if (!(fx && fy)) return false;
	double f = getffeo(c->s);
	double xd = x - c->x;
	double yd = y - c->y;
	double d = sqrt(xd*xd + yd*yd);
	f = (d > 5.f) ? f / d : 0.f;
	*fx += f * (d ? xd / d : 0);
	*fy += f *  (d ? yd / d : 0);
	return true;
}

void m_update(uint32_t delta)
{
	int f = 0;
	if (!delta) return;
	if (gdragCharge == (UBase*)gmp)
	{
		static double lastx = 0;
		static double lasty = 0;
		gmp->dx = (gmp->x - lastx) / delta;
		gmp->dy = (gmp->y - lasty) / delta;
		clamp(&gmp->dx, -MAX_SPEED, MAX_SPEED);
		clamp(&gmp->dy, -MAX_SPEED, MAX_SPEED);
		lastx = gmp->x;
		lasty = gmp->y;
		return;
	}
	da_elm_t * it = da_obj.first;
	while(it)
	{
		double fx = 0;
		double fy = 0;
		double vx = 0;
		double vy = 0;

		vx = gmp->x;
		vy = gmp->y;

		EObjectType e = it->elt ? ((UBase*)it->elt)->s : EObjectType_Unknown;
		if(e >= EObjectType_ppp && e <= EObjectType_mmm)
		{			
			if(ftoa((UCharge*)it->elt, &fx, &fy, gmp->x, gmp->y))
			printf("force x = %lf, y = %lf\n", fx, fy);	
		}
		fx -= gmp->dx*AIR_FRICTION;
		fy -= gmp->dy*AIR_FRICTION;

		fx *= PARTICLE_WEIGHT_CONST;
		fy *= PARTICLE_WEIGHT_CONST;

		clamp(&fx, -MAX_ACCELERATION, MAX_ACCELERATION);
		clamp(&fy, -MAX_ACCELERATION, MAX_ACCELERATION);

		gmp->x += gmp->dx*delta + fx*delta*delta;
		gmp->y += gmp->dy*delta + fy*delta*delta;

		clamp(&gmp->x, 0.f, 100.f);
		clamp(&gmp->y, 0.f, 100.f);


		gmp->dx = (gmp->x - vx)/delta;
		gmp->dy = (gmp->y - vy)/delta;

		clamp(&gmp->dx, -MAX_SPEED, MAX_SPEED);
		clamp(&gmp->dy, -MAX_SPEED, MAX_SPEED);

		it = it->next;
	}
}

void da_obj_order()
{
	da_elm_t * tnn;
	da_elm_t * tn;
	da_elm_t * tp; 
	da_elm_t * it;
	da_elm_t * it2;
	bool bChange = true;
	while(bChange) //bubble sort
	{
		if (da_obj.size == 2)
			bChange = true;
		it = da_obj.first;
		bChange = false; 
		while (it)
		{
			if (it->elt && it->next	&& it->next->elt && ((UBase *)it->elt)->y > ((UBase *)it->next->elt)->y)
			{
				tnn = it->next->next;
				tn = it->next;
				tp = NULL;
				it2 = da_obj.first;
				while (tp && !(tp->next == it)) tp = tp->next;
				if (tp) tp->next = it;

				if (da_obj.first == it) da_obj.first = tn;
				tn->next = it;
				it->next = tnn;
	
				while(it2 && !(it2->next == it)) it2 = it2->next;
				if (it2) it2->next = tn;
				bChange = true;
			}
			it = it->next;
		}
	}	
}

void da_obj_render(da_t* da)
{
	da_elm_t * it = da_obj.first;
	da_obj_order();
	while (it)
	{
		if (it->elt)
		{
			switch (((UBase*)it->elt)->s)
			{
			case EObjectType_Unknown: break;
			case EObjectType_Particle: 
				p_render();
				break;
			case EObjectType_ppp: 
			case EObjectType_pp: 
			case EObjectType_p: 
			case EObjectType_m: 
			case EObjectType_mm: 
			case EObjectType_mmm:
				ub_render((UCharge*)it->elt);
			default: ;
			}		
		}
		else
			printf("Error : presence of nullptr obj in array\n");
		it = it->next;
	}
}

void m_render()
{
	SDL_RenderClear(gSdlRenderer);

	br_render();
	da_obj_render(&da_obj);

	SDL_RenderPresent(gSdlRenderer);
}

void m_clear()
{
	void * elt;
	while ((elt = da_remove(&da_obj))) free(elt);
	br_destroy();
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

	m_init();
	while (gIsRunning)
	{
		uint32_t CurrentTick = SDL_GetTicks();
		SDL_PollInput();
		m_update(CurrentTick - LastTick);
		m_render();

		clamp60fps(CurrentTick);
		LastTick = CurrentTick;
	}

	m_clear();

	return 0;
}