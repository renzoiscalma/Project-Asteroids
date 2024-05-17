#include"Collisions.h"

bool Collisions::ShotAsteroid(Asteroid& asteroid, Shot& shot)
{
	// guard, if asteroid has no hp
	if (asteroid.hitpoints <= 0) return false;
	// compute distance between asteroid and shot
	glm::vec2 positionDifference = asteroid.position - shot.position;
	float posDiffDot = glm::dot(positionDifference, positionDifference);
	float sumRadius = asteroid.hitbox + shot.radius;

	// if not colliding
	if (posDiffDot < sumRadius * sumRadius) return true;

	return false;
}

bool Collisions::ShipAsteroid(Asteroid& asteroid, Ship& ship)
{
	if (ship.lives <= 0) return false;
	glm::vec2 posDiff = asteroid.position - ship.position;
	float posDiffDot = glm::dot(posDiff, posDiff);
	float sumRadius = asteroid.hitbox + ship.radius;

	if (posDiffDot < sumRadius * sumRadius) return true;
	
	return false;
}

bool Collisions::ShotBoid(Boid& boid, Shot& shot)
{
	glm::vec2 positionDifference = boid.position - shot.position;
	float posDiffDot = glm::dot(positionDifference, positionDifference);
	float sumRadius = boid.hitbox + shot.radius;

	if (posDiffDot < sumRadius * sumRadius) return true;
	return false;
}

bool Collisions::ShipBoid(Boid& boid, Ship& ship)
{
	if (ship.lives <= 0) return false;
	glm::vec2 positionDifference = boid.position - ship.position;
	float posDiffDot = glm::dot(positionDifference, positionDifference);
	float sumRadius = boid.hitbox + ship.radius;

	if (posDiffDot < sumRadius * sumRadius) return true;
	return false;
}