#pragma once
#include "Blit3d.h"

class Boid;

class Flock
{
	std::vector<Boid> boidList;

	int currentBoid = 0;
	float cohesion_radius = 100.f;
	float alignment_radius = 200.f;
	float seperation_radius = 300.f;

	float cohesion_radius_squared;
	float alignment_radius_squared;
	float seperation_radius_squared;

	float cohesion_weight = .5f;
	float alignment_weight = 0.8f;
	float seperation_weight = 1.f;

	std::vector<int> localBoidList;

	glm::vec2 Cohesion();
	glm::vec2 Alignment();
	glm::vec2 Seperation();

public:
	void Update(float seconds);
	void Draw();
	std::vector<Boid>* GetBoidList();
	Flock();
	Flock(int flockSize);
	void addFlock(int flockSize);
};

class Boid
{
public:
	glm::vec2 position;
	glm::vec2 virtual_position;
	float angle;
	glm::vec2 dir; //unit vector pointing in the direction this boid is facing
	glm::vec2 newDir; //temp direction
	float maxTurnSpeed = 290.f;//degrees per second
	int frameNumber = 0;
	float thrustTimer = 0;
	float hitbox = 20;
	bool dead = false;
	void TurnToHeading(glm::vec2 desiredHeading, float seconds);
	bool Update(float seconds);
	void Draw();
};