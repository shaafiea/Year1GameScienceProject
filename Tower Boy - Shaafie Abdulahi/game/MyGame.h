#pragma once

class CMyGame : public CGame
{
	// Define sprites and other instance variables here
	CSprite m_player;		// Player sprite
	CSpriteList m_sprites;	// All other sprites
	CSpriteList player_death;
	CSpriteList barrel_list;
	CSpriteList coinList;
	CSprite background;
	CSprite mainmenu;
	CSprite game_over;
	CGraphics fireballs;
	CGraphics spikes;
	CGraphics ladders;
	CGraphics barrels;
	CGraphics big_platform;
	CGraphics a_platform;
	CGraphics s_platform;
	CGraphics b_platform;

	//Sounds
	CSoundPlayer barrelbounce;
	CSoundPlayer coin;
	CSoundPlayer life;
	CSoundPlayer gameover;
	CSoundPlayer fireburn;
	CSoundPlayer spikedeath;
	CSoundPlayer doorunlock;
	CSoundPlayer jump;
	CSoundPlayer keycollect;
	CSoundPlayer pauseoff;
	CSoundPlayer pauseon;
	CSoundPlayer Lwalking;
	CSoundPlayer Rwalking;
	CSoundPlayer win;
	CSoundPlayer lose;


	int lives;
	int score;
	int level;
	bool GotKey;


	//barrels
	float x;
	float y;
	float f1;
	float f2;

	// Player State
	enum { STANDING, RUNNING, AIRBORNE, CLIMBING } m_state;
	enum { LEFT, RIGHT } m_side;

	//levels
	void SetupLevel1();
	void SetupLevel2();

	// Barrel
	void BarrelControl();

public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);


	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnStartLevel(Sint16 nLevel);
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
