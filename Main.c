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
const char N_TEXTURE_BACKGROUNDW[] = "./images/backgroundw.png";
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
const char N_TEXTURE_END[] = "./images/end.png";
const char N_TEXTURE_BACK[] = "./images/back.png";
const char N_TEXTURE_BACKB[] = "./images/backb.png";
const char N_TEXTURE_PAUSE[] = "./images/pause.png";
const char N_TEXTURE_PAUSEB[] = "./images/pauseb.png";
const char N_TEXTURE_PLAY[] = "./images/play.png";
const char N_TEXTURE_PLAYB[] = "./images/playb.png";
const char N_TEXTURE_REFRESH[] = "./images/refresh.png";
const char N_TEXTURE_REFRESHB[] = "./images/refreshb.png";
const char N_TEXTURE_CLEAR[] = "./images/cross.png";
const char N_TEXTURE_CLEARB[] = "./images/crossb.png";

const char F_MAP_NAME[] = "./mainmap.dat";
const char F_MENUMAP_NAME[] = "./menumap.dat";
const char F_SCORE_NAME[] = "./scores.dat";

#define MAX_SCORES 3
uint32_t gScores[MAX_SCORES];

const double MAX_ACCELERATION = 0.00005f;
const double MAX_SPEED = 0.1f;
const double PARTICLE_WEIGHT_CONST = 0.005f;
const double AIR_FRICTION = 0.001f;

const char M_GAMETITLE[] = "Charge Game";
const char M_FREEMODE[] = "FreeMode";
const char M_LEVEL[] = "Level";
const char M_HIGHSCORE[] = "Highscore";
const char M_TRY[] = "Try : %d";
const char M_WIN[] = "You win !";
const char M_PRESSESCAPE[] = "Press escape to come back !";
const char M_SUCEED[] = "You suceed in %d try";
const char M_PLACEINSCORE[] = "You are %d in highscore";
const char M_QUIT[] = "Quit";

const double P_GAMETILE_X = 50;
const double P_GAMETILE_Y = 30;

const double P_FREEMODE_X = 50;
const double P_FREEMODE_Y = 60;

const double P_LEVEL_X = 50;
const double P_LEVEL_Y = 70;

const double P_HIGHSCORE_X = 50;
const double P_HIGHSCORE_Y = 80;

const double P_QUIT_X = 50;
const double P_QUIT_Y = 90;

const double P_BACK_X = 5;
const double P_BACK_Y = 8;

const double P_PAUSEPLAY_X = 95;
const double P_PAUSEPLAY_Y = 93;

const double P_CLEAR_X = 95;
const double P_CLEAR_Y = 8;

/** \brief is the application running, set to false to stop the main loop */
bool gbRun = true;
/** \brief function pointer to destroy current game state when the state is changed */
void (*gDeleteCurrentState)() = NULL;

/** \brief the main window, init in m_init() */
SDL_Window* gSdlWindow = NULL;
/** \brief the sdl renderer that render every texture, init in m_init( */
SDL_Renderer* gSdlRenderer = NULL;

/** \brief pointer onto the main font */
TTF_Font* gMainFont = NULL;
/** \brief pointer onto the main font bold */
TTF_Font* gMainFontBold = NULL;
/** \brief pointer onto the secondary font */
TTF_Font* gSecondaryFont = NULL;
/** \brief pointer onto the main font bold */
TTF_Font* gSecondaryFontBold = NULL;
/** \brief pointer onto the title font */
TTF_Font* gTitleFont = NULL;

/** \brief Background SDL texture pointer */
SDL_Texture* gtBACKGROUND = NULL;
/** \brief Background win SDL texture pointer */
SDL_Texture* gtBACKGROUNDW = NULL;
/** \brief Particle image SDL texture pointer */
SDL_Texture* gtPARTICLE = NULL;
/** \brief charge + image SDL texture pointer */
SDL_Texture* gtCHARGE1 = NULL;
/** \brief charge ++ image SDL texture pointer */
SDL_Texture* gtCHARGE2 = NULL;
/** \brief charge +++ image SDL texture pointer */
SDL_Texture* gtCHARGE3 = NULL;
/** \brief charge - image SDL texture pointer */
SDL_Texture* gtNEGCHARGE1 = NULL;
/** \brief charge -- image SDL texture pointer */
SDL_Texture* gtNEGCHARGE2 = NULL;
/** \brief charge --- image SDL texture pointer */
SDL_Texture* gtNEGCHARGE3 = NULL;
/** \brief image draw to overlay a particle/charge/flag when mouse is on it image SDL texture pointer */
SDL_Texture* gtOVERLAYCHARGE = NULL;
/** \brief image draw to overlay a particle/charge/flag when drag and drap image SDL texture pointer */
SDL_Texture* gtDRAGCHARGE = NULL;
/** \brief flag image SDL texture pointer */
SDL_Texture* gtFLAG = NULL;
/** \brief not used in this version, may be used to draw wall begin */
SDL_Texture* gtBEGIN = NULL;
/** \brief not used in this version, may be used to draw wall end */
SDL_Texture* gtEND = NULL;
/** \brief pause button texture */
SDL_Texture* gtPAUSE = NULL;
/** \brief pause button bold texture */
SDL_Texture* gtPAUSEB = NULL;
/** \brief back texture */
SDL_Texture* gtBACK = NULL;
/** \brief back bold texture */
SDL_Texture* gtBACKB = NULL;
/** \brief play texture */
SDL_Texture* gtPLAY = NULL;
/** \brief play bold texture */
SDL_Texture* gtPLAYB = NULL;
/** \brief refresh texture */
SDL_Texture* gtREFRESH = NULL;
/** \brief refresh bold texture */
SDL_Texture* gtREFRESHB = NULL;
/** \brief clear texture */
SDL_Texture* gtCLEAR = NULL;
/** \brief clear bold texture */
SDL_Texture* gtCLEARB = NULL;

typedef struct da_elm_t
{
	struct da_elm_t* next;
	void* elt;
} da_elm_t;

typedef struct da_t
{
	struct da_elm_t* first;
	size_t size;
} da_t;

