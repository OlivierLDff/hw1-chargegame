#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

/**
 * \brief Charge Game - Olivier Le Doeuff - CR0ZOQ
 * Homework Basic Of Programming
 * to compile you need SDL2 / image / ttf
 * olivier.ldff@gmail.com
 */

#define GAME_TITLE "ChargeGame"		//Window title
#define SCREEN_WIDTH 640			
int gCurrentWidth = SCREEN_WIDTH;	//Current width of screen can be resized
#define SCREEN_HEIGHT 480
int gCurrentHeight = SCREEN_HEIGHT; //Current height of screen can be resized

#define STRING_BUFFER_SIZE 64

//_________________PATH CONSTANT________________

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

//_______________GAME PHYSICS CONSTANT________________

const double MAX_ACCELERATION = 0.00005f;
const double MAX_SPEED = 10.f;
const double PARTICLE_WEIGHT_CONST = 0.005f;
const double AIR_FRICTION = 0.001f;

//_________________STRING CONSTANT____________________

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

char sc1[STRING_BUFFER_SIZE];	//Score 1 string
char sc2[STRING_BUFFER_SIZE];	//Score 2 string
char sc3[STRING_BUFFER_SIZE];	//Score 3 string

//_________________POSTION CONSTANT____________________

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

//________________GLOBAL VARIABLE___________________

/** \brief is the application running, set to false to stop the main loop */
bool gbRun = true;
/** \brief function pointer to destroy current game state when the state is changed */
void (*gDeleteCurrentState)() = NULL;

/** \brief the main window, init in m_init() */
SDL_Window* gSdlWindow = NULL;
/** \brief the sdl renderer that render every texture, init in m_init( */
SDL_Renderer* gSdlRenderer = NULL;

//________________FONTS___________________

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

//________________TEXTURES___________________

/** \brief Background SDL texture pointer */
SDL_Texture * gtBACKGROUND = NULL;
/** \brief Background win SDL texture pointer */
SDL_Texture * gtBACKGROUNDW = NULL;
/** \brief Particle image SDL texture pointer */
SDL_Texture * gtPARTICLE = NULL;
/** \brief charge + image SDL texture pointer */
SDL_Texture * gtCHARGE1 = NULL;
/** \brief charge ++ image SDL texture pointer */
SDL_Texture * gtCHARGE2 = NULL;
/** \brief charge +++ image SDL texture pointer */
SDL_Texture * gtCHARGE3 = NULL;
/** \brief charge - image SDL texture pointer */
SDL_Texture * gtNEGCHARGE1 = NULL;
/** \brief charge -- image SDL texture pointer */
SDL_Texture * gtNEGCHARGE2 = NULL;
/** \brief charge --- image SDL texture pointer */
SDL_Texture * gtNEGCHARGE3 = NULL;
/** \brief image draw to overlay a particle/charge/flag when mouse is on it image SDL texture pointer */
SDL_Texture * gtOVERLAYCHARGE = NULL;
/** \brief image draw to overlay a particle/charge/flag when drag and drap image SDL texture pointer */
SDL_Texture * gtDRAGCHARGE = NULL;
/** \brief flag image SDL texture pointer */
SDL_Texture * gtFLAG = NULL;
/** \brief not used in this version, may be used to draw wall begin */
SDL_Texture * gtBEGIN = NULL;
/** \brief not used in this version, may be used to draw wall end */
SDL_Texture * gtEND = NULL;
/** \brief pause button texture */
SDL_Texture * gtPAUSE = NULL;
/** \brief pause button bold texture */
SDL_Texture * gtPAUSEB = NULL;
/** \brief back texture */
SDL_Texture * gtBACK = NULL;
/** \brief back bold texture */
SDL_Texture * gtBACKB = NULL;
/** \brief play texture */
SDL_Texture * gtPLAY = NULL;
/** \brief play bold texture */
SDL_Texture * gtPLAYB = NULL;
/** \brief refresh texture */
SDL_Texture * gtREFRESH = NULL;
/** \brief refresh bold texture */
SDL_Texture * gtREFRESHB = NULL;
/** \brief clear texture */
SDL_Texture * gtCLEAR = NULL;
/** \brief clear bold texture */
SDL_Texture * gtCLEARB = NULL;

