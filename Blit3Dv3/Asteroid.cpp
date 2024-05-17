#include "Asteroid.h";
#include "GameMaster.h";

extern GameMaster* gameMaster;

const float Asteroid::BASE_HITBOX_RADIUS = 17;
const float Asteroid::BASE_SPRITE_RADIUS = 48;
const float Asteroid::RADIUS_MULT[] = { 2, 4, 8 };
const int Asteroid::HIT_POINT[] = { 1, 2, 3 };
const float Asteroid::ASTEROID_SPEED_MULT[] = {200, 150, 70};
const int Asteroid::SPIN_RATE[] = {100, 50, 10};

Asteroid::Asteroid(int type, std::vector<Sprite*> asteroidSprites)
{
	this->hitbox = BASE_HITBOX_RADIUS * RADIUS_MULT[type];
	this->spriteRadius = BASE_SPRITE_RADIUS * RADIUS_MULT[type];
	this->hitpoints = HIT_POINT[type];
	this->speed = ASTEROID_SPEED_MULT[type];
	this->type = type;
	this->sprites = asteroidSprites;
	this->deathTimer = 0;
	this->frameNumber = 0;
	this->subAsteroidSpawn = false;
	this->angle = 90;
}
void Asteroid::Draw()
{
	float percent = 0.01f;
	float circumference = static_cast<float>(spriteRadius * 2);
	float scale = circumference * percent;
	sprites[frameNumber]->angle = angle;
	for (float xoffset = -1920; xoffset <= 1920; xoffset += 1920)
	{
		for (float yoffset = -1080; yoffset <= 1080; yoffset += 1080)
		{
			sprites[frameNumber]->Blit(position.x + xoffset, position.y + yoffset, scale, scale);
		}
	}
}

bool Asteroid::Update(float seconds)
{
	// handle death animation
	if (hitpoints <= 0 && frameNumber < 6)
	{
		// update death animation
		deathTimer += seconds;
		if (deathTimer >= 1.f / 20.f) {
			frameNumber++;
			deathTimer -= 1.f / 20.f;
		}
		
	} if (hitpoints <= 0 && frameNumber >= 6) {
		return false;
	}

	// handle change in frameNumber depending on hp
	if (type == 2) {						// large asteroid
		if (hitpoints == 2)
			frameNumber = 1;
		else if (hitpoints == 1)
			frameNumber = 2;
	}
	if (type == 1 && hitpoints == 1) {		// medium asteroid
		frameNumber = 1;
	}

	//move the asteroid
	position += velocity * seconds;

	angle += SPIN_RATE[type] * seconds;

	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	return true;
}