/** \brief init a dynamic array */
void da_init(da_t* this);
/** \brief push a value into a dynamic array */
void da_push(da_t* this, void* elt);
/** \brief remove at element dynamic array \return pointer to the element removed */
void* da_removeat(da_t* this, void* elt);
/** \brief remove last element \return pointer to the element removed */
void* da_remove(da_t* this);

typedef enum EGameState
{
	EGameState_Unknown,
	EGameState_MainMenu,
	EGameState_ScoreMenu,
	EGameState_FreeGame,
	EGameState_LevelGame,
	EGameState_Win,
	EGameState_size,
} EGameState;

/** \brief current state of the game */
EGameState gState = EGameState_MainMenu;

typedef enum ETextButtonFunction
{
	ETextButtonFunction_Unknown,
	ETextButtonFunction_none,
	ETextButtonFunction_free,
	ETextButtonFunction_score,
	ETextButtonFunction_level,
	ETextButtonFunction_quit,
	ETextButtonFunction_size
} ETextButtonFunction;

typedef enum EButtonFunction
{
	EButtonFunction_Unknown,
	EButtonFunction_Pause,
	EButtonFunction_Play,
	EButtonFunction_Back,
	EButtonFunction_Clear,
	EButtonFunction_Retry,
	EButtonFunction_Size,
} EButtonFunction;

typedef enum EObjectType
{
	EObjectType_Unknown,
	EObjectType_Particle,
	EObjectType_Charge,
	EObjectType_flag,
	EObjectType_begin,
	EObjectType_end,
	EObjectType_Text,
	EObjectType_Button,
	EObjectType_size,
} EObjectType;

typedef enum EChargeStrength
{
	EChargeStrength_ppp,
	EChargeStrength_pp,
	EChargeStrength_p,
	EChargeStrength_m,
	EChargeStrength_mm,
	EChargeStrength_mmm,
}EChargeStrength;

typedef struct UBase
{
	/** \brief type of object */
	EObjectType eot;
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
} UBase_t;

typedef struct UTextBase
{
	UBase_t b;
	bool bTextButton;
	bool bBig;
	const char* t;
} UTextBase_t;

typedef struct UTextButton
{
	UTextBase_t txt;
	EButtonFunction f;
} UTextButton_t;

typedef union UText
{
	UTextBase_t t;
	UTextButton_t tb;
} UText_t;

typedef struct UCharge
{
	UBase_t b;
	EChargeStrength f;
} UCharge_t;

typedef struct UGE
{
	UBase_t b;
} UFlag_t;

typedef struct UParticle
{
	UBase_t b;
	/** \brief speed of the particle x axis */
	double dx;
	/** \brief speed of the particle y axis */
	double dy;
} UParticle_t;

typedef struct UButton
{
	UBase_t b;
	EButtonFunction f;
} UButton_t;

/** \brief object that can be displayed and have a x/y, may be overlay */
typedef union UObject
{
	UBase_t b;
	UCharge_t c;
	UFlag_t ge;
	UParticle_t p;
	UText_t t;
	UButton_t bt;
} UObject_t;

/** \brief current drag object */
UBase_t* gDragObj = NULL;
/** \brief dynamic array of objects */
da_t da_obj;
/** \brief return the main particle inside the da_obj array */
UParticle_t* getMainParticle();
bool bGameFreeze = false;
double gMPPosx = 50.f;
double gMPPosy = 50.f;
uint32_t gTrynb = 0;

//________________DISPLAY FUNCTION______________________

/** \brief name to texture */
SDL_Texture* ntt(const char* name);

//________________UOBJECT FUNCTION______________________

//____UBASE
/** \brief render a Ubase object */
void ub_render(UBase_t* this);

//____UCHARGE

//____UBUTTON
void utb_click(UTextButton_t* this);

void menu_init();
void freemode_init(bool bLoadMap);

void saveMap(const char * filename);
void loadMap(const char * filename);
void score_init();

UParticle_t* getMainParticle()
{
	da_elm_t * it = da_obj.first;
	while(it)
	{
		if (it->elt && ((UBase_t *)it->elt)->eot == EObjectType_Particle) 
			return (UParticle_t *)it->elt;
		it = it->next;
	}
	return NULL;
}

SDL_Texture* ecstot(UCharge_t * c)
{
	if (!c) return NULL;
	switch(c->f)
	{
	case EChargeStrength_ppp: return gtCHARGE3;
	case EChargeStrength_pp: return gtCHARGE2;
	case EChargeStrength_p: return gtCHARGE1;
	case EChargeStrength_m: return gtNEGCHARGE1;
	case EChargeStrength_mm: return gtNEGCHARGE2;
	case EChargeStrength_mmm: return gtNEGCHARGE3;
	default: ;
	}
	return NULL;
}

SDL_Texture* ebtot(UButton_t* this)
{
	if (!this) return NULL;
	switch (this->f)
	{
	case EButtonFunction_Pause: return this->b.bO ? gtPAUSEB : gtPAUSE;
	case EButtonFunction_Play: return this->b.bO ? gtPLAYB : gtPLAY;
	case EButtonFunction_Back: return this->b.bO ? gtBACKB : gtBACK;
	case EButtonFunction_Clear: return this->b.bO ? gtCLEARB : gtCLEAR;
	case EButtonFunction_Retry: return this->b.bO ? gtREFRESHB : gtREFRESH;
	default:;
	}
	return NULL;
}

SDL_Texture* eotot(UBase_t * b)
{
	if (!b) return NULL;
	switch (b->eot)
	{
	case EObjectType_Particle: return gtPARTICLE;
	case EObjectType_Charge: return ecstot((UCharge_t *)b);
	case EObjectType_flag: return gtFLAG;
	case EObjectType_begin: return gtBEGIN;
	case EObjectType_end: return gtEND;
	case EObjectType_Button: return ebtot((UButton_t *)b);
	default: ;
	}
	return NULL;
}

double pxtod(const int px, const bool xAxis)
{
	return ((double)px * 100.f) / (xAxis ? (double)gCurrentWidth : (double)gCurrentHeight);
}

