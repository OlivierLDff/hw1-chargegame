#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

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
const char N_TEXTURE_FLAG[] = "./images/flagicon.png";
const char N_TEXTURE_BEGIN[] = "./images/begin.png";

const double MAX_ACCELERATION = 0.0005f;
const double MAX_SPEED = 0.1f;
const double PARTICLE_WEIGHT_CONST = 0.005f;
const double AIR_FRICTION = 0.001f;

const char M_GAMETITLE[] = "Charge Game";
const char M_FREEMODE[] = "FreeMode";
const char M_LEVEL[] = "Level";
const char M_HIGHSCORE[] = "Highscore";
const char M_QUIT[] = "Quit";
const char M_BACK[] = "Back";

const double P_GAMETILE_X = 50;
const double P_GAMETILE_Y = 10;

const double P_FREEMODE_X = 50;
const double P_FREEMODE_Y = 60;

const double P_LEVEL_X = 50;
const double P_LEVEL_Y = 70;

const double P_HIGHSCORE_X = 50;
const double P_HIGHSCORE_Y = 80;

const double P_QUIT_X = 50;
const double P_QUIT_Y = 90;

const double P_BACK_X = 50;
const double P_BACK_Y = 90;

bool gbRun = true;
void (*gDeleteCurrentState)() = NULL;

SDL_Renderer* gSdlRenderer = NULL;
SDL_Window* gSdlWindow = NULL;

TTF_Font * gMainFont = NULL;
TTF_Font * gMainFontBold = NULL;
TTF_Font * gSecondaryFont = NULL;
TTF_Font * gSecondaryFontBold = NULL;
TTF_Font * gTitleFont = NULL;

SDL_Texture * gtBACKGROUND = NULL;
SDL_Texture * gtPARTICLE = NULL;
SDL_Texture * gtCHARGE1 = NULL;
SDL_Texture * gtCHARGE2 = NULL;
SDL_Texture * gtCHARGE3 = NULL;
SDL_Texture * gtNEGCHARGE1 = NULL;
SDL_Texture * gtNEGCHARGE2 = NULL;
SDL_Texture * gtNEGCHARGE3 = NULL;
SDL_Texture * gtOVERLAYCHARGE = NULL;
SDL_Texture * gtDRAGCHARGE = NULL;
SDL_Texture * gtFLAG = NULL;
SDL_Texture * gtBEGIN = NULL;

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

/** \brief init a dynamic array */
void da_init(da_t * this);
/** \brief push a value into a dynamic array */
void da_push(da_t * this, void * elt);
/** \brief insert a value in the dynamic array */
void da_insert(da_t * this, void * elt, size_t pos);
/** \brief remove at element dynamic array \return pointer to the element removed */
void * da_removeat(da_t * this, void * elt);
/** \brief remove last element \return pointer to the element removed */
void * da_remove(da_t * this);

typedef enum EGameState
{
	EGameState_Unknown,
	EGameState_MainMenu,
	EGameState_ScoreMenu,
	EGameState_FreeGame,
	EGameState_LevelGame,
	EGameState_size
}EGameState;

/** \brief current state of the game */
EGameState gState = EGameState_MainMenu;

typedef enum EButtonFunction
{
	EButtonFunction_Unknown,
	EButtonFunction_none,
	EButtonFunction_free,
	EButtonFunction_score,
	EButtonFunction_level,
	EButtonFunction_quit,
	EButtonFunction_back,
	EButtonFunction_size
}EButtonFunction;

typedef enum EObjectType
{
	EObjectType_Unknown,
	EObjectType_Particle,
	EObjectType_ppp,
	EObjectType_pp,
	EObjectType_p,
	EObjectType_m,
	EObjectType_mm,
	EObjectType_mmm,
	EObjectType_flag,
	EObjectType_begin,
	EObjectType_Text,
	EObjectType_size,
}EObjectType;

typedef struct UBase
{
	/** \brief type of object */
	EObjectType s;
	/** \brief position in the x axis, between 0 and 100 */
	double x;
	/** \brief position in the x axis, between 0 and 100 */
	double y;
	/** \brief width of the sprite */
	double w;
	/** \brief height of the sprite */
	double h;
	/** \brief is the object currently overlay by the pointer (mouse) */
	bool bO;
	/** \brief can be moved or delete ? used in the level game mode /// level editor */
	bool bMoD;
}UBase_t;

typedef struct UTextBase
{
	UBase_t b;
	bool bTextButton;
	const char * t;
}UTextBase_t;

