#pragma once
#include<Blit3D.h>

class Shot
{
public:
	glm::vec2 velocity, position;
	Sprite *sprite = NULL;
	float timeToLive = 1.05f; 
	float currentLifeTime = 0.f;
	float radius = 4.f;
	void Draw();
	bool Update(float seconds); //return false if shot dead (timeToLive <= 0)
};

class Explosion
{
public: 
	Sprite* sprite = NULL;
	glm::vec2 position;
	int frameNumber = 0;
	float timer = 0;
	float size = 0;
	void Draw();
	bool Update(float seconds);
	Explosion(glm::vec2 position, float size);
};

class Ship
{
public:
	Sprite* shotSprite;
	std::vector<Sprite*> lifeSprite;
	std::vector<Sprite*> spriteList;
	std::vector<Sprite*> explosionSprite;
	glm::vec2 velocity, position;
	float angle = 0;
	float shotTimer = 0;
	float shotInterval = 0.25;
	float radius = 27.f;
	int lives = 3;
	int frameNumber = 0;
	float thrustTimer = 0;
	bool thrusting = false;
	bool turningLeft = false;
	bool turningRight = false;
	bool firstShot = false;
	bool invulnerable = true;
	float invulnerableTimer = 2;

	void Draw();
	void Update(float seconds);
	bool Shoot(std::vector<Shot> &shotList);
	void reset();
};