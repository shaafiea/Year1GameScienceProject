#include "stdafx.h"
#include "MyGame.h"


CMyGame::CMyGame(void) :
	m_player(400, 540, 0, 0, 0),
	background(400, 371,0,0,0),
	mainmenu(400, 350, "mainmenu.bmp", 0),
	game_over(400, 350, "gameover.bmp", 0),
	barrels("barrel.bmp", CColor::White()),
	fireballs("fireball.bmp", CColor::White()),
	spikes("spike.bmp", CColor::White()),
	big_platform("big_platform.bmp", CColor::Red()),
	a_platform("a2platform.bmp", CColor::Red()),
	s_platform("smallplatform.bmp", CColor::Red()),
	b_platform("babyplatform.bmp", CColor::Red())
	
	// to initialise more sprites here use a comma-separated list
{
	// TODO: add initialisation here
	lives = 5;
	level = 1;
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{

	if (IsMenuMode() || IsGameOver()) return;

	Uint32 t = GetTime();

	for (CSprite* pDeath : player_death) pDeath->Update(t);
	for (CSprite* pSprite : coinList) pSprite->Update(t);

	// Gravity
	if ((m_state == AIRBORNE && m_state != CLIMBING))
	{
		m_player.Accelerate(0, -50);
	}


	// Run and Stand

	if ((m_state == STANDING || m_state == RUNNING) && (m_state != CLIMBING))
	{   // this is where all the code should go - that resets the player's velocity,
		m_player.SetVelocity(0, 0);
		// and then checks for the A and D keys being pressed,
		if ((IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT)))
		{
			m_player.Accelerate(-180, 0);
			if (m_state != RUNNING || m_side != LEFT)
				Lwalking.Play("walking.wav", 1),
				m_player.SetAnimation("run_left");
			Rwalking.Stop();
			m_state = RUNNING;
			m_side = LEFT;
		}
		else if ((IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT)))
		{
			m_player.Accelerate(180, 0);
			if (m_state != RUNNING || m_side != RIGHT)
				Rwalking.Play("walking.wav", 1),
				m_player.SetAnimation("run_right");
			Lwalking.Stop();
			m_state = RUNNING;
			m_side = RIGHT;
		}
		else
		{
			// and finally sets the STANDING state is no key is pressed.
			if (m_state == RUNNING)
				m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
			m_state = STANDING;
			Lwalking.Stop();
			Rwalking.Stop();
		}
	}


	// Pre-Update Position
	CVector v0 = m_player.GetPos();

	// Updates
	for (CSprite *pSprite : m_sprites)
		pSprite->Update(t);
	    m_player.Update(t);

	BarrelControl();
	for (CSprite* pBarrel : barrel_list) pBarrel->Update(t);

	// Checking for Collisions With Platforms
	int h = m_player.GetHeight() / 2;
	bool bTouchingPlatform = false;
	for (CSprite* pSprite : m_sprites)
	{
		if (m_player.HitTest(pSprite, 0))
		{
			// platforms 
			if (((string)pSprite->GetProperty("tag") == "platform") && m_state != CLIMBING || ((string)pSprite->GetProperty("tag") == "wall") && m_state != CLIMBING)
			{
				if (v0.m_y >= pSprite->GetTop())
				{
					bTouchingPlatform = true;
					m_player.SetVelocity(0, 0);
					m_player.SetY(pSprite->GetTop() + h);
				}

				else if (v0.m_y <= pSprite->GetBottom())
				{
					bTouchingPlatform = true;
					m_player.SetVelocity(0, 0);
					m_player.SetY(m_player.GetY() - h);
				}

				if (v0.m_x <= pSprite->GetLeft())
				{
					bTouchingPlatform = true;
					m_player.SetVelocity(0, 0);
					m_player.SetX(m_player.GetX() - h);
				}
				else if (v0.m_x >= pSprite->GetRight())
				{
					bTouchingPlatform = true;
					m_player.SetVelocity(0, 0);
					m_player.SetX(m_player.GetX() + h);
				}

			}

		}
	}

	// processing of airborne condition
	if ((m_state == AIRBORNE && bTouchingPlatform))
	{
		// just landed
		m_state = STANDING;
		m_player.SetImage(m_side == LEFT ? "stand_left" : "stand_right");
	}

	if ((m_state != AIRBORNE && !bTouchingPlatform))
	{
		// just taken off
		m_state = AIRBORNE;
		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}

	// Ladder
	bool bTouchingLadder = false;
	for (CSprite* pSprite : m_sprites)
	{
		if (m_player.HitTest(pSprite))
		{
			if ((string)pSprite->GetProperty("tag") == "ladder")
			{
				bTouchingLadder = true;
				m_player.SetVelocity(0, 0);
			}
		}

	}

	// processing of climbing condition
	if (m_state == AIRBORNE && bTouchingLadder)
	{
		m_state = CLIMBING;
		m_player.SetImage("hang");

	}

	if (m_state == STANDING && bTouchingLadder && ((IsKeyDown(SDLK_s)) || IsKeyDown(SDLK_DOWN)))
	{
		m_state = CLIMBING;
		m_player.SetImage("hang");
	}


	if (m_state == CLIMBING)
	{
		// player climbing
		if ((IsKeyDown(SDLK_w)) || IsKeyDown(SDLK_UP))
		{
			m_player.Accelerate(0, 100);
		}

		if ((IsKeyDown(SDLK_s)) || IsKeyDown(SDLK_DOWN))
		{
			m_player.Accelerate(0, -100);
		}

		if ((IsKeyDown(SDLK_a)) || IsKeyDown(SDLK_LEFT))
		{
			m_player.Accelerate(-50, 0);
		}

		if ((IsKeyDown(SDLK_d)) || IsKeyDown(SDLK_RIGHT))
		{
			m_player.Accelerate(50, 0);
		}

	}

	// Player Death / HitTest
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if (m_player.GetYVelocity() < -800)
				{
					CSprite* pDeath = new CSprite(m_player.GetX(), m_player.GetY(), 0, 0, GetTime());
					pDeath->LoadAnimation("playerdie.bmp", "death", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Black());
					pDeath->SetAnimation("death", 25);
					pDeath->Die(300);
					life.Play("land.wav");
					lives = lives - 1;
					player_death.push_back(pDeath);
					m_player.SetPos(80, 50);
				}

				if ((string)pSprite->GetProperty("tag") == "enemy" || (string)pSprite->GetProperty("tag") == "enemy2")
				{
					CSprite* pDeath = new CSprite(m_player.GetX(), m_player.GetY(), 0, 0, GetTime());
					pDeath->LoadAnimation("playerdie.bmp", "death", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Black());
					pDeath->SetAnimation("death", 25);
					pDeath->Die(300);
					fireburn.Play("fire.wav");
					lives = lives - 1;
					player_death.push_back(pDeath);
					m_player.SetPos(80, 50);
				}

				if ((string)pSprite->GetProperty("tag") == "hazard")
				{
					CSprite* pDeath = new CSprite(m_player.GetX(), m_player.GetY(), 0, 0, GetTime());
					pDeath->LoadAnimation("playerdie.bmp", "death", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Black());
					pDeath->SetAnimation("death", 25);
					pDeath->Die(300);
					spikedeath.Play("spikedeath.wav");
					lives = lives - 1;
					player_death.push_back(pDeath);
					m_player.SetPos(80, 50);
				}


				if ((string)pSprite->GetProperty("tag") == "deathplatform")
				{
					m_player.SetPos(80, 50);
					lose.Play("land.wav");
					lives = lives - 1;
				}
			}
			player_death.delete_if(deleted);
		}


		for (CSprite* pBarrel : barrel_list)
		{

			if (m_player.HitTest(pBarrel, 0))
			{
				if ((string)pBarrel->GetProperty("tag") == "barrel")
				{
					CSprite* pDeath = new CSprite(m_player.GetX(), m_player.GetY(), 0, 0, GetTime());
					pDeath->LoadAnimation("playerdie.bmp", "death", CSprite::Sheet(5, 1).Row(0).From(0).To(4), CColor::Black());
					pDeath->SetAnimation("death", 25);
					pDeath->Die(300);
					pBarrel->Delete();
					lose.Play("land.wav");
					lives = lives - 1;
					player_death.push_back(pDeath);
					m_player.SetPos(80, 50);
				}

			}
			player_death.delete_if(deleted);
		}


	// if player loses all their lives
		if (lives <= 0)
		{
			gameover.Play("gameover.wav");
			GameOver();
		}

		// Level 1 Skull Colliders X Value
		for (CSprite* pEnemy : m_sprites)
			if ((string)pEnemy->GetProperty("tag") == "enemy")
				for (CSprite* pCollider : m_sprites)
				{
					if ((string)pCollider->GetProperty("tag") == "lcollider"
						&& pEnemy->HitTest(pCollider, 0))
					{
						// Collision response code – left collider
						pEnemy->SetVelocity(100, 0);
						pEnemy->SetOmega(3.82 * 100);

					}
					else
						if ((string)pCollider->GetProperty("tag") == "rcollider"
							&& pEnemy->HitTest(pCollider, 0))
						{
							// Collision response code – right collider
							pEnemy->SetVelocity(-100, 0);
							pEnemy->SetOmega(-3.82 * 100);
						}
				}

		// Level 2 Skull Colliders Y value
		for (CSprite * pEnemy : m_sprites)
			if ((string)pEnemy->GetProperty("tag") == "enemy2")
				for (CSprite* pCollider : m_sprites)
				{
					if ((string)pCollider->GetProperty("tag") == "lcollider2"
						&& pEnemy->HitTest(pCollider, 0))
					{
						// Collision response code – left collider
						pEnemy->SetVelocity(0, 100);
						pEnemy->SetOmega(3.82 * 100);

					}
					else
						if ((string)pCollider->GetProperty("tag") == "rcollider2"
							&& pEnemy->HitTest(pCollider, 0))
						{
							// Collision response code – right collider
							pEnemy->SetVelocity(0, -100);
							pEnemy->SetOmega(-3.82 * 100);
						}
				}



		//Collecting Key
		for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if ((string)pSprite->GetProperty("tag") == "collectible")
				{
					GotKey = true;
					pSprite->Delete();
					keycollect.Play("keycollect.wav");
					doorunlock.Play("doorunlock.wav");

				}
			}
		}
		m_sprites.delete_if(deleted);

		//Collecting Coins
		for (CSprite* pSprite : coinList)
		{
			if (m_player.HitTest(pSprite, 1))
			{
				pSprite->Delete();
				score = score + 1;
				coin.Play("coin.wav");
			}

		}
		coinList.delete_if(deleted);

		// Barrel Cloning
		if (level == 1)
		{
			if (rand() % 580 == 0)
			{
				CSprite* pBarrel;
				pBarrel = new CSprite(360, 625, &barrels, CColor::White(), GetTime());
				pBarrel->SetProperty("tag", "barrel");
				pBarrel->SetMotion(-60, 0);
				pBarrel->Accelerate(0, -150);
				pBarrel->Die(58000);
				barrel_list.push_back(pBarrel);
			}
		}

		if (level == 2)
		{
			if (rand() % 520 == 0)
			{
				CSprite* pBarrel;
				pBarrel = new CSprite(460, 525, &barrels, CColor::White(), GetTime());
				pBarrel->SetProperty("tag", "barrel");
				pBarrel->SetMotion(-60, 0);
				pBarrel->Accelerate(0, -150);
				pBarrel->Die(58000);
				barrel_list.push_back(pBarrel);
			}
		}
		

		for (CSprite* pSprite : m_sprites)
		{
				if ((string)pSprite->GetProperty("tag") == "door" && GotKey == true)
				{
					pSprite->Delete();

				}
			
		}

		//Going to the next level

		 for (CSprite* pSprite : m_sprites)
		{
			if (m_player.HitTest(pSprite, 0))
			{
				if (((string)pSprite->GetProperty("tag") == "opendoor" && (GotKey == true) && score >= 8))
				{
					if ((IsKeyDown(SDLK_s)) || IsKeyDown(SDLK_DOWN))
					{
						NewLevel();
					}

				}
			}
		} 
		
		
}