typedef struct UTextButton
{
	UTextBase_t txt;
	EButtonFunction f;
}UTextButton_t;

typedef union UText
{
	UTextBase_t t;
	UTextButton_t tb;
}UText_t;

typedef struct UCharge
{
	UBase_t b;
	/** \brief force that this charge apply in newton */
	double f;
}UCharge_t;

typedef struct UGE
{
	UBase_t b;
}UGE_t;

typedef struct UParticle
{
	UBase_t b;
	/** \brief speed of the particle x axis */
	double dx;
	/** \brief speed of the particle y axis */
	double dy;
}UParticle_t;

/** \brief object that can be displayed and have a x/y, may be overlay */
typedef union UObject
{
	UBase_t b;
	UCharge_t c;
	UGE_t ge;
	UParticle_t p;
	UText_t t;
}UObject_t;

/** \brief current drag object */
UBase_t * gDragObj = NULL;
/** \brief dynamic array of objects */
da_t da_obj;
/** \brief MainParticle */
UParticle_t * gMp = NULL;

//________________DISPLAY FUNCTION______________________

/** \brief Enum object to texture \return the pointer to display an object from his type */
SDL_Texture * eotot(EObjectType e);
/** \brief name to texture */
SDL_Texture * ntt(const char * name);

//________________UOBJECT FUNCTION______________________

//____UBASE
/** \brief render a Ubase object */
void ub_render(UBase_t * this);

//____UCHARGE
/** \brief init a charge with p charge */
void ch_init(UCharge_t * this, const double x, const double y);

//____UBUTTON
void utb_click(UTextButton_t * this);

void menu_init();
void menu_destroy();

void freemode_init();
void freemode_destroy();

SDL_Texture * eotot(EObjectType e)
{
	switch (e)
	{
	case EObjectType_Particle:return gtPARTICLE;
	case EObjectType_ppp:return gtCHARGE3;
	case EObjectType_pp:return gtCHARGE2;
	case EObjectType_p: return gtCHARGE1;
	case EObjectType_m: return gtNEGCHARGE1;
	case EObjectType_mm: return gtNEGCHARGE2;
	case EObjectType_mmm: return gtNEGCHARGE3;
	case EObjectType_flag: return gtFLAG;
	case EObjectType_begin: return gtBEGIN;
	case EObjectType_Unknown:
	case EObjectType_size:
	default:;
	}
	return NULL;
}

double pxtod(int px, bool xAxis)
{
	return ((double)px * 100.f) / (xAxis ? (double)gCurrentWidth : (double)gCurrentHeight);
}

int dtopx(double p, bool xAxis)
{
	double d = xAxis ? (double)gCurrentWidth : (double)gCurrentHeight;
	return (p*(d)/100.f);
}

void ub_init(UBase_t * this, const double x, const double y, const double h, const double w, const  EObjectType e)
{
	if (this)
	{
		this->bO = 0;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->s = e;
	}
}

void ch_init(UCharge_t * this, const double x, const double y)
{
	ub_init((UBase_t *)this, x, y, pxtod(32, false), pxtod(32, true), EObjectType_p);
	if(this)
		this->f = 0;
}

bool objTExist(EObjectType type)
{
	da_elm_t * it = da_obj.first;
	while (it)
	{
		if (it->elt && ((UBase_t *)it->elt)->s == type) return true;
		it = it->next;
	}
	return false;
}

void uge_create(const double x, const double y, EObjectType type)
{
	if (type != EObjectType_begin && type != EObjectType_flag) return;
	
	if(objTExist(type)) type = type == EObjectType_begin ? EObjectType_flag : EObjectType_begin;
	if (objTExist(type)) return;

	UGE_t * e = malloc(sizeof(UGE_t));
	ub_init((UBase_t *)e, x, y, pxtod(32, false), pxtod(32, true), type);
	da_push(&da_obj, e);
	printf("create uge\n");
}

