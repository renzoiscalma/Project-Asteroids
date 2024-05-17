	#pragma once
#include<Blit3D.h>

class Asteroid
{
private:
	static const float BASE_HITBOX_RADIUS;
	static const float BASE_SPRITE_RADIUS;
	static const float RADIUS_MULT[];
	static const float ASTEROID_SPEED_MULT[];
	static const int HIT_POINT[];
	static const int SPIN_RATE[];
 public:
	glm::vec2 velocity, position;
	std::vector<Sprite*> sprites;
	float hitbox;
	float spriteRadius;
	float angle;
	int hitpoints;
	int type;
	int frameNumber;
	float deathTimer;
	float speed;
	bool subAsteroidSpawn;
	
	void Draw();
	bool Update(float seconds);
	Asteroid(int type, std::vector<Sprite*> asteroidSprites);
};