//________________DYNAMIC ARRAY___________________

/** \brief dynamic array element */
typedef struct da_elm_t
{
	/** \brief pointer to the next dynamic array element */
	struct da_elm_t * next;
	/** \brief pointer to element, must be allocate and free by the user */
	void * elt;
} da_elm_t;

/** \brief dynamic array */
typedef struct da_t
{
	/** \brief first element of the array */
	struct da_elm_t * first;
	/** \brief size of the array */
	size_t size;
} da_t;

/** \brief init a dynamic array */
void da_init(da_t* this);
/** \brief push a value into a dynamic array */
void da_push(da_t* this, void* elt);
/** \brief remove at element dynamic array \return pointer to the element removed */
void* da_removeat(da_t* this, const void* elt);
/** \brief remove last element \return pointer to the element removed */
void* da_remove(da_t* this);

//_______________ENUMERATIONS___________________

/** \brief game state */
typedef enum EGameState
{
	EGameState_Unknown,		//Unkwown state, should never occured
	EGameState_MainMenu,	//In main menu
	EGameState_ScoreMenu,	//in score menu
	EGameState_FreeGame,	//in level editor
	EGameState_LevelGame,	//in level play
	EGameState_Win,			//level play win
	EGameState_size,		//to iterate inside the enum
} EGameState;

/** \brief current state of the game, by default we start in main menu */
EGameState gState = EGameState_MainMenu;

/** \brief all text button available function */
typedef enum ETextButtonFunction
{
	ETextButtonFunction_Unknown,	//unknown function, should never occured
	ETextButtonFunction_none,		//text button doesn't do anything
	ETextButtonFunction_free,		//Launch level editor
	ETextButtonFunction_score,		//launch score menu
	ETextButtonFunction_level,		//launch level play menu
	ETextButtonFunction_quit,		//quit the application
	ETextButtonFunction_size		//to iterate inside the enum
} ETextButtonFunction;

/** \brief all button available function */
typedef enum EButtonFunction
{
	EButtonFunction_Unknown,	//unknown function, should never occured
	EButtonFunction_Pause,		//Freeze the game
	EButtonFunction_Play,		//Unfreeze the game
	EButtonFunction_Back,		//Back to main menu
	EButtonFunction_Clear,		//Clear the level editor or score files
	EButtonFunction_Retry,		//retry level
	EButtonFunction_Size,		//to iterate inside the enum
} EButtonFunction;

typedef enum EObjectType
{
	EObjectType_Unknown,	//unknown function, should never occured
	EObjectType_Particle,	//Object is a UParticle_t
	EObjectType_Charge,		//Object is a UCharge_t
	EObjectType_flag,		//Object is a UFlag_t
	EObjectType_begin,		//not implemented
	EObjectType_end,		//not implemented
	EObjectType_Text,		//Object is a UText_t
	EObjectType_Button,		//Object is a UButton_t
	EObjectType_size,		//to iterate inside the enum
} EObjectType;

typedef enum EChargeStrength
{
	EChargeStrength_ppp,	//Charge Strength is +++
	EChargeStrength_pp,		//Charge Strength is ++
	EChargeStrength_p,		//Charge Strength is +
	EChargeStrength_m,		//Charge Strength is -
	EChargeStrength_mm,		//Charge Strength is --
	EChargeStrength_mmm,	//Charge Strength is ---
}EChargeStrength;

typedef struct UBase
{
	EObjectType eot;//type of object 
	double x;		//position in the x axis, between 0 and 100
	double y;		//position in the y axis, between 0 and 100
	double w;		//width of the object, can't be used to render sprite but this is for texts
	double h;		//height of the object, can't be used to render sprite but this is for texts
	bool bO;		// is the object currently overlay by the pointer (mouse)
	bool bMoD;		//can be moved or delete ? used in the level game mode /// level editor 
} UBase_t;

/** \brief base object for a text */
typedef struct UTextBase
{
	UBase_t b;			//parent
	bool bTextButton;	//Is it a button also?
	bool bBig;			//Print in big(difference for text only between Secondary font or TitleFont in render)
	const char* t;		//Char to print
} UTextBase_t;

