#include<Blit3D.h>
#include "GameMaster.h"

glm::vec2 rotateVector(glm::vec2 vector, float angle);
extern int level;

const glm::vec2 SPAWN_POSITIONS[] = {
	glm::vec2(1920, 1080),
	glm::vec2(960, 0),
	glm::vec2(1920, 0),
	glm::vec2(0, 1080),
	glm::vec2(0, 540),
	glm::vec2(0, 0),
};

GameMaster::GameMaster () {
	flock = new Flock();
}

void GameMaster::spawnAsteroid(std::vector<Asteroid>& asteroidList, Ship ship)
{
	
	if (asteroidToSpawn <= 0) return;

	rng.seed(rd());
	std::uniform_int_distribution<unsigned int> uint_ast_type(0, 2);
	std::uniform_int_distribution<unsigned int> uint_ast_pos(0, 5);
	int type = uint_ast_type(rng);
	int pos = uint_ast_pos(rng);
	Asteroid* asteroidInstance = new Asteroid(type, asteroidSprites);
	asteroidInstance->position = SPAWN_POSITIONS[pos];
	// shoot towards ship position
	glm::vec2 asteroidDirection = ship.position - asteroidInstance->position;
	asteroidInstance->velocity = glm::normalize(asteroidDirection);
	asteroidInstance->velocity *= asteroidInstance->speed;
	asteroidList.push_back(*asteroidInstance);
	asteroidToSpawn--;
	std::cout << asteroidToSpawn << "\n";
}

void GameMaster::spawnSubAsteroid(Asteroid& asteroid, std::vector<Asteroid>& asteroidList)
{
	asteroid.subAsteroidSpawn = true;
	if (asteroid.type == 0) return;

	Asteroid* asteroid1 = new Asteroid(asteroid.type-1, asteroidSprites);
	asteroid1->position = asteroid.position;
	glm::vec2 a1Direction = rotateVector(asteroid.velocity, 30);
	asteroid1->velocity = glm::normalize(a1Direction) * 200.f;

	Asteroid* asteroid2 = new Asteroid(asteroid.type-1, asteroidSprites);
	asteroid2->position = asteroid.position;
	glm::vec2 a2Direction = rotateVector(asteroid.velocity, 330);
	asteroid2->velocity = glm::normalize(a2Direction) * 200.f;

	asteroidList.push_back(*asteroid1);
	asteroidList.push_back(*asteroid2);
}

void GameMaster::spawnEnemyFlock(int flockNumber)
{
	flock->addFlock(flockNumber);
}

void GameMaster::resetAsteroidTimer()
{
	asteroidTimer = currAstSpawnTimer;
}

glm::vec2 rotateVector(glm::vec2 vector, float angle)
{
	angle = glm::radians(angle);
	float newX = vector.x * cos(angle) - vector.y * sin(angle);
	float newY = vector.y * sin(angle) + vector.y * cos(angle);
	return glm::vec2(newX, newY);
}

void GameMaster::resetGame(std::vector<Shot>& shotList, std::vector<Asteroid>& asteroidList, Ship& ship)
{
	resetAsteroidTimer();
	ship.reset();
	shotList = std::vector<Shot>();
	asteroidList = std::vector<Asteroid>();
	asteroidToSpawn = 2;
}

void GameMaster::updateTimers(double seconds) {
	asteroidTimer -= seconds;
	flockTimer -= seconds;
}

void GameMaster::resetFlockTimer() {
	flockTimer = currFlockTimer;
}