void CMyGame::BarrelControl()
{
		for (CSprite* pBarrel : barrel_list)
		{

			// Barrel
			const float r = 12; // radius of the barrel
			Uint32 dt = GetDeltaTime();

			for (CSprite* pSprite : m_sprites)
			{
				string tag = pSprite->GetProperty("tag");

				for (CSprite* pBarrel : barrel_list)
				{


					if (tag == "platform" || tag == "wall")
					{
						CVector v = pBarrel->GetVelocity() * dt / 1000;
						CVector vc = pBarrel->GetVelocity();
						CVector t = pSprite->GetPos() - pBarrel->GetPos();
						x = pSprite->GetWidth() / 2;
						y = pSprite->GetHeight() / 2;


						if (v.m_y < 0)
						{

							f1 = (t.m_y + y + r) / v.m_y;
							f2 = (t.m_x - v.m_x * f1) / (x + r);

							if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
							{
								pBarrel->SetVelocity(Reflect(pBarrel->GetVelocity(), CVector(0, 1)));
								barrelbounce.Play("ballbounce.wav");
							}

						}

						if (v.m_y > 0)
						{
							f1 = (t.m_y - y - r) / v.m_y;
							f2 = (t.m_x - v.m_x * f1) / (x + r);

							if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
							{
								pBarrel->SetVelocity(Reflect(pBarrel->GetVelocity(), CVector(0, 1)));
								barrelbounce.Play("ballbounce.wav");
							}
						}

						if (v.m_x > 0)
						{

							f1 = (t.m_x - x - r) / v.m_x;
							f2 = (t.m_y - v.m_y * f1) / (y + r);

							if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
							{
								pBarrel->SetVelocity(Reflect(pBarrel->GetVelocity(), CVector(1, 0)));
								barrelbounce.Play("ballbounce.wav");
							}

						}

						if (v.m_x < 0)
						{

							f1 = (t.m_x + x + r) / v.m_x;
							f2 = (t.m_y - v.m_y * f1) / (y + r);

							if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
							{
								pBarrel->SetVelocity(Reflect(pBarrel->GetVelocity(), CVector(1, 0)));
								barrelbounce.Play("ballbounce.wav");
							}

						}
					}
				}
			}
		}
	}