/** \brief child of Textbase, can have a function */
typedef struct UTextButton
{
	UTextBase_t txt;	//parent
	EButtonFunction f;	//function of the button
} UTextButton_t;

/** \brief union between Utext and UTextButton */
typedef union UText
{
	UTextBase_t t;
	UTextButton_t tb;
} UText_t;

/** \brief Charge object, that can be positive or negative depending on ChargeStrength */
typedef struct UCharge
{
	UBase_t b;			//parent
	EChargeStrength f;	//force
} UCharge_t;

/** \brief flag that is the goal of the main particle */
typedef struct UFlag
{
	UBase_t b;			//parent
} UFlag_t;

/** \brief, main particle, this is a singleton */
typedef struct UParticle
{
	UBase_t b;			//parent
	double dx;			//speed of the particle x axis 
	double dy;			//speed of the particle y axis
} UParticle_t;

/** \brief object that can be clicked */
typedef struct UButton
{
	UBase_t b;			//parent
	EButtonFunction f;	//function of the button
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
/** \brief dynamic array of objects that are render */
da_t da_obj;
/** \brief return the main particle inside the da_obj array */
UParticle_t* getMainParticle();
/** \brief is the game freeze, ie charge don't do nothing to the main Particle */
bool gbGameFreeze = false;
/** \brief default location of main particle x axis */
double gMPPosx = 50.f;
/** \brief default location of main particle y axis */
double gMPPosy = 50.f;
/** \brief number of try for the level, reset when level menu is init */
uint32_t gNbTry = 0;

/** \brief name to texture */
SDL_Texture* ntt(const char* name);
/** \brief EChargeStrength To Texture */
SDL_Texture* ecstot(UCharge_t * c);
/** \brief EButtonFunction to Texture */
SDL_Texture* ebftot(UButton_t* this);
/** \brief EObjectType to Texture */
SDL_Texture* eotot(UBase_t * b);
/** \brief char to texture, texture need to be free after */
SDL_Texture* ctot(char* textureText, SDL_Color textColor, TTF_Font* f, int* w, int* h);

/** \brief pixel to double. Translate the value of a pixel in a double between 0.f and 100.f depending on the current screen resolution */
double pxtod(const int px, const bool xAxis);
/** \brief double to pixel*/
int dtopx(const double p, const bool xAxis);
/** \brief clamp a double */
void clamp(double* p, double from, double to);

/** \brief UBase_t init */
void ub_init(UBase_t* this, const double x, const double y, const double h, const double w, const EObjectType e);
/** \brief UCharge_t initialize */
void ch_init(UCharge_t* this, const double x, const double y, EChargeStrength cs);
/** \brief allocate a Charge_t and init it */
void ch_create(const double x, const double y, const enum EChargeStrength cs);
/** \brief create an UFlag object and add it into da_obj */
void uf_create(const double x, const double y, EObjectType type);
/** \brief create main particle if it doesn't already exist */
void p_create(const double x, const double y, const double dx, const double dy);
/** \brief allocate an UtextBase_t object */
UTextBase_t* ut_create(const double x, const double y, const char* t, bool bBig);
/** Allocate and init a text button object */
UTextButton_t* utb_create(const double x, const double y, EButtonFunction f);

/** \brief load background image */
void br_init();
/** \brief render the background */
void br_render();
/** \brief destroy background image */
void br_destroy();

/** \brief UTextButton_t click */
void utb_click(UTextButton_t* this);
/** \brief UButton click */
void ubt_click(UButton_t* this);

/** \brief are x and y over the UBase object */
bool ub_over(UBase_t* elt, const double x, const double y);
/** \brief get the object inside da_obj under the mouse \return nullptr if no object found */
UBase_t* findOverlayObj(const int x, const int y);
/** \brief Clear highlight flag from all da_obj object */
void clrhflg();
/** \brief sort da_obj from behind to front */
void da_obj_sort();

/** \brief charge strength to force */
double cstof(enum EChargeStrength object);
/** \brief UCharge To Acceleration */
bool uctoa(UCharge_t* c, double* fx, double* fy, double x, double y);

/** \brief UBase_t render */
void ub_render(UBase_t* this);
/** \brief UText render */
void ut_render(UText_t* this);
/** \brief render every object inside dynamic array */
void da_obj_render(da_t* da);

/** \brief initialize game state menu */
void menu_init();
/** \brief initialize game state score */
void score_init();
/** \brief initialize game state level */
void level_init(bool bLoadMap);
/** \brief initialize game state freemode \param bLoadMap should the map be loaded or reinitialize to 0 */
void freemode_init(bool bLoadMap);

/** \brief destroy all object insa da_obj */
void obj_destroy();

/** \brief save current map to filename */
void saveMap(const char * filename);
/** \brief load map into da_obj */
void loadMap(const char * filename);

/** \brief load score array */
void loadScores();
/** \brief save score array and add the new value if necessary */
void saveScores(uint32_t value);
/** \brief Erase Score */
void eraseScores();
/** \brief score to rank */
uint32_t scrtorank();

/** \brief does an object type exist inside da_obj ?*/
bool objTExist(const EObjectType type);

/** \brief are we allowed to create or delete object */
inline bool bCanCreateOrDelete();

/** \brief Handle mouse left click */
void hmlc(const bool bUp, const int x, const int y);
/** \brief Handle mouse right click */
void hmrc(const bool bUp, const int x, const int y);
/** \brief Handle mouse middle click */
void hmmc(const bool bUp, const int x, const int y);

/** \brief initialize the app */
void m_init();
/** \brief poll sdl event */
void m_poll();
/** \brief update the physics of the game */
void m_update(uint32_t delta);
/** \brief render the game */
void m_render();
/** \brief clamp refreshment of main loop to 60fps */
void m_clamp60fps(const uint32_t LastUpdateTick);
/** \brief clear all the ressources */
void m_clear();

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

SDL_Texture* ebftot(UButton_t* this)
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
	case EObjectType_Button: return ebftot((UButton_t *)b);
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
		this->bMoD = 1;
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

void uf_create(const double x, const double y, EObjectType type)
{
	if (objTExist(type)) return;

	UFlag_t* e = malloc(sizeof(UFlag_t));
	ub_init((UBase_t *)e, x, y, pxtod(32, false), pxtod(32, true), type);
	da_push(&da_obj, e);
}

void ub_render(UBase_t* this)
{
	SDL_Rect dest;

	if (this->x >= 0 && this->y >= 0 && this->x <= 100 && this->y <= 100)
	{
		dest.w = 32; // ub always have a 32 px sprite
		dest.h = 32; // ub always have a 32 px sprite
		dest.x = dtopx(this->x - this->w * 0.5f, true);
		dest.y = dtopx(this->y - this->h * 0.5f, false);

		if(this->eot >= EObjectType_Particle && this->eot <= EObjectType_begin)
		{
			const bool bGameIsNotBeingPlayed = (gState != EGameState_LevelGame || gbGameFreeze);
			const bool bCanBeMoved = ((gState != EGameState_LevelGame) || this->bMoD);
			if (gDragObj == this)
				SDL_RenderCopy(gSdlRenderer, gtDRAGCHARGE, NULL, &dest);
			else if (this->bO && bGameIsNotBeingPlayed && bCanBeMoved && gState != EGameState_Win && gState != EGameState_MainMenu)
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

void p_create(const double x, const double y, const double dx, const double dy)
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

void eraseScores()
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
			gbGameFreeze = true; 
			this->f = EButtonFunction_Play;
		}
		break;
	case EButtonFunction_Play:
		if (gState == EGameState_FreeGame)
		{
			gbGameFreeze = false; 
			this->f = EButtonFunction_Pause;
		}
		else if (gState == EGameState_LevelGame)
		{
			gbGameFreeze = false; 
			this->f = EButtonFunction_Retry;
		}
		break;
	case EButtonFunction_Back: menu_init(); break;
	case EButtonFunction_Clear:
		if (gState == EGameState_FreeGame)
			freemode_init(false);
		else if (gState == EGameState_ScoreMenu)
		{
			eraseScores();
			score_init();
		}
		break;
	case EButtonFunction_Retry: 
		if (gState == EGameState_LevelGame) 
		{
			++gNbTry;
			gbGameFreeze = true; 
			this->f = EButtonFunction_Play;
			p_create(gMPPosx, gMPPosy, 0, 0);
		}
	case EButtonFunction_Size: break;
	default: ;
	}
}

bool bCanCreateOrDelete()
{
	return !(gState == EGameState_LevelGame && !gbGameFreeze);
}

void hmlc(const bool bUp, const int x, const int y)
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
		else if (obj && obj->bMoD && (gState != EGameState_LevelGame || gbGameFreeze))
			gDragObj = obj;
				
		break;
	default: ;
	}
}

