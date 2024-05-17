#pragma once
#include"Asteroid.h"
#include"Ship.h"
#include "Flock.h"
#include <random>
class GameMaster
{
public:
	GameMaster();
	std::mt19937 rng;
	std::random_device rd;
	float currAstSpawnTimer = 5.f;
	float asteroidToSpawn = 2;
	float asteroidTimer = 0;
	float flockTimer = 0;
	float currFlockTimer = 2.f;
	std::vector<Sprite*> asteroidSprites;
	Flock* flock;
	void spawnAsteroid(std::vector<Asteroid>& asteroidList, Ship ship);
	void spawnSubAsteroid(Asteroid& asteroid, std::vector<Asteroid>& asteroidList);
	void spawnEnemyFlock(int flockSize);
	void resetAsteroidTimer();
	void resetGame(std::vector<Shot>& shotList, std::vector<Asteroid>& asteroidList, Ship& ship);
	void updateTimers(double seconds);
	void resetFlockTimer();
};