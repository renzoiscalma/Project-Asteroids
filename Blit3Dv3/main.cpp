/*
	Asteroids ship movement example
*/
//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"

#include"Ship.h"
#include"Asteroid.h"
#include"GameMaster.h"
#include"Collisions.h" 
#include "Flock.h"

Blit3D *blit3D = NULL;

//GLOBAL DATA
bool shoot = false;
bool gameOver = false;
int score = 0;
double elapsedTime = 0;
double gameTimer = 0;
float timeSlice = 1.f / 120.f;
int level = 1;

GameMaster* gameMaster = NULL;
Sprite* backgroundSprite = NULL;
Ship* ship = NULL;


std::vector<Sprite*> explosionSprites;
std::vector<Sprite*> enemyShipSprites;
std::vector<Shot> shotList;
std::vector<Asteroid> asteroidList;
std::vector<Explosion> explosionList;

AngelcodeFont* caviar57font = NULL;


void Init()
{
	// init game master
	gameMaster = new GameMaster();
	//turn cursor off
	blit3D->ShowCursor(false);

	//load our background image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.
	backgroundSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\back.png");

	// load enemy ship sprite
	for (int i = 0; i < 6; ++i)
		enemyShipSprites.push_back(blit3D->MakeSprite(i * 60, 0, 60, 109, "Media\\Ship.png"));

	//create a ship
	ship = new Ship;
	//load a sprite off of a spritesheet
	for (int i = 0; i < 4; ++i)
		ship->spriteList.push_back(blit3D->MakeSprite(i * 72, 0, 72, 88, "Media\\Player_Ship2c.png"));

	//load the shot graphic
	ship->shotSprite = blit3D->MakeSprite(0, 0, 8, 8, "Media\\shot.png");
	
	// load asteroid graphic
	// load asteroid death animation
	for (int i = 0; i < 7; i++)
		gameMaster->asteroidSprites.push_back(
			blit3D->MakeSprite(i * 96, 0, 96, 96, "Media\\Asteroid_Sprites.png"));
	// load hitpoints sprite
	for (int i = 0; i < 5; i++)
		ship->lifeSprite.push_back(
			blit3D->MakeSprite(i * 17, 0, 17, 17, "Media\\heart.png")
		);

	// load explosion sprites
	for (int i = 0; i < 9; i++)
		explosionSprites.push_back(
			blit3D->MakeSprite(i * 32, 0, 32, 32, "Media\\Explosion.png")
		);

	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);

	//load an Angelcode binary32 font file
	caviar57font = blit3D->MakeAngelcodeFontFromBinary32("Media\\CaviarDreams57.bin");

	//set the clear colour
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a 	
}

