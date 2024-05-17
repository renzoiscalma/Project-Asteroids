#include "Ship.h"

extern bool shoot;
extern std::vector<Sprite*> explosionSprites;
void Shot::Draw()
{
	sprite->Blit(position.x, position.y);
}

bool Shot::Update(float seconds)
{
	if (currentLifeTime > timeToLive) return false;
	else currentLifeTime += seconds;
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	return true;
}

void Ship::Draw()
{
	if (lives <= 0) return;
	//change ship angle because my graphics face "up", not "right"
	spriteList[frameNumber]->angle = angle - 90;
	
	//draw main ship sprite
	spriteList[frameNumber]->Blit(position.x, position.y);
	
	if (invulnerable)
		spriteList[frameNumber]->Blit(2.f);

	//redraw if too close to an edge
	//left
	if(position.x < radius + 10.f) spriteList[frameNumber]->Blit(position.x + 1920.f, position.y);
	//right
	if (position.x > 1920.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x - 1920.f, position.y);
	//down
	if (position.y < radius + 10.f) spriteList[frameNumber]->Blit(position.x, position.y + 1080.f);
	//up
	if (position.y > 1080.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x, position.y - 1080.f);

	//copies for 4 diagonal corners
	spriteList[frameNumber]->Blit(position.x + 1920.f, position.y + 1080.f);
	spriteList[frameNumber]->Blit(position.x - 1920.f, position.y - 1080.f);
	spriteList[frameNumber]->Blit(position.x - 1920.f, position.y + 1080.f);
	spriteList[frameNumber]->Blit(position.x + 1920.f, position.y - 1080.f);
	//show hp
	for (int i = 0; i < 3; i++) {
		if (i < lives)
			lifeSprite[0]->Blit(50 + (i * 55), 1040, 3.f, 3.f);
		else
			lifeSprite[4]->Blit(50 + (i * 55), 1040, 3.f, 3.f);
	}
}

void Ship::Update(float seconds)
{
	//handle turning
	if (turningLeft)
	{
		angle += 180.f * seconds;
	}

	if (turningRight)
	{
		angle -= 180.f * seconds;
	}

	if (thrusting)
	{
		//calculate facing vector
		float radians = angle * (M_PI / 180);
		glm::vec2 facing;
		facing.x = std::cos(radians);
		facing.y = std::sin(radians);

		facing *= seconds * 400.f;

		velocity += facing;

		//check if over max speed
		if (velocity.length() > 600.f)
		{
			velocity = glm::normalize(velocity) * 600.f;
		}

		thrustTimer += seconds;
		
		//animation timing
		if (thrustTimer >= 1.f / 20.f)
		{
			//change frames
			frameNumber++;
			if (frameNumber > 3)
				frameNumber = 1;

			thrustTimer -= 1.f / 20.f;
		}
	}
	else frameNumber = 0;

	//update position
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	//reduce velocity due to "space friction"
	float scale = 1.f - seconds * 0.5f;
	velocity *= scale;

	//velocity could potentialy get very small:we should
	//cap it to zero when it gets really close,
	//but that is a more advanced topic that
	//requires techniques like std::isnan()
	float length = glm::length(velocity);
	if(std::isnan(length) || length < 0.00001f)
		velocity = glm::vec2(0, 0);

	//handle shot timer
	if (shoot) {
		if (shotTimer > shotInterval) shotTimer = 0;	// reset shotTimer if shotTimer > shotInterval
		else if (firstShot) shotTimer += seconds;		// accumulate after first shot
		else firstShot = true;							// toggle first shot to true if first time shooting
	}
	else {
		shotTimer = 0;
		firstShot = false;
	}

	// handle invlunerable timer
	if (invulnerableTimer <= 0) {
		invulnerable = false;
	}
	else {
		invulnerableTimer -= seconds;
	}
}

bool Ship::Shoot(std::vector<Shot> &shotList)
{
	if(shotTimer > 0) return false;
	shotTimer = 0;
	Shot* shotInstance = new Shot;
	shotInstance->sprite = shotSprite;
	shotInstance->position = position;
	glm::vec2 shotDirection = glm::vec2(cos(glm::radians(angle)), sin(glm::radians(angle)));
	shotInstance->velocity = glm::normalize(shotDirection);
	shotInstance->velocity *= 900.f;
	shotInstance->velocity += velocity;
 	shotList.push_back(*shotInstance);

	return true;
}

void Ship::reset() {
	this->angle = 90;
	this->shotTimer = 0;
	this->shotInterval = 0.30;
	this->radius = 27.f;
	this->lives = 3;
	this->frameNumber = 0;
	this->thrustTimer = 0;
	this->thrusting = false;
	this->turningLeft = false;
	this->turningRight = false;
	this->firstShot = false;
	this->invulnerable = true;
	this->invulnerableTimer = 2;
	this->position = glm::vec2(1920.f / 2, 1080.f / 2);
	this->velocity = glm::vec2(0, 0);
}

void Explosion::Draw()
{
	explosionSprites[frameNumber]->Blit(position.x, position.y, size, size);
}

bool Explosion::Update(float seconds)
{
	if (frameNumber >= 8) return false;
	timer += seconds;
	if (timer >= 1 / 10.f) {
		timer -= 1 / 10.f;
		frameNumber++;
	}
	return true;
}

Explosion::Explosion(glm::vec2 position, float size)
{
	this->position = position;
	this->size = size;
}