void hmrc(const bool bUp, const int x, const int y)
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
				uf_create(pxtod(x, true), pxtod(y, false), EObjectType_flag);
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

void hmmc(const bool bUp, const int x, const int y)
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
		if (b && b->eot == EObjectType_Charge && (gState != EGameState_LevelGame || gbGameFreeze) && ((gState != EGameState_LevelGame) || b->bMoD))
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
		break;
	case EGameState_size: break;
	default: ;
	}
}

UTextBase_t* ut_create(const double x, const double y, const char* t, bool bBig)
{
	UTextBase_t* res = malloc(sizeof(UTextButton_t));
	if (!res) fprintf(stderr, "Fail to allocate memory for utb\n");
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

SDL_Texture* ctot(char* textureText, SDL_Color textColor, TTF_Font* f, int* w, int* h)
{
	if (!(w && h)) return NULL;
	//Get rid of preexisting texture
	//free();
	SDL_Texture* res = NULL;
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended(f, textureText, textColor);
	if (textSurface == NULL)
	{
		fprintf(stderr ,"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return NULL;
	}

	//Create texture from surface pixels
	res = SDL_CreateTextureFromSurface(gSdlRenderer, textSurface);
	if (res == NULL)
	{
		fprintf(stderr, "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
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

void score_init()
{
	if (gDeleteCurrentState) gDeleteCurrentState();	
	gState = EGameState_ScoreMenu;
	loadScores();
	ut_create(P_GAMETILE_X, P_GAMETILE_Y - 15.f, M_HIGHSCORE, true);
	ubt_create(P_BACK_X, P_BACK_Y, EButtonFunction_Back);
	ubt_create(P_CLEAR_X, P_CLEAR_Y, EButtonFunction_Clear);

	snprintf(sc1, STRING_BUFFER_SIZE, "1. Score is : %d", gScores[0]);
	snprintf(sc2, STRING_BUFFER_SIZE, "2. Score is : %d", gScores[1]);
	snprintf(sc3, STRING_BUFFER_SIZE, "3. Score is : %d", gScores[2]);

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
		uf_create(50.f,50.f, EObjectType_flag);

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
		gNbTry = 0;
	}

	p_create(gMPPosx, gMPPosy, 0, 0);	

	ubt_create(P_BACK_X, P_BACK_Y, EButtonFunction_Back);
	ubt_create(P_PAUSEPLAY_X, P_PAUSEPLAY_Y, EButtonFunction_Play);
	ut_create(P_BACK_X+5.f, P_PAUSEPLAY_Y, M_TRY, false);

	gbGameFreeze = true;

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
	gbGameFreeze = false;
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
	p_create(gMPPosx, gMPPosy, 0, 0); //Ensure the main particle always exist
	ubt_create(P_BACK_X, P_BACK_Y, EButtonFunction_Back);
	ubt_create(P_PAUSEPLAY_X, P_PAUSEPLAY_Y, EButtonFunction_Pause);
	ubt_create(P_CLEAR_X, P_CLEAR_Y, EButtonFunction_Clear);
	gbGameFreeze = false;
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
		fprintf(stderr, "Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gMainFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 48);
	if (gMainFont == NULL)
		fprintf(stderr, "Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gSecondaryFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 28);
	if (gSecondaryFont == NULL)
		fprintf(stderr, "Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gSecondaryFontBold = TTF_OpenFont("./fonts/Roboto-Black.ttf", 60);
	if (gSecondaryFontBold == NULL)
		fprintf(stderr, "Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());
	gTitleFont = TTF_OpenFont("./fonts/Roboto-Regular.ttf", 70);
	if (gSecondaryFont == NULL)
		fprintf(stderr, "Failed to load Roboto font! SDL_ttf Error: %s\n", TTF_GetError());

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
	//p_create();
	menu_init();
}

void clrhflg()
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
				fread(&p, sizeof(UParticle_t), 1, fp);
				p_create(p.b.x, p.b.y, p.dx, p.dy);
			}
			break;
		case EObjectType_Charge:
			{
				UCharge_t c;
				fread(&c, sizeof(UCharge_t), 1, fp);
				ch_create(c.b.x, c.b.y, c.f);
			}
			break;
		case EObjectType_flag:
		{
			UFlag_t ge;
			fread(&ge, sizeof(UFlag_t), 1, fp);
			uf_create(ge.b.x, ge.b.y, ge.b.eot);
		}
			break;
		default: fprintf(stderr, "failed to load map\n");
			fclose(fp); break;
		}
	}
	fclose(fp);
}

void m_poll()
{
	// event handling
	SDL_Event e;
	while (SDL_PollEvent(&e)) //Poll every input
	{
		if (e.type == SDL_QUIT) gbRun = false;
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
		else if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONDOWN)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			switch (e.button.button)
			{
			case SDL_BUTTON_LEFT:
				hmlc(e.type == SDL_MOUSEBUTTONUP, x, y);
				break;
			case SDL_BUTTON_RIGHT:
				hmrc(e.type == SDL_MOUSEBUTTONUP, x, y);
				break;
			default: ;
			}
		}
		else if (e.type == SDL_MOUSEMOTION)
		{
			clrhflg();
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
					b->bO = true;
			}
		}
		else if (e.type == SDL_MOUSEWHEEL)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (e.wheel.y == 1) // scroll up
			{
				hmmc(true, x, y);
			}
			else if (e.wheel.y == -1) // scroll down
			{
				hmmc(false, x, y);
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

double cstof(enum EChargeStrength object)
{
	double f = 0;
	switch (object)
	{
	case EChargeStrength_ppp: f = 3.f; break;		
	case EChargeStrength_pp: f = 2.f; break;		
	case EChargeStrength_p: f = 1.f; break;	
	case EChargeStrength_m: f = -1.f; break;
	case EChargeStrength_mm: f = -2.f; break;		
	case EChargeStrength_mmm: f = -3.f; break;		
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
bool uctoa(UCharge_t* c, double* fx, double* fy, double x, double y)
{
	if (!(fx && fy)) return false;
	double f = cstof(c->f);
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
		if (!gbGameFreeze)
		{	
			double fx = 0; //sum of force to apply on x axis
			double fy = 0; //sum of force to apply on y axis
			double lastx = p->b.x; //last x pos, used to compute dx
			double lasty = p->b.y; //last y pos, used to compute dy

			while (it)
			{

				EObjectType e = it->elt ? ((UBase_t*)it->elt)->eot : EObjectType_Unknown;
				if (e == EObjectType_Charge)
					uctoa((UCharge_t*)it->elt, &fx, &fy, p->b.x, p->b.y);
				it = it->next;
			}

			fx -= p->dx * AIR_FRICTION; //remove air friction on x axis (-speedx)
			fy -= p->dy * AIR_FRICTION; //remove air friction on y axis (-speedy)

			fx *= PARTICLE_WEIGHT_CONST; //multiply the force with the particle weigth
			fy *= PARTICLE_WEIGHT_CONST; //multiply the force with the particle weigth

			clamp(&fx, -MAX_ACCELERATION, MAX_ACCELERATION); //Clamp the maximum acceleration, we are in a game not real world
			clamp(&fy, -MAX_ACCELERATION, MAX_ACCELERATION);

			p->b.x += p->dx * delta + fx * delta * delta; // s = vt + 0.5*vt^2
			p->b.y += p->dy * delta + fy * delta * delta;

			clamp(&p->b.x, 0.f, 100.f); //stay inside the playground
			clamp(&p->b.y, 0.f, 100.f); //stay inside the playground

			p->dx = (p->b.x - lastx) / delta; //compute new x speed
			p->dy = (p->b.y - lasty) / delta; //compute new y speed

			clamp(&p->dx, -MAX_SPEED, MAX_SPEED); //clamp the x speed
			clamp(&p->dy, -MAX_SPEED, MAX_SPEED); //clamp the y speed
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
				gbGameFreeze = true;
				gState = EGameState_Win;
				++gNbTry;
				saveScores(gNbTry);
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
		char c[STRING_BUFFER_SIZE];// = (char *)this->t.t;

		snprintf(c, STRING_BUFFER_SIZE, this->t.t, gNbTry);

		if ((t = ctot(c, textColor, f, &w, &h)))
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
				fprintf(stderr, "error don't know how to render this object\n");
			}
		}
		else
			fprintf(stderr, "Error : presence of nullptr obj in array\n");
		it = it->next;
	}
}

uint32_t scrtorank()
{
	size_t i = 0;
	for(i; i < MAX_SCORES; ++i) if (gScores[i] == gNbTry) return i + 1;
	return 0;
}

void m_render()
{
	SDL_RenderClear(gSdlRenderer);

	br_render();
	da_obj_render(&da_obj);

	if(gState == EGameState_Win) //A bit ugly in here...
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

		char c[STRING_BUFFER_SIZE];
		snprintf(c, STRING_BUFFER_SIZE, M_SUCEED, gNbTry);

		UTextBase_t intry;
		intry.b.x = P_GAMETILE_X;
		intry.b.y = P_GAMETILE_Y + 30.f;
		intry.b.eot = EObjectType_Text;
		intry.t = c;
		intry.bBig = false;
		intry.bTextButton = false;
		ut_render((UText_t *)&intry);

		uint32_t p = scrtorank();
		switch (p)
		{
		case 1:
			snprintf(c, STRING_BUFFER_SIZE, "You are 1st in highscore"); break;
		case 2:
			snprintf(c, STRING_BUFFER_SIZE, "You are 2nd in highscore"); break;
		case 3:
			snprintf(c, STRING_BUFFER_SIZE, "You are 3rd in highscore"); break;
		default:
			snprintf(c, STRING_BUFFER_SIZE, "You are not in highscore"); break;
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

	TTF_CloseFont(gMainFont);
	TTF_CloseFont(gMainFontBold);
	TTF_CloseFont(gSecondaryFont);
	TTF_CloseFont(gSecondaryFontBold);
	TTF_CloseFont(gTitleFont);

	SDL_DestroyTexture(gtBACKGROUNDW);
	SDL_DestroyTexture(gtPARTICLE);
	SDL_DestroyTexture(gtCHARGE1);
	SDL_DestroyTexture(gtCHARGE2);
	SDL_DestroyTexture(gtCHARGE3);
	SDL_DestroyTexture(gtNEGCHARGE1);
	SDL_DestroyTexture(gtNEGCHARGE2);
	SDL_DestroyTexture(gtNEGCHARGE3);
	SDL_DestroyTexture(gtOVERLAYCHARGE);
	SDL_DestroyTexture(gtDRAGCHARGE);
	SDL_DestroyTexture(gtFLAG);
	SDL_DestroyTexture(gtBEGIN);
	SDL_DestroyTexture(gtEND);
	SDL_DestroyTexture(gtPAUSE);
	SDL_DestroyTexture(gtPAUSEB);
	SDL_DestroyTexture(gtBACK);
	SDL_DestroyTexture(gtBACKB);
	SDL_DestroyTexture(gtPLAY);
	SDL_DestroyTexture(gtPLAYB);
	SDL_DestroyTexture(gtREFRESH);
	SDL_DestroyTexture(gtREFRESHB);
	SDL_DestroyTexture(gtCLEAR);
	SDL_DestroyTexture(gtCLEARB);

	SDL_DestroyWindow(gSdlWindow);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	uint32_t uilt = SDL_GetTicks() + 16;	//last tick
	m_init();								//init the application

	while (gbRun)
	{
		uint32_t uict = SDL_GetTicks();		//current tick
		m_poll();							//poll input
		m_update(uict - uilt);				//update physics
		m_render();							//render the game

		m_clamp60fps(uict);					//clamp the rendering to 60fps
		uilt = uict;						//update last time
	}
	m_clear();								//clear all ressources
	return 0;
}