void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode())
	{
		mainmenu.Draw(g);
		return;
	}

	if (IsGameOver())
	{
		game_over.Draw(g);
		return;
	}


	background.Draw(g);

	for (CSprite* pBarrel : barrel_list)
	{
		pBarrel->Draw(g);
	}

	for (CSprite* pSprite : m_sprites)
	{
		if ((string)pSprite->GetProperty("tag") != "rcollider"
			&& (string)pSprite->GetProperty("tag") != "lcollider" && (string)pSprite->GetProperty("tag") != "rcollider2"
			&& (string)pSprite->GetProperty("tag") != "lcollider2")
			pSprite->Draw(g);

		m_player.Draw(g);
		*g << font(16) << color(CColor::Black()) << xy(10, 660) << "coins: " << score;
		*g << font(16) << color(CColor::Black()) << xy(10, 680) << "key: " << GotKey;
		*g << font(16) << color(CColor::Black()) << xy(740, 680) << "lives: " << lives;
	}

	for (CSprite* pDeath : player_death)
	{
		pDeath->Draw(g);
	}

	for (CSprite* pSprite : coinList) pSprite->Draw(g);

}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_player.LoadImage("purpleplayer2.bmp", "stand_right", 10, 3, 0, 2, CColor::Red());
	m_player.LoadImage("purpleplayer2.bmp", "stand_left", 10, 3, 0, 1, CColor::Red());
	m_player.AddImage("purpleplayer2.bmp", "run_right", 10, 3, 1, 2, 8, 2, CColor::Red());
	m_player.AddImage("purpleplayer2.bmp", "run_left", 10, 3, 1, 1, 8, 1, CColor::Red());
	m_player.LoadImage("purpleplayer2.bmp", "jump_right", 10, 3, 9, 2, CColor::Red());
	m_player.LoadImage("purpleplayer2.bmp", "jump_left", 10, 3, 9, 1, CColor::Red());
	m_player.LoadImage("purpleplayer2.bmp", "crouch_right", 10, 3, 0, 2, CColor::Red());
	m_player.LoadImage("purpleplayer2.bmp", "crouch_left", 10, 3, 0, 1, CColor::Red());
	m_player.LoadImage("purpleplayer2.bmp", "hang", 10, 3, 1, 0, CColor::Red());
	m_player.AddImage("purpleplayer2.bmp", "climb", 10, 3, 0, 0, 1, 0, CColor::Red());
	
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
}