int dtopx(const double p, const bool xAxis)
{
	return (int)(p * (xAxis ? (double)gCurrentWidth : (double)gCurrentHeight) / 100.f);
}

void obj_destroy()
{
	UObject_t* obj = NULL;
	while ((obj = (UObject_t *)da_remove(&da_obj)))
		free(obj);
}

void ub_init(UBase_t* this, const double x, const double y, const double h, const double w, const EObjectType e)
{
	if (this)
	{
		this->bO = 0;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->eot = e;
	}
}

void loadScores()
{
	FILE* fp = NULL;
	memset(gScores, 0, sizeof(uint32_t)*MAX_SCORES);
	fp = fopen(F_SCORE_NAME, "rb");
	if (!fp)
	{
		fprintf(stderr, "fail to open %s file in rb mode\n", F_SCORE_NAME);
		return;
	}
	fread(gScores, sizeof(uint32_t), MAX_SCORES, fp);
	fclose(fp);
}

void saveScores(uint32_t value)
{
	size_t i = 0;
	size_t j = 0;
	FILE* fp = NULL;
	loadScores();
	for (i; i < MAX_SCORES; ++i) if (value < gScores[i] || gScores[i] == 0) break;
	for (j = MAX_SCORES - 1; j> i; --j)
		gScores[j] = gScores[j - 1];
	gScores[i] = value;

	fp = fopen(F_SCORE_NAME, "wb");
	if (!fp)
	{
		fprintf(stderr, "fail to open %s file in wb mode\n", F_SCORE_NAME);
		return;
	}
	fwrite(gScores, sizeof(uint32_t), MAX_SCORES, fp);
	fclose(fp);
}

void ch_init(UCharge_t* this, const double x, const double y, EChargeStrength cs)
{
	ub_init((UBase_t *)this, x, y, pxtod(32, false), pxtod(32, true), EObjectType_Charge);
	if (this)
		this->f = cs;
}

void ubt_create(double x, double y, EButtonFunction fct)
{
	UButton_t * ubt = malloc(sizeof(UButton_t));
	if (ubt)
	{
		ub_init((UBase_t *)ubt, x, y, pxtod(32, false), pxtod(32, true), EObjectType_Button);
		ubt->f = fct;
		ubt->b.bMoD = false;
		da_push(&da_obj, ubt);
	}
	else
		fprintf(stderr, "failed to allocation memory for a button\n");
}

bool objTExist(const EObjectType type)
{
	da_elm_t* it = da_obj.first;
	while (it)
	{
		if (it->elt && ((UBase_t *)it->elt)->eot == type) return true;
		it = it->next;
	}
	return false;
}

void uge_create(const double x, const double y, EObjectType type)
{
	//if (type != EObjectType_begin && type != EObjectType_flag) return;

	//if (objTExist(type)) type = type == EObjectType_begin ? EObjectType_flag : EObjectType_begin;
	if (objTExist(type)) return;

	UFlag_t* e = malloc(sizeof(UFlag_t));
	ub_init((UBase_t *)e, x, y, pxtod(32, false), pxtod(32, true), type);
	da_push(&da_obj, e);
	printf("create uge\n");
}

void ub_render(UBase_t* this)
{
	SDL_Rect dest;

	//printf(">render\n");

	if (this->x >= 0 && this->y >= 0 && this->x <= 100 && this->y <= 100)
	{
		dest.w = 32;//dtopx(this->w, true);
		dest.h = 32;//dtopx(this->h, false);
		dest.x = dtopx(this->x - this->w * 0.5f, true);
		dest.y = dtopx(this->y - this->h * 0.5f, false);

		if(this->eot >= EObjectType_Particle && this->eot <= EObjectType_begin)
		{
			if (gDragObj == this)
				SDL_RenderCopy(gSdlRenderer, gtDRAGCHARGE, NULL, &dest);
			else if (this->bO && ((gState != EGameState_LevelGame) || this->bMoD) && gState != EGameState_Win)
				SDL_RenderCopy(gSdlRenderer, gtOVERLAYCHARGE, NULL, &dest);
		}	
		SDL_RenderCopy(gSdlRenderer, eotot(this), NULL, &dest);
	}
}

void da_init(da_t* this)
{
	if (this) memset(this, 0, sizeof(da_t));
}

void da_push(da_t* this, void* elt)
{
	if (!this || !elt) return;
	da_elm_t* nElm = malloc(sizeof(da_elm_t));
	memset(nElm, 0, sizeof(da_elm_t));
	nElm->elt = elt;

	if (this->first == NULL)
		this->first = nElm;
	else
	{
		da_elm_t* it = this->first;
		while (it->next) it = it->next;
		it->next = nElm;
	}
	++this->size;
}

