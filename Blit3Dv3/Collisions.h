#pragma once
#include"Ship.h"
#include"Asteroid.h"
#include"Flock.h"


namespace Collisions {
	bool ShotAsteroid(Asteroid& asteroid, Shot& shot);
	bool ShipAsteroid(Asteroid& asteroid, Ship& ship);
	bool ShotBoid(Boid& boid, Shot& shot);
	bool ShipBoid(Boid& boid, Ship& ship);
}