void CMyGame::SetupLevel1()
{

	CSprite* pSprite;

	int type, x, y;
	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level1.txt", ios_base::in);
	score = 0;
	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				pSprite = new CSprite(x, y, &big_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 2)
			{
				pSprite = new CSprite(x, y, &a_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 3)
			{
				pSprite = new CSprite(x, y, &s_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 4)
			{
				pSprite = new CSprite(x, y, "ladder2.png", CColor::Black(), GetTime());
				pSprite->SetProperty("tag", "ladder");
				m_sprites.push_back(pSprite);
			}

			if (type == 5)
			{
				pSprite = new CSpriteRect(x, y, 10, 40, CColor::Black(), CColor::White(), GetTime());
				pSprite->SetProperty("tag", "lcollider");
				m_sprites.push_back(pSprite);
			}

			if (type == 6)
			{
				pSprite = new CSpriteRect(x, y, 10, 40, CColor::Black(), CColor::White(), GetTime());
				pSprite->SetProperty("tag", "rcollider");
				m_sprites.push_back(pSprite);
			}

			if (type == 7)
			{
				pSprite = new CSprite(x, y, &spikes, CColor::Black(), GetTime());
				pSprite->SetProperty("tag", "hazard");
				m_sprites.push_back(pSprite);
			}

			if (type == 8)
			{
				pSprite = new CSprite(x, y, &fireballs, CColor::White(), GetTime());
				pSprite->SetProperty("tag", "enemy");
				pSprite->SetVelocity(100, 0);
				pSprite->SetOmega(3.82 * 100);
				m_sprites.push_back(pSprite);
			}

			if (type == 9)
			{
				pSprite = new CSprite(x, y, "key.bmp", CColor::White(), GetTime());
				pSprite->SetProperty("tag", "collectible");
				m_sprites.push_back(pSprite);
			}

			if (type == 10)
			{
				pSprite = new CSprite(x, y, "redcoin.bmp", CColor::White(), GetTime());
				pSprite->SetProperty("tag", "coins");
				coinList.push_back(pSprite);
			}

			if (type == 11)
			{
				pSprite = new CSpriteRect(x, y, 20, 1000, CColor::Black(), CColor::White(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}


		}
	} while (neof);
	myfile.close();

}

void CMyGame::SetupLevel2()
{

	CSprite* pSprite;

	int type, x, y;
	// declaring new file for reading in data
	fstream myfile;
	// opening file for reading
	myfile.open("level2.txt", ios_base::in);
	score = 0;
	// reading while the end of file has not been reached
	bool neof; // not end of file
	do
	{
		// read in data from file line by line
		myfile >> type >> x >> y;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << endl;
		if (neof)
		{
			if (type == 1)
			{
				pSprite = new CSprite(x, y, &big_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 2)
			{
				pSprite = new CSprite(x, y, &a_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 3)
			{
				pSprite = new CSprite(x, y, &s_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 4)
			{
				pSprite = new CSprite(x, y, "ladder2.bmp", CColor::Black(), GetTime());
				pSprite->SetProperty("tag", "ladder");
				m_sprites.push_back(pSprite);
			}

			if (type == 5)
			{
				pSprite = new CSpriteRect(x, y, 200, 20, CColor::Red(), CColor::White(), GetTime());
				pSprite->SetProperty("tag", "lcollider2");
				m_sprites.push_back(pSprite);
			}

			if (type == 6)
			{
				pSprite = new CSpriteRect(x, y, 200, 20, CColor::Red(), CColor::White(), GetTime());
				pSprite->SetProperty("tag", "rcollider2");
				m_sprites.push_back(pSprite);
			}

			if (type == 7)
			{
				pSprite = new CSprite(x, y, &spikes, CColor::Black(), GetTime());
				pSprite->SetProperty("tag", "hazard");
				m_sprites.push_back(pSprite);
			}

			if (type == 8)
			{
				pSprite = new CSprite(x, y, &fireballs, CColor::White(), GetTime());
				pSprite->SetProperty("tag", "enemy2");
				pSprite->SetVelocity(0, 100);
				pSprite->SetOmega(3.82 * 100);
				m_sprites.push_back(pSprite);
			}

			if (type == 9)
			{
				pSprite = new CSprite(x, y, "key.bmp", CColor::White(), GetTime());
				pSprite->SetProperty("tag", "collectible");
				m_sprites.push_back(pSprite);
			}

			if (type == 10)
			{
				pSprite = new CSprite(x, y, "redcoin.bmp", CColor::White(), GetTime());
				pSprite->SetProperty("tag", "coins");
				coinList.push_back(pSprite);
			}

			if (type == 11)
			{
				pSprite = new CSpriteRect(x, y, 20, 1000, CColor::Black(), CColor::White(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 12)
			{
				pSprite = new CSprite(x, y, &b_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "platform");
				m_sprites.push_back(pSprite);
			}

			if (type == 13)
			{
				pSprite = new CSprite(x, y, &big_platform, CColor::Red(), GetTime());
				pSprite->SetProperty("tag", "deathplatform");
				m_sprites.push_back(pSprite);
			}

		}
	} while (neof);
	myfile.close();

}
	

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
	// Clean up first
	for (CSprite *pSprite : m_sprites)
		delete pSprite;
	m_sprites.clear();
	barrel_list.clear();
	coinList.clear();
	player_death.clear();
	background.ClearImage();

	CSprite *pSprite;
	GotKey = false;
	score = 0;
	lives = 5;
	switch (nLevel)
	{
	case 1:// build Level 1 sprites
	
		level = 1;
		// spawn the player
		m_player.SetPos(80, 50);
		m_player.SetImage("stand_right");


		// spawn all other sprites here ...
		background.LoadImage("background.bmp", "level", CColor::Black());
		background.SetImage("level"); background.SetPosition(400, 350);

		//open door
		pSprite = new CSprite(430, 340, "gate_open.bmp", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "opendoor");
		m_sprites.push_back(pSprite);


		//door
		pSprite = new CSprite(430, 340, "gate.bmp", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "door");
		m_sprites.push_back(pSprite);

		//level spawn
		SetupLevel1();


		break;

	case 2:// Level 2 
		level = 2;
		m_player.SetPos(80, 50);
		m_player.SetImage("stand_right");

		background.LoadImage("background.bmp", "level", CColor::Black());
		background.SetImage("level"); background.SetPosition(400, 350);

		SetupLevel2();

		win.Play("win.wav");

		//open door
		pSprite = new CSprite(760, 50, "gate_open.bmp", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "opendoor");
		m_sprites.push_back(pSprite);



		//door
		pSprite = new CSprite(760, 50, "gate.bmp", CColor::White(), GetTime());
		pSprite->SetProperty("tag", "door");
		m_sprites.push_back(pSprite);

		break;

	case 3: //Win screen

						// spawn all other sprites here ...
		background.LoadImage("winscreen.bmp", "level", CColor::White());
		background.SetImage("level"); background.SetPosition(400, 350);
		win.Play("win.wav");

		break;
	}

	// any initialisation common to all levels
	m_player.SetImage("stand_right");
	m_player.SetVelocity(0, 0);
	m_state = STANDING;
	m_side = RIGHT;
}

// called when the game is over
void CMyGame::OnGameOver()
{

}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame(), pauseon.Play("pauseon.wav");
	if (sym == SDLK_F2)
		NewGame();

	// player jumping
	if ((sym == SDLK_w || sym == SDLK_UP) && (m_state == STANDING || m_state == RUNNING) && (m_state != CLIMBING))
	{
		m_player.Accelerate(0, 690);
		m_state = AIRBORNE;
		jump.Play("jump.wav");
		Lwalking.Stop();
		Rwalking.Stop();
		if (IsKeyDown(SDLK_a) || IsKeyDown(SDLK_LEFT))
			m_player.Accelerate(-220, 0);
		else if (IsKeyDown(SDLK_d) || IsKeyDown(SDLK_RIGHT))
			m_player.Accelerate(220, 0);

		m_player.SetImage(m_side == LEFT ? "jump_left" : "jump_right");
	}


}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (IsMenuMode()) StartGame();
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