void ub_render(UBase_t * this)
{
	SDL_Rect dest;

	if (this->x >= 0 && this->y >= 0 && this->x <= 100 && this->y <= 100)
	{
		dest.w = dtopx(this->w, true);
		dest.h = dtopx(this->h, false);
		dest.x = dtopx(this->x - this->w*0.5f, true);
		dest.y = dtopx(this->y - this->h*0.5f, false);

		if(gDragObj == this) 
			SDL_RenderCopy(gSdlRenderer, gtDRAGCHARGE, NULL, &dest);
		else if(this->bO)
			SDL_RenderCopy(gSdlRenderer, gtOVERLAYCHARGE, NULL, &dest);

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

SDL_Texture * ntt(const char * name)
{
	SDL_Texture * texture = NULL;
	SDL_Surface * loadedImage = IMG_Load(name);

	if (loadedImage != NULL)
	{
		texture = SDL_CreateTextureFromSurface(gSdlRenderer, loadedImage);
		SDL_FreeSurface(loadedImage);
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
	gMp = malloc(sizeof(UParticle_t));
	da_push(&da_obj, gMp);
	ub_init((UBase_t*)gMp, 50, 50, pxtod(32, false), pxtod(32, true), EObjectType_Particle);
	gMp->dx = 0;
	gMp->dy = 0;
}

void br_init()
{
	gtBACKGROUND = ntt(N_TEXTURE_BACKGROUND);
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

void m_clamp60fps(unsigned int LastUpdateTick)
{
	uint32_t ticks = SDL_GetTicks();

	if (LastUpdateTick < ticks)
		return;

	if (ticks - LastUpdateTick <= 20)
		SDL_Delay(20 - (ticks - LastUpdateTick));
}

bool ub_over(UBase_t * elt, double x, double y)
{
	double dx = elt->w*0.5f;
	double dy = elt->h*0.5f;
	if(elt)
	{
		if (fabs(elt->x - x) <= dx &&fabs(elt->y - y) <= dy)
			return true;
	}
	return false;
}

UBase_t* findOverlayObj(int x, int y)
{
	da_elm_t * it = da_obj.first;
	UBase_t* res = NULL;
	while(it)
	{
		if (ub_over((UBase_t*)it->elt, x * 100 / (double)gCurrentWidth, y * 100 / (double)gCurrentHeight))
			res =(UBase_t*)it->elt;
		it = it->next;
	}
	return res;
}

void ch_create(int x, int y)
{
	UCharge_t * c = malloc(sizeof(UCharge_t));
	da_push(&da_obj, c);
	ch_init(c, pxtod(x, true) , pxtod(y, false));
}

void SDL_HandleLeftClick(bool bUp, int x, int y)
{
	UBase_t * obj = findOverlayObj(x, y);
	
	switch (gState)
	{
	case EGameState_ScoreMenu:
	case EGameState_MainMenu:
		if (bUp && obj && obj->s == EObjectType_Text && ((UTextBase_t *)obj)->bTextButton)
			utb_click((UTextButton_t*)obj);
		break;
	case EGameState_FreeGame: 
		if (bUp)
		{
			if (gDragObj)
			{
				gDragObj = NULL;
			}
			else
			{
				ch_create(x, y);
			}
		}
		else
		{
			
			if (obj)
			{
				gDragObj = obj;
			}
		}
		break;
	case EGameState_LevelGame: break;
	default: ;
	}
	
}

void SDL_HandleRightClick(bool bUp, int x, int y)
{
	UBase_t * obj = findOverlayObj(x, y);
	switch (gState)
	{
	case EGameState_Unknown: break;
	case EGameState_MainMenu: break;
	case EGameState_ScoreMenu: break;
	case EGameState_FreeGame:
		if (bUp)
		{	
			if (obj == NULL)
			{
				uge_create(pxtod(x, true), pxtod(y, false), EObjectType_begin);
			}
			else
			{
				EObjectType e = obj ? ((UBase_t*)obj)->s : EObjectType_Unknown;
				if (e >= EObjectType_ppp && e <= EObjectType_begin)
				{
					void * res = da_removeat(&da_obj, obj);
					if (res == obj) free(obj);
					else fprintf(stderr, "Error removing charge\n");
				}
			}		
		}
		break;
	case EGameState_LevelGame: break;
	case EGameState_size: break;
	default: ;
	}	
}

void SDL_HandleMiddleClick(bool bUp, int x, int y)
{
	UBase_t * c = NULL;
	switch (gState)
	{
	case EGameState_Unknown: break;
	case EGameState_MainMenu: break;
	case EGameState_ScoreMenu: break;
	case EGameState_FreeGame: 
		c = findOverlayObj(x, y);
		EObjectType e = c ? ((UBase_t*)c)->s : EObjectType_Unknown;
		if (c && e >= EObjectType_ppp && e <= EObjectType_mmm)
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
		if (c && e >= EObjectType_flag && e <= EObjectType_begin)
		{
			if(objTExist((e == EObjectType_flag) ? EObjectType_begin : EObjectType_flag))
			{
				da_elm_t * it = da_obj.first;
				while(it)
				{
					if(it->elt && (((UBase_t *)it->elt)->s == ((e == EObjectType_flag) ? EObjectType_begin : EObjectType_flag)))
					{
						((UBase_t *)it->elt)->s = e;
						break;
					}					
					it = it->next;
				}			
			}
			((UBase_t*)c)->s = (e == EObjectType_flag) ? EObjectType_begin : EObjectType_flag;
		}
		break;
	case EGameState_LevelGame: break;
	case EGameState_size: break;
	default: ;
	}
}

SDL_Texture * loadFromRenderedText(char * textureText, SDL_Color textColor, TTF_Font * f, int * w, int * h)
{
	if (!(w && h)) return NULL;
	//Get rid of preexisting texture
	//free();
	SDL_Texture * res = NULL;
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended(f, textureText, textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	res = SDL_CreateTextureFromSurface(gSdlRenderer, textSurface);
	if (res == NULL)
	{
		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		//Get image dimensions
		*w = textSurface->w;
		*h = textSurface->h;
	}

	//Get rid of old surface
	SDL_FreeSurface(textSurface);

	//Return success
	return res;
}
UTextButton_t * utb_create(const double x, const double y, EButtonFunction f)
{
	UTextButton_t * res = malloc(sizeof(UTextButton_t));
	if (!res) printf("Fail to allocate memory for utb\n");
	else
	{
		ub_init((UBase_t*)res, x, y, 0, 0, EObjectType_Text);
		
		res->txt.bTextButton = true;
		res->f = f;
		switch(f)
		{
		case EButtonFunction_free: res->txt.t = M_FREEMODE; break;
		case EButtonFunction_score: res->txt.t = M_HIGHSCORE; break;
		case EButtonFunction_level: res->txt.t = M_LEVEL; break;
		case EButtonFunction_quit: res->txt.t = M_QUIT; break;
		case EButtonFunction_back: res->txt.t = M_BACK; break;
		case EButtonFunction_Unknown:
		case EButtonFunction_none:
		case EButtonFunction_size:
		default: res->txt.t = NULL;
		}
		da_push(&da_obj, res);
	}
	return res;
}

void utb_click(UTextButton_t * this)
{
	switch (this->f)
	{
	case EButtonFunction_Unknown: break;
	case EButtonFunction_none: break;
	case EButtonFunction_free: freemode_init(); break;
	case EButtonFunction_score: break;
	case EButtonFunction_level: break;
	case EButtonFunction_quit: gbRun = false; break;
	case EButtonFunction_back: break;
	case EButtonFunction_size: break;
	default:;
	}
}

UTextBase_t * ut_create(const double x, const double y, const char * t)
{
	UTextBase_t * res = malloc(sizeof(UTextButton_t));
	if (!res) printf("Fail to allocate memory for utb\n");
	else
	{
		ub_init((UBase_t*)res, x, y, 0, 0, EObjectType_Text);
		res->bTextButton = false;
		res->t = t;

		da_push(&da_obj, res);
	}
	return res;
}

/** \brief create all the object for the menu */
void menu_init()
{
	if (gDeleteCurrentState) gDeleteCurrentState();
	printf("create menu \n");
	gState = EGameState_MainMenu;
	ut_create(P_GAMETILE_X, P_GAMETILE_Y, M_GAMETITLE);
	utb_create(P_FREEMODE_X, P_FREEMODE_Y, EButtonFunction_free);
	utb_create(P_LEVEL_X, P_LEVEL_Y, EButtonFunction_level);
	utb_create(P_HIGHSCORE_X, P_HIGHSCORE_Y, EButtonFunction_score);
	utb_create(P_QUIT_X, P_QUIT_Y, EButtonFunction_quit);

	gDeleteCurrentState = &menu_destroy;
}

/** \brief destroy the menu */
void menu_destroy()
{
	UObject_t * obj = NULL;
	while((obj = (UObject_t *)da_remove(&da_obj))) 
		free(obj);
	gState = EGameState_Unknown;
}

void freemode_init()
{
	if (gDeleteCurrentState) gDeleteCurrentState();
	gState = EGameState_FreeGame;
	p_init();
	gDeleteCurrentState = &freemode_destroy;
}

void freemode_destroy()
{
	UObject_t * obj = NULL;
	while ((obj = (UObject_t *)da_remove(&da_obj)))
		free(obj);
	gMp = NULL;
	gState = EGameState_Unknown;
}

void m_init()
{
//__________________SDL MODULE INIT__________________
	SDL_Init(SDL_INIT_EVERYTHING);
	gSdlWindow = SDL_CreateWindow(GAME_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!gSdlWindow) fprintf(stderr, "Can't create gSdlWindow\n");

	gSdlRenderer = SDL_CreateRenderer(gSdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!gSdlRenderer) fprintf(stderr, "Can't create gSdlRenderer\n");

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		fprintf(stderr, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
	if (TTF_Init() == -1)
		fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

//__________________OPEN FONT__________________
	gMainFontBold = TTF_OpenFont("./fonts/Roboto-Black.ttf", 48);
	if (gMainFontBold == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gMainFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 48);
	if (gMainFont == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gSecondaryFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 28);
	if (gSecondaryFont == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gSecondaryFontBold = TTF_OpenFont("./fonts/Roboto-Black.ttf", 28);
	if (gSecondaryFontBold == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gTitleFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 70);
	if (gSecondaryFont == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());

//__________________OPEN IMAGES__________________
	gtPARTICLE = ntt(N_TEXTURE_PARTICLE);
	gtCHARGE1 = ntt(N_TEXTURE_CHARGE);
	gtCHARGE2 = ntt(N_TEXTURE_CHARGE2);
	gtCHARGE3 = ntt(N_TEXTURE_CHARGE3);
	gtNEGCHARGE1 = ntt(N_TEXTURE_NEGCHARGE);
	gtNEGCHARGE2 = ntt(N_TEXTURE_NEGCHARGE2);
	gtNEGCHARGE3 = ntt(N_TEXTURE_NEGCHARGE3);
	gtDRAGCHARGE = ntt(N_TEXTURE_DRAGCHARGE);
	gtOVERLAYCHARGE = ntt(N_TEXTURE_SELCHARGE);
	gtFLAG = ntt(N_TEXTURE_FLAG);
	gtBEGIN = ntt(N_TEXTURE_BEGIN);

	da_init(&da_obj); //init array of obj
	br_init(); //init background
	//p_init();
	menu_init();
}

void clearHighlightf()
{
	da_elm_t * it = da_obj.first;
	while(it)
	{
		if (it->elt)
			((UBase_t *)it->elt)->bO = false;
		it = it->next;
	}
}

void m_poll()
{
	// event handling
	SDL_Event e;
	while (SDL_PollEvent(&e)) //Poll every input
	{
		if (e.type == SDL_QUIT) gbRun = false;
			//break; 
		else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
		{
			switch(gState)
			{
			case EGameState_Unknown: break;
			case EGameState_MainMenu: gbRun = false; break;
			case EGameState_ScoreMenu: break;
			case EGameState_FreeGame: 
				menu_init(); break;
			case EGameState_LevelGame: break;
			case EGameState_size: break;
			default: ;
			}
			
			
		}
			//break;
			//If mouse event happened
		if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
		{
			//Get mouse position
			int x, y;
			SDL_GetMouseState(&x, &y);
			//printf("mouse click : %d %d\n", x, y);

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
			clearHighlightf();
			int x, y;
			SDL_GetMouseState(&x, &y);
			if(gDragObj)
			{
				gDragObj->x = x * 100 / (double)gCurrentWidth;
				gDragObj->y = y * 100 / (double)gCurrentHeight;
			}
			else
			{
				UBase_t * b = findOverlayObj(x, y);
				if(b)
				{
					b->bO = true;
				}
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

/** \brief force to acceleration */
bool ftoa(UCharge_t * c, double* fx, double* fy, double x, double y)
{
	if (!(fx && fy)) return false;
	double f = getffeo(c->b.s);
	double xd = x - c->b.x;
	double yd = y - c->b.y;
	double d = sqrt(xd*xd + yd*yd);
	f = (d > 5.f) ? f / d : 0.f;
	*fx += f * (d ? xd / d : 0);
	*fy += f *  (d ? yd / d : 0);
	return true;
}

void m_update(uint32_t delta)
{
	if (!delta) return;
	if(gMp)
	{
		if (gDragObj == (UBase_t*)gMp)
		{
			static double lastx = 0;
			static double lasty = 0;
			gMp->dx = (gMp->b.x - lastx) / delta;
			gMp->dy = (gMp->b.y - lasty) / delta;
			clamp(&gMp->dx, -MAX_SPEED, MAX_SPEED);
			clamp(&gMp->dy, -MAX_SPEED, MAX_SPEED);
			lastx = gMp->b.x;
			lasty = gMp->b.y;
			return;
		}
		da_elm_t * it = da_obj.first;
		while (it)
		{
			double fx = 0;
			double fy = 0;
			double lastx = gMp->b.x;
			double lasty = gMp->b.y;

			EObjectType e = it->elt ? ((UBase_t*)it->elt)->s : EObjectType_Unknown;
			if (e >= EObjectType_ppp && e <= EObjectType_mmm)
				ftoa((UCharge_t*)it->elt, &fx, &fy, gMp->b.x, gMp->b.y);

			fx -= gMp->dx*AIR_FRICTION;
			fy -= gMp->dy*AIR_FRICTION;

			fx *= PARTICLE_WEIGHT_CONST;
			fy *= PARTICLE_WEIGHT_CONST;

			clamp(&fx, -MAX_ACCELERATION, MAX_ACCELERATION);
			clamp(&fy, -MAX_ACCELERATION, MAX_ACCELERATION);

			gMp->b.x += gMp->dx*delta + fx*delta*delta;
			gMp->b.y += gMp->dy*delta + fy*delta*delta;

			clamp(&gMp->b.x, 0.f, 100.f);
			clamp(&gMp->b.y, 0.f, 100.f);


			gMp->dx = (gMp->b.x - lastx) / delta;
			gMp->dy = (gMp->b.y - lasty) / delta;

			clamp(&gMp->dx, -MAX_SPEED, MAX_SPEED);
			clamp(&gMp->dy, -MAX_SPEED, MAX_SPEED);

			it = it->next;
		}
	}
}

void da_obj_order()
{
	da_elm_t * tnn = NULL;
	da_elm_t * tn = NULL;
	da_elm_t * tp = NULL;
	da_elm_t * it = NULL;
	da_elm_t * it2 = NULL;
	bool bChange = true;
	while(bChange) //bubble sort
	{
		if (da_obj.size == 2)
			bChange = true;
		it = da_obj.first;
		bChange = false; 
		while (it)
		{
			if (it->elt && it->next	&& it->next->elt && ((UBase_t *)it->elt)->y > ((UBase_t *)it->next->elt)->y)
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

void ut_render(UText_t* this)
{
	if (this == NULL) return;
	TTF_Font * f = NULL;
	if(this->t.bTextButton)
		f = this->tb.txt.b.bO ? gMainFontBold : gMainFont;
	else
		f = gSecondaryFont;
	if (f)
	{
		int w, h;
		SDL_Color textColor = { 200, 200, 200 };
		SDL_Texture * t;
		if ((t = loadFromRenderedText((char *)this->t.t, textColor, f, &w, &h)))
		{
			SDL_Rect dest;

			this->t.b.w = pxtod(w, true);
			this->t.b.h = pxtod(h, false);
			dest.w = w;
			dest.h = h;
			dest.x = dtopx(this->t.b.x - this->t.b.w*0.5f, true);
			dest.y = dtopx(this->t.b.y - this->t.b.h*0.5f, false);

			SDL_RenderCopy(gSdlRenderer, t, NULL, &dest);
			SDL_DestroyTexture(t);
		}
		if (this->t.bTextButton && this->tb.txt.b.bO)
		{
			SDL_Rect dest;
			dest.w = 32;
			dest.h = 32;
			dest.x = dtopx(this->t.b.x - this->t.b.w*0.5f, true) - (int)(1.2f * dest.w);
			dest.y = dtopx(this->t.b.y - this->t.b.h*0.5f , false) - -(int)(0.5 * dest.h);
			SDL_RenderCopy(gSdlRenderer, gtPARTICLE, NULL, &dest);
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
			switch (((UBase_t*)it->elt)->s)
			{
			case EObjectType_Unknown: break;
			case EObjectType_Particle: 
			case EObjectType_ppp: 
			case EObjectType_pp: 
			case EObjectType_p: 
			case EObjectType_m: 
			case EObjectType_mm: 
			case EObjectType_mmm:
			case EObjectType_flag:
			case EObjectType_begin:
				ub_render((UBase_t*)it->elt);
				break;
			case EObjectType_Text:
				ut_render((UText_t *)it->elt);
				break;
			case EObjectType_size:
			default: ;
				printf("error don't know how to render this object\n");
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
	uint32_t uilt = SDL_GetTicks() + 16;
	m_init();

	while (gbRun)
	{
		uint32_t uict = SDL_GetTicks();
		m_poll();
		m_update(uict - uilt);
		m_render();

		m_clamp60fps(uict);
		uilt = uict;
	}

	m_clear();

	return 0;
}