void DeInit(void)
{
	if (ship != NULL) delete ship;
	if (gameMaster != NULL) delete gameMaster;
	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	bool scoreChanged = false;
	//only update time to a maximun amount - prevents big jumps in 
	//the simulation if the computer "hiccups"
	if (seconds < 0.15)
	{
		elapsedTime += seconds;
		gameTimer += seconds;
		gameMaster->updateTimers(seconds);
	}
	else {
		elapsedTime += 0.15;
		gameTimer += 0.15;
		gameMaster->updateTimers(seconds);
	}
	//update by a full timeslice when it's time
	while (elapsedTime >= timeSlice)
	{
		elapsedTime -= timeSlice;
		ship->Update(timeSlice);
		gameMaster->flock->Update(timeSlice);
		if (shoot) ship->Shoot(shotList);

		if (gameMaster->asteroidTimer <= 0) {
			gameMaster->spawnAsteroid(asteroidList, *ship);
			gameMaster->resetAsteroidTimer();
		}

		if (gameMaster->flockTimer <= 0 && gameMaster->flock->GetBoidList()->size() <= 15) {
			gameMaster->spawnEnemyFlock(5);
			gameMaster->resetFlockTimer();
		}

		if (ship->lives <= 0)
			gameOver = true;

		//iterate backwards through the shotlist,
		//so we can erase shots without messing up the vector
		//for the next loop
		for (int i = shotList.size() - 1; i >= 0; --i)
		{
			//shot Update() returns false when the bullet should be killed off
			if (!shotList[i].Update(timeSlice))
				shotList.erase(shotList.begin() + i);
		}
		// apply to asteroids as well
		for (int i = asteroidList.size() - 1; i >= 0; --i)
		{
			if (asteroidList[i].hitpoints <= 0 && 
				!asteroidList[i].subAsteroidSpawn) {
				gameMaster->spawnSubAsteroid(asteroidList[i], asteroidList);
			}
 			if (!asteroidList[i].Update(timeSlice))
				asteroidList.erase(asteroidList.begin() + i);

		}
		// apply to explosions
		for (int i = explosionList.size() - 1; i >= 0; --i)
		{
			if (!explosionList[i].Update(timeSlice))
				explosionList.erase(explosionList.begin() + i);
		}
		
		// check for collisions
		for (auto& asteroid : asteroidList) {
			// check for asteroid shot collision
			for (auto& shot : shotList) {
				if (Collisions::ShotAsteroid(asteroid, shot)) {
					shot.currentLifeTime = FLT_MAX;
					asteroid.hitpoints -= 1;
					explosionList.push_back(Explosion(shot.position, 0.5f));

					if (asteroid.hitpoints <= 0) {
						score++;
						scoreChanged = true;
					}
				}
			}
			// check for asteroid ship collision
			if (Collisions::ShipAsteroid(asteroid, *ship) && !ship->invulnerable) {
				ship->lives--;
				asteroid.hitpoints = 0;
				ship->invulnerable = true;
				ship->invulnerableTimer = 2;
				explosionList.push_back(Explosion(ship->position, 2.f));
			}
		}

		for (auto& boid : *gameMaster->flock->GetBoidList()) {
			for (auto& shot : shotList) {
				if (Collisions::ShotBoid(boid, shot)) {
					std::cout << "shot boid";
					shot.currentLifeTime = FLT_MAX;
					boid.dead = true;
					explosionList.push_back(Explosion(shot.position, 2.f));
					score++;
				}
			}

			// check for boid ship collision
			if (Collisions::ShipBoid(boid, *ship) && !ship->invulnerable) {
				ship->lives--;
				boid.dead = true;
				ship->invulnerable = true;
				ship->invulnerableTimer = 2;
				explosionList.push_back(Explosion(ship->position, 2.f));
			}
		}

		// check if level increment
		if (gameMaster->asteroidToSpawn == 0 && asteroidList.size() == 0) {
			level++;
			gameMaster->currAstSpawnTimer -= 0.25f;
			gameMaster->asteroidToSpawn = level * 3;
		}
	}
}

void Draw(void)
{
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw stuff here

	//draw the background in the middle of the screen
	//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at, 
	//starting as 0,0 in the bottom-left corner.

	backgroundSprite->Blit(1920.f / 2, 1080.f / 2);
	
	std::string scoreText = "SCORE: " + std::to_string(score);
	float scoreTextWidth = caviar57font->WidthText(scoreText);
	caviar57font->BlitText((1920.f - scoreTextWidth - 250), 1040.f, scoreText);

	std::string levelText = "LEVEL: " + std::to_string(level);
	float levelTextWidth = caviar57font->WidthText(levelText);
	caviar57font->BlitText((1920.f - levelTextWidth - 250)/2, 1040.f, levelText);

	//draw the ship
	ship->Draw();

	//draw the shots
	for (auto& S : shotList) S.Draw();

	//draw the asteroids
	for (auto& A : asteroidList) A.Draw();

	//draw explosions
	for (auto& E : explosionList) E.Draw();

	// todo change to another place
	gameMaster->flock->Draw();

	if (gameOver) {
		float fontWidth = caviar57font->WidthText("GAME OVER! PRESS SPACE TO RESTART");
		caviar57font->BlitText((1920.f - fontWidth) / 2, 1080.f / 2, "GAME OVER! PRESS SPACE TO RESTART");
		return;
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		blit3D->Quit(); //start the shutdown sequence

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		ship->turningLeft = true;

	if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		ship->turningLeft = false;

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		ship->turningRight = true;

	if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		ship->turningRight = false;

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		ship->thrusting = true;

	if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		ship->thrusting = false;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (gameOver) {
			gameMaster->resetGame(shotList, asteroidList, *ship);
			score = 0;
			level = 1;
			gameOver = false;
		}
		shoot = true;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) 
		shoot = false;
}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::DECORATEDWINDOW, 1920, 1080);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}