void da_insert(da_t* this, void* elt, const size_t pos)
{
	if (!(pos >= 0 && pos < this->size)) return; //outofbound
	da_elm_t* nElm = malloc(sizeof(da_elm_t));
	nElm->elt = elt;
	memset(nElm, 0, sizeof(da_elm_t));
	if (pos == 0)
	{
		nElm->next = this->first;
		this->first = nElm;
	}
	else
	{
		da_elm_t* it = this->first;
		size_t i = 0;
		while (it)
		{
			if (i == pos)
			{
				da_elm_t* tmp = it->next;
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

void* da_removeat(da_t* this, const void* elt)
{
	void* eltRs;
	if (this)
	{
		da_elm_t* it = this->first;
		while (it)
		{
			if (it->elt == elt)
			{
				if (it == this->first)
				{
					this->first = this->first->next;
					eltRs = it->elt;
					free(it);
					--this->size;
					return eltRs;
				}
				da_elm_t* it2 = this->first;
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

void* da_remove(da_t* this)
{
	if (this)
	{
		da_elm_t* it = this->first;
		while (it && it->next) it = it->next;
		return it ? da_removeat(this, it->elt) : NULL;
	}
	return NULL;
}

SDL_Texture* ntt(const char* name)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* loadedImage = IMG_Load(name);

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

void p_init(const double x, const double y, const double dx, const double dy)
{
	UParticle_t* mp = getMainParticle();
	if (!mp)
	{
		mp = malloc(sizeof(UParticle_t));
		if(!mp)
		{
			fprintf(stderr,"Error : Bad allocation\n");
			return;
		}
		da_push(&da_obj, mp);
	}

	ub_init((UBase_t*)mp, x, y, pxtod(32, false), pxtod(32, true), EObjectType_Particle);
	mp->dx = dx;
	mp->dy = dy;
	mp->b.bMoD = gState == EGameState_FreeGame;
}

void br_init()
{
	gtBACKGROUND = ntt(N_TEXTURE_BACKGROUND);
}

void br_destroy()
{
	if (gtBACKGROUND) SDL_DestroyTexture(gtBACKGROUND);
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

void m_clamp60fps(const uint32_t LastUpdateTick)
{
	uint32_t ticks = SDL_GetTicks();

	//printf("LastUpdateTick = %d, ticks = %d\n", LastUpdateTick, ticks);

	if (LastUpdateTick > ticks)
		return;

	if (ticks - LastUpdateTick <= 16)
		SDL_Delay(16 - (ticks - LastUpdateTick));

	//printf("inter : %d\n", SDL_GetTicks() - LastUpdateTick);
}

bool ub_over(UBase_t* elt, const double x, const double y)
{
	double dx = pxtod(16, true);
	double dy = pxtod(16, false);
	if(elt->eot == EObjectType_Text)
	{
		dx = elt->w * 0.5f;
		dy = elt->h * 0.5f;
	}
	if (elt)
	{
		if (fabs(elt->x - x) <= dx && fabs(elt->y - y) <= dy)
			return true;
	}
	return false;
}

UBase_t* findOverlayObj(const int x, const int y)
{
	da_elm_t* it = da_obj.first;
	UBase_t* res = NULL;
	while (it)
	{
		if (ub_over((UBase_t*)it->elt, x * 100 / (double)gCurrentWidth, y * 100 / (double)gCurrentHeight))
			res = (UBase_t*)it->elt;
		it = it->next;
	}
	return res;
}

void ch_create(const double x, const double y, const enum EChargeStrength cs)
{
	UCharge_t* c = malloc(sizeof(UCharge_t));
	da_push(&da_obj, c);
	ch_init(c, x, y, cs);
}

void saveScoresx()
{
	FILE* fp = NULL;
	memset(gScores, 0, sizeof(uint32_t)* MAX_SCORES);
	fp = fopen(F_SCORE_NAME, "wb");
	if (!fp)
	{
		fprintf(stderr, "fail to open %s file in wb mode\n", F_SCORE_NAME);
		return;
	}
	fwrite(gScores, sizeof(uint32_t), MAX_SCORES, fp);
	fclose(fp);
}

void ubt_click(UButton_t* this)
{
	if (!this) return;
	printf("click\n");
	switch (this->f)
	{
	case EButtonFunction_Pause: 
		if(gState == EGameState_FreeGame)
		{
			bGameFreeze = true; 
			this->f = EButtonFunction_Play;
		}
		break;
	case EButtonFunction_Play:
		if (gState == EGameState_FreeGame)
		{
			bGameFreeze = false; 
			this->f = EButtonFunction_Pause;
		}
		else if (gState == EGameState_LevelGame)
		{
			bGameFreeze = false; 
			this->f = EButtonFunction_Retry;
		}
		break;
	case EButtonFunction_Back: menu_init(); break;
	case EButtonFunction_Clear:
		if (gState == EGameState_FreeGame)
			freemode_init(false);
		else if (gState == EGameState_ScoreMenu)
		{
			saveScoresx();
			score_init();
		}
		break;
	case EButtonFunction_Retry: 
		if (gState == EGameState_LevelGame) 
		{
			++gTrynb;
			bGameFreeze = true; 
			this->f = EButtonFunction_Play;
			p_init(gMPPosx, gMPPosy, 0, 0);
		}
	case EButtonFunction_Size: break;
	default: ;
	}
}

inline bool bCanCreateOrDelete()
{
	return !(gState == EGameState_LevelGame && !bGameFreeze);
}

void SDL_HandleLeftClick(const bool bUp, const int x, const int y)
{
	UBase_t* obj = findOverlayObj(x, y);

	switch (gState)
	{
	case EGameState_ScoreMenu:
		if (bUp && obj && obj->eot == EObjectType_Button)
			ubt_click((UButton_t *)obj);
		break;
	case EGameState_MainMenu:
		if (bUp && obj && obj->eot == EObjectType_Text && ((UTextBase_t *)obj)->bTextButton)
			utb_click((UTextButton_t*)obj);
		break;
	case EGameState_LevelGame:
	case EGameState_FreeGame:
		if (bUp)
		{
			if (gDragObj)
				gDragObj = NULL;
			else
			{		
				if ( obj && obj->eot == EObjectType_Button)
					ubt_click((UButton_t *)obj);
				else if(bCanCreateOrDelete())
					ch_create(pxtod(x, true), pxtod(y, false), EChargeStrength_m);
			}
		}
		else if (obj && obj->bMoD)
				gDragObj = obj;
		break;
	default: ;
	}
}

void SDL_HandleRightClick(const bool bUp, const int x, const int y)
{
	UBase_t* obj = findOverlayObj(x, y);
	switch (gState)
	{
	case EGameState_Unknown: break;
	case EGameState_MainMenu: break;
	case EGameState_ScoreMenu: break;
	case EGameState_FreeGame:
	case EGameState_LevelGame:
		if (bUp &&  bCanCreateOrDelete())
		{
			if (obj == NULL)
			{
				uge_create(pxtod(x, true), pxtod(y, false), EObjectType_flag);
			}
			else
			{
				EObjectType e = obj ? ((UBase_t*)obj)->eot : EObjectType_Unknown;
				if (e >= EObjectType_Charge && e <= EObjectType_begin)
				{
					void* res = da_removeat(&da_obj, obj);
					if (res == obj) free(obj);
					else fprintf(stderr, "Error removing charge\n");
				}
			}
		}
		break;
	case EGameState_size: break;
	default: ;
	}
}

void SDL_HandleMiddleClick(const bool bUp, const int x, const int y)
{
	UBase_t* b = NULL;
	switch (gState)
	{
	case EGameState_Unknown: break;
	case EGameState_MainMenu: break;
	case EGameState_ScoreMenu: break;
	case EGameState_LevelGame:
	case EGameState_FreeGame:
		b = findOverlayObj(x, y);
		//EObjectType e = b ? ((UBase_t*)b)->s : EObjectType_Unknown;
		if (b && b->eot == EObjectType_Charge)
		{
			UCharge_t * c = (UCharge_t *)b;
			if (bUp)
			{
				if (c->f == EChargeStrength_ppp)
					c->f = EChargeStrength_mmm;
				else
					c->f--;
			}
			else
			{
				if (c->f == EChargeStrength_mmm)
					c->f = EChargeStrength_ppp;
				else
					c->f++;
			}
		}
		/*if (b && b->eot >= EObjectType_flag && b->eot <= EObjectType_begin)
		{
			if (objTExist((b->eot == EObjectType_flag) ? EObjectType_begin : EObjectType_flag))
			{
				da_elm_t* it = da_obj.first;
				while (it)
				{
					if (it->elt && (((UBase_t *)it->elt)->eot == ((b->eot == EObjectType_flag) ? EObjectType_begin : EObjectType_flag)))
					{
						((UBase_t *)it->elt)->eot = b->eot;
						break;
					}
					it = it->next;
				}
			}
			((UBase_t*)b)->eot = (b->eot == EObjectType_flag) ? EObjectType_begin : EObjectType_flag;
		}*/
		break;
	case EGameState_size: break;
	default: ;
	}
}

UTextBase_t* ut_create(const double x, const double y, const char* t, bool bBig)
{
	UTextBase_t* res = malloc(sizeof(UTextButton_t));
	if (!res) printf("Fail to allocate memory for utb\n");
	else
	{
		ub_init((UBase_t*)res, x, y, 0, 0, EObjectType_Text);
		res->bTextButton = false;
		res->t = t;
		res->bBig = bBig;

		da_push(&da_obj, res);
	}
	return res;
}

SDL_Texture* loadFromRenderedText(char* textureText, SDL_Color textColor, TTF_Font* f, int* w, int* h)
{
	if (!(w && h)) return NULL;
	//Get rid of preexisting texture
	//free();
	SDL_Texture* res = NULL;
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

UTextButton_t* utb_create(const double x, const double y, EButtonFunction f)
{
	UTextButton_t* res = malloc(sizeof(UTextButton_t));
	if (!res) printf("Fail to allocate memory for utb\n");
	else
	{
		ub_init((UBase_t*)res, x, y, 0, 0, EObjectType_Text);

		res->txt.bTextButton = true;
		res->f = f;
		res->txt.bBig = false;
		switch (f)
		{
		case ETextButtonFunction_free: res->txt.t = M_FREEMODE;
			break;
		case ETextButtonFunction_score: res->txt.t = M_HIGHSCORE;
			break;
		case ETextButtonFunction_level: res->txt.t = M_LEVEL;
			break;
		case ETextButtonFunction_quit: res->txt.t = M_QUIT;
			break;
		case ETextButtonFunction_Unknown:
		case ETextButtonFunction_none:
		case ETextButtonFunction_size:
		default: res->txt.t = NULL;
		}
		da_push(&da_obj, res);
	}
	return res;
}

char sc1[MAX_FILE_NAME];
char sc2[MAX_FILE_NAME];
char sc3[MAX_FILE_NAME];

void score_init()
{
	if (gDeleteCurrentState) gDeleteCurrentState();	
	gState = EGameState_ScoreMenu;
	loadScores();
	ut_create(P_GAMETILE_X, P_GAMETILE_Y - 15.f, M_HIGHSCORE, true);
	ubt_create(P_BACK_X, P_BACK_Y, EButtonFunction_Back);
	ubt_create(P_CLEAR_X, P_CLEAR_Y, EButtonFunction_Clear);

	snprintf(sc1, MAX_FILE_NAME, "1. Score is : %d", gScores[0]);
	snprintf(sc2, MAX_FILE_NAME, "2. Score is : %d", gScores[1]);
	snprintf(sc3, MAX_FILE_NAME, "3. Score is : %d", gScores[2]);

	if (gScores[0]) ut_create(50.f, 50.f, sc1, false);
	if (gScores[1]) ut_create(50.f, 60.f, sc2, false);
	if (gScores[2]) ut_create(50.f, 70.f, sc3, false);

	gDeleteCurrentState = &obj_destroy;
}

void level_init(bool bLoadMap)
{
	da_elm_t * it = NULL;
	UFlag_t * flg = NULL;
	UParticle_t * p = NULL;
	da_t userobj;
	da_init(&userobj);
	if(gState == EGameState_LevelGame) //Save object placed by the user
	{
		it = da_obj.first;
		while (it)
		{
			if (it->elt && ((UBase_t *)it->elt)->bMoD) //this is an object placed by the user
			{
				da_push(&userobj, it->elt);
			}		
			it = it->next;
		}
		it = userobj.first;
		while (it)
		{
			da_removeat(&da_obj, it->elt);
			it = it->next;
		}
	}

	if (gDeleteCurrentState) gDeleteCurrentState();
	gState = EGameState_LevelGame;
	if(bLoadMap) loadMap(F_MAP_NAME);
	it = da_obj.first;

	while(it)
	{
		if(it->elt)
		{
			UBase_t * b = (UBase_t *)it->elt;
			b->bMoD = false;
			if (b->eot == EObjectType_flag)
				flg = (UFlag_t *)b;
		}
		it = it->next;
	}
	if (!flg) //create a flag
		uge_create(50.f,50.f, EObjectType_flag);

	it = userobj.first;
	while (it)
	{
		da_push(&da_obj, it->elt);		
		it = it->next;
	}

	p = getMainParticle();

	if(p && bLoadMap)
	{
		gMPPosx = p->b.x;
		gMPPosy = p->b.y;
		gTrynb = 0;
	}

	p_init(gMPPosx, gMPPosy, 0, 0);	

	ubt_create(P_BACK_X, P_BACK_Y, EButtonFunction_Back);
	ubt_create(P_PAUSEPLAY_X, P_PAUSEPLAY_Y, EButtonFunction_Play);
	ut_create(P_BACK_X+5.f, P_PAUSEPLAY_Y, M_TRY, false);

	bGameFreeze = true;

	gDeleteCurrentState = &obj_destroy;
}

void utb_click(UTextButton_t* this)
{
	switch (this->f)
	{
	case ETextButtonFunction_free: freemode_init(true); break;
	case ETextButtonFunction_score: score_init(); break;
	case ETextButtonFunction_level: level_init(true); break;
	case ETextButtonFunction_quit: gbRun = false;
		break;
	default: ;
	}
}

/** \brief create all the object for the menu */
void menu_init()
{
	if (gDeleteCurrentState) gDeleteCurrentState();
	printf("create menu \n");
	bGameFreeze = false;
	loadMap(F_MENUMAP_NAME);
	gState = EGameState_MainMenu;
	ut_create(P_GAMETILE_X, P_GAMETILE_Y, M_GAMETITLE, true);
	utb_create(P_FREEMODE_X, P_FREEMODE_Y, ETextButtonFunction_free);
	utb_create(P_LEVEL_X, P_LEVEL_Y, ETextButtonFunction_level);
	utb_create(P_HIGHSCORE_X, P_HIGHSCORE_Y, ETextButtonFunction_score);
	utb_create(P_QUIT_X, P_QUIT_Y, ETextButtonFunction_quit);

	gDeleteCurrentState = &obj_destroy;
}

void freemode_init(bool bLoadMap)
{
	if (gDeleteCurrentState) gDeleteCurrentState();
	if(bLoadMap) loadMap(F_MAP_NAME);
	gState = EGameState_FreeGame;
	p_init(gMPPosx, gMPPosy, 0, 0); //Ensure the main particle always exist
	ubt_create(P_BACK_X, P_BACK_Y, EButtonFunction_Back);
	ubt_create(P_PAUSEPLAY_X, P_PAUSEPLAY_Y, EButtonFunction_Pause);
	ubt_create(P_CLEAR_X, P_CLEAR_Y, EButtonFunction_Clear);
	bGameFreeze = false;
	gDeleteCurrentState = &obj_destroy;
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
	gSecondaryFontBold = TTF_OpenFont("./fonts/Roboto-Black.ttf", 60);
	if (gSecondaryFontBold == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gTitleFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 70);
	if (gSecondaryFont == NULL)
		printf("Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());

	//__________________OPEN IMAGES__________________
	gtPARTICLE = ntt(N_TEXTURE_PARTICLE); 
	gtBACKGROUNDW = ntt(N_TEXTURE_BACKGROUNDW);
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
	gtEND = ntt(N_TEXTURE_END);
	gtPAUSE = ntt(N_TEXTURE_PAUSE);
	gtPAUSEB = ntt(N_TEXTURE_PAUSEB);
	gtBACK = ntt(N_TEXTURE_BACK);
	gtBACKB = ntt(N_TEXTURE_BACKB);
	gtPLAY = ntt(N_TEXTURE_PLAY);
	gtPLAYB = ntt(N_TEXTURE_PLAYB);
	gtREFRESH = ntt(N_TEXTURE_REFRESH);
	gtREFRESHB = ntt(N_TEXTURE_REFRESHB);
	gtCLEAR = ntt(N_TEXTURE_CLEAR);
	gtCLEARB = ntt(N_TEXTURE_CLEARB);

	da_init(&da_obj); //init array of obj
	br_init(); //init background
	//p_init();
	menu_init();
}

void clearHighlightf()
{
	da_elm_t* it = da_obj.first;
	while (it)
	{
		if (it->elt)
			((UBase_t *)it->elt)->bO = false;
		it = it->next;
	}
}

void saveMap(const char * filename)
{
	FILE* fp = NULL;
	da_elm_t* it = da_obj.first;
	fp = fopen(filename, "wb");
	if(!fp)
	{
		fprintf(stderr, "fail to open %s file in wb mode\n", filename);
		return;
	}
	while (it)
	{
		UBase_t* b = (UBase_t*)it->elt;
		if (b)
		{
			switch (b->eot)
			{
			case EObjectType_Unknown: break;
			case EObjectType_Particle: 
				fwrite(b, sizeof(UParticle_t), 1, fp); break;
			case EObjectType_Charge:
				fwrite(b, sizeof(UCharge_t), 1, fp); break;
			case EObjectType_flag:
			case EObjectType_begin:
				fwrite(b, sizeof(UFlag_t), 1, fp); break;
			case EObjectType_Text: break;
			case EObjectType_size: break;
			default: ;
			}
		}

		it = it->next;
	}

	/*fseek(fp, 0, SEEK_END);
	long int size = ftell(fp); // now you got size of file in bytes
	fseek(fp, 0, SEEK_SET);    // same as rewind(fp)
	for (int i = 0; i<size; i++)
	{
		fread(&a, sizeof(int), 1, fp); // you read one int (sizeof(int)!=1 byte)
		printf("%d\t", a);
	}*/
	fclose(fp);
}

void loadMap(const char * filename)
{
	size_t rsize = 0;
	FILE* fp = fopen(filename, "rb");
	if (!fp)
	{
		fprintf(stderr, "fail to open %s file in rb mode\n", filename);
		return;
	}
	EObjectType eot = 0;

	obj_destroy();

	while((rsize = fread(&eot, sizeof(EObjectType), 1, fp)))
	{
		size_t ws = ftell(fp);
		fseek(fp, ws-sizeof(EObjectType), SEEK_SET);
		switch(eot)
		{
		case EObjectType_Particle:
			{
				UParticle_t p;
				printf("create patcile\n");
				fread(&p, sizeof(UParticle_t), 1, fp);
				p_init(p.b.x, p.b.y, p.dx, p.dy);
			}
			break;
		case EObjectType_Charge:
			{printf("create c\n");
				UCharge_t c;
				fread(&c, sizeof(UCharge_t), 1, fp);
				ch_create(c.b.x, c.b.y, c.f);
			}
			break;
		case EObjectType_flag:
		{
			UFlag_t ge;
			fread(&ge, sizeof(UFlag_t), 1, fp);
			uge_create(ge.b.x, ge.b.y, ge.b.eot);
		}
			break;
		default: printf("failed to load map\n");
			fclose(fp); break;
		}
	}
	fclose(fp);

	/*fseek(fp, 0, SEEK_END);
	long int size = ftell(fp); // now you got size of file in bytes
	fseek(fp, 0, SEEK_SET);    // same as rewind(fp)
	for (int i = 0; i<size; i++)
	{
		fread(&a, sizeof(int), 1, fp); // you read one int (sizeof(int)!=1 byte)
		printf("%d\t", a);
	}*/
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
			switch (gState)
			{
			case EGameState_Unknown: break;
			case EGameState_MainMenu: gbRun = false;
				break;
			case EGameState_ScoreMenu:
			case EGameState_FreeGame:
			case EGameState_LevelGame:
			case EGameState_Win:
				menu_init();
				break;
			default: ;
			}
		}
		else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_s)
		{
			if (gState == EGameState_FreeGame)
			{
				printf("save level to file\n");
				saveMap(F_MAP_NAME);
			}
		}
		else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_o)
		{
			if (gState == EGameState_FreeGame)
			{
				printf("open level from file\n");
				loadMap(F_MAP_NAME);
			}
		}
		/*if (e.key.keysym.scancode != SDL_GetScancodeFromKey(e.key.keysym.sym)) {
		 SDL_Log("Physical %s key acting as %s key",
			 SDL_GetScancodeName(e.key.keysym.scancode),
			 SDL_GetKeyName(e.key.keysym.sym));
	 } */
		//break;
		//If mouse event happened
		else if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
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
		else if (e.type == SDL_MOUSEMOTION)
		{
			clearHighlightf();
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (gDragObj)
			{
				gDragObj->x = x * 100 / (double)gCurrentWidth;
				gDragObj->y = y * 100 / (double)gCurrentHeight;
			}
			else
			{
				UBase_t* b = findOverlayObj(x, y);
				if (b)
				{
					b->bO = true;
				}
			}
		}
		else if (e.type == SDL_MOUSEWHEEL)
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
		else if (e.type == SDL_WINDOWEVENT) //windows resize
		{
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				gCurrentWidth = e.window.data1;
				gCurrentHeight = e.window.data2;
			}
		}
	}
}

double getffeo(enum EChargeStrength object)
{
	double f = 0;
	switch (object)
	{
	case EChargeStrength_ppp: f = 0.6f;
		break;
	case EChargeStrength_pp: f = 0.4f;
		break;
	case EChargeStrength_p: f = 0.2f;
		break;
	case EChargeStrength_m: f = -1.f;
		break;
	case EChargeStrength_mm: f = -2.f;
		break;
	case EChargeStrength_mmm: f = -3.f;
		break;
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
bool ftoa(UCharge_t* c, double* fx, double* fy, double x, double y)
{
	if (!(fx && fy)) return false;
	double f = getffeo(c->f);
	double xd = x - c->b.x;
	double yd = y - c->b.y;
	double d = sqrt(xd * xd + yd * yd);
	if (d < 5.f) d = 5.f;
	f = f / d;
	*fx += f * (d ? xd / d : 0);
	*fy += f * (d ? yd / d : 0);
	return true;
}

void m_update(uint32_t delta)
{
	if (!delta) return;
	UParticle_t * p = getMainParticle();
	if (p)
	{
		if (gDragObj == (UBase_t*)p)
		{
			static double lastx = 0;
			static double lasty = 0;
			p->dx = (p->b.x - lastx) / delta;
			p->dy = (p->b.y - lasty) / delta;
			clamp(&p->dx, -MAX_SPEED, MAX_SPEED);
			clamp(&p->dy, -MAX_SPEED, MAX_SPEED);
			lastx = p->b.x;
			lasty = p->b.y;
			return;
		}
		da_elm_t* it = da_obj.first;
		if (!bGameFreeze)
		{
			while (it)
			{
				double fx = 0;
				double fy = 0;
				double lastx = p->b.x;
				double lasty = p->b.y;

				EObjectType e = it->elt ? ((UBase_t*)it->elt)->eot : EObjectType_Unknown;
				if (e == EObjectType_Charge)
					ftoa((UCharge_t*)it->elt, &fx, &fy, p->b.x, p->b.y);

				fx -= p->dx * AIR_FRICTION;
				fy -= p->dy * AIR_FRICTION;

				fx *= PARTICLE_WEIGHT_CONST;
				fy *= PARTICLE_WEIGHT_CONST;

				clamp(&fx, -MAX_ACCELERATION, MAX_ACCELERATION);
				clamp(&fy, -MAX_ACCELERATION, MAX_ACCELERATION);

				p->b.x += p->dx * delta + fx * delta * delta;
				p->b.y += p->dy * delta + fy * delta * delta;

				clamp(&p->b.x, 0.f, 100.f);
				clamp(&p->b.y, 0.f, 100.f);


				p->dx = (p->b.x - lastx) / delta;
				p->dy = (p->b.y - lasty) / delta;

				clamp(&p->dx, -MAX_SPEED, MAX_SPEED);
				clamp(&p->dy, -MAX_SPEED, MAX_SPEED);

				it = it->next;
			}
		}
		if(gState == EGameState_LevelGame)
		{
			UFlag_t * flag = NULL;
			it = da_obj.first;
			while (it)
			{
				if (it->elt)
				{
					UBase_t * b = (UBase_t *)it->elt;
					if (b->eot == EObjectType_flag)
						flag = (UFlag_t *)b;
				}
				it = it->next;
			}
			if (!flag)
			{
				fprintf(stderr, "Error : no flag to compare to\n");
				return;
			}
		
			if (fabs(p->b.x - flag->b.x) <= pxtod(16, true) && fabs(p->b.y - flag->b.y) <= pxtod(16, false))
			{
				printf("win\n");
				bGameFreeze = true;
				gState = EGameState_Win;
				//ut_create(P_GAMETILE_X, P_GAMETILE_Y, M_WIN, true);
				++gTrynb;
				saveScores(gTrynb);
			}		
		}	
	}
}

void da_obj_sort()
{
	da_elm_t* tnn = NULL;
	da_elm_t* tn = NULL;
	da_elm_t* tp = NULL;
	da_elm_t* it = NULL;
	da_elm_t* it2 = NULL;
	bool bChange = true;
	while (bChange) //bubble sort
	{
		it = da_obj.first;
		bChange = false;
		while (it)
		{
			if (it->elt && it->next && it->next->elt && ((UBase_t *)it->elt)->y > ((UBase_t *)it->next->elt)->y)
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

				while (it2 && !(it2->next == it)) it2 = it2->next;
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
	TTF_Font* f = NULL;
	if (this->t.bTextButton)
		f = this->tb.txt.b.bO ? gMainFontBold : gMainFont;
	else if(this->t.bBig)
		f = gSecondaryFontBold;
	else
		f = gSecondaryFont;
	if (f)
	{
		int w, h;
		SDL_Color textColor = {200, 200, 200};
		SDL_Texture* t;
		char c[MAX_FILE_NAME];// = (char *)this->t.t;

		snprintf(c, MAX_FILE_NAME, this->t.t, gTrynb);

		if ((t = loadFromRenderedText(c, textColor, f, &w, &h)))
		{
			SDL_Rect dest;

			this->t.b.w = pxtod(w, true);
			this->t.b.h = pxtod(h, false);
			dest.w = w;
			dest.h = h;
			dest.x = dtopx(this->t.b.x - this->t.b.w * 0.5f, true);
			dest.y = dtopx(this->t.b.y - this->t.b.h * 0.5f, false);

			SDL_RenderCopy(gSdlRenderer, t, NULL, &dest);
			SDL_DestroyTexture(t);
		}
		if (this->t.bTextButton && this->tb.txt.b.bO)
		{
			SDL_Rect dest;
			dest.w = 32;
			dest.h = 32;
			dest.x = dtopx(this->t.b.x - this->t.b.w * 0.5f, true) - (int)(1.2f * dest.w);
			dest.y = dtopx(this->t.b.y - this->t.b.h * 0.5f, false) - -(int)(0.5 * dest.h);
			SDL_RenderCopy(gSdlRenderer, gtPARTICLE, NULL, &dest);
		}
	}
}

void da_obj_render(da_t* da)
{
	da_elm_t* it = NULL;
	da_obj_sort();
	it = da_obj.first;
	while (it)
	{
		if (it->elt)
		{	
			switch (((UBase_t*)it->elt)->eot)
			{
			case EObjectType_Unknown: break;
			case EObjectType_Particle:
			case EObjectType_Charge:
			case EObjectType_flag:
			case EObjectType_begin:
			case EObjectType_Button:
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

uint32_t getPlaceFromScore()
{
	size_t i = 0;
	for(i; i < MAX_SCORES; ++i) if (gScores[i] == gTrynb) return i + 1;
	return 0;
}

void m_render()
{
	SDL_RenderClear(gSdlRenderer);

	br_render();
	da_obj_render(&da_obj);

	if(gState == EGameState_Win)
	{
		SDL_Rect dest;

		dest.w = gCurrentWidth;
		dest.h = gCurrentHeight;
		dest.x = 0;
		dest.y = 0;

		SDL_RenderCopy(gSdlRenderer, gtBACKGROUNDW, NULL, &dest);

		UTextBase_t youwint;
		youwint.b.x = P_GAMETILE_X;
		youwint.b.y = P_GAMETILE_Y;
		youwint.b.eot = EObjectType_Text;
		youwint.t = M_WIN;
		youwint.bBig = true;
		youwint.bTextButton = false;
		ut_render((UText_t *)&youwint);

		UTextBase_t pressEscpae;
		pressEscpae.b.x = P_GAMETILE_X;
		pressEscpae.b.y = P_GAMETILE_Y + 40.f;
		pressEscpae.b.eot = EObjectType_Text;
		pressEscpae.t = M_PRESSESCAPE;
		pressEscpae.bBig = false;
		pressEscpae.bTextButton = false;
		ut_render((UText_t *)&pressEscpae);

		char c[MAX_FILE_NAME];
		snprintf(c, MAX_FILE_NAME, M_SUCEED, gTrynb);

		UTextBase_t intry;
		intry.b.x = P_GAMETILE_X;
		intry.b.y = P_GAMETILE_Y + 30.f;
		intry.b.eot = EObjectType_Text;
		intry.t = c;
		intry.bBig = false;
		intry.bTextButton = false;
		ut_render((UText_t *)&intry);

		uint32_t p = getPlaceFromScore();
		switch (p)
		{
		case 1:
			snprintf(c, MAX_FILE_NAME, "You are 1st in highscore"); break;
		case 2:
			snprintf(c, MAX_FILE_NAME, "You are 2nd in highscore"); break;
		case 3:
			snprintf(c, MAX_FILE_NAME, "You are 3rd in highscore"); break;
		default:
			snprintf(c, MAX_FILE_NAME, "You are not in highscore"); break;
		}	

		UTextBase_t placeis;
		placeis.b.x = P_GAMETILE_X;
		placeis.b.y = P_GAMETILE_Y + 20.f;
		placeis.b.eot = EObjectType_Text;
		placeis.t = c;
		placeis.bBig = false;
		placeis.bTextButton = false;
		ut_render((UText_t *)&placeis);
	}

	SDL_RenderPresent(gSdlRenderer);
}

void m_clear()
{
	void* elt;
	while ((elt = da_remove(&da_obj))) free(elt);
	br_destroy();
	SDL_DestroyWindow(gSdlWindow);
	SDL_Quit();
}

int main(int argc, char* argv[])
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
