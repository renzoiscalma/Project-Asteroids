#include "Flock.h"
#include<random>
std::mt19937 rng;

extern std::vector<Sprite*> enemyShipSprites;

Flock::Flock()
{

}

Flock::Flock(int flockSize)
{
	std::random_device rd;
	rng.seed(rd());
	std::uniform_real_distribution<float> percentDist(0.f, 1.f);
	std::uniform_int_distribution<u_int> zeroTo5(0, 5);
		
	cohesion_radius_squared = cohesion_radius * cohesion_radius;
	alignment_radius_squared = alignment_radius * alignment_radius;
	seperation_radius_squared = seperation_radius * seperation_radius;

	//create and randomize data for a bunch of boids
	for (int i = 0; i < flockSize; ++i)
	{
		Boid B;
		B.angle = percentDist(rng) * 360.f;
		B.dir.x = cos(glm::radians(B.angle));
		B.dir.y = sin(glm::radians(B.angle));
		B.position.x = 1920.f * percentDist(rng);
		B.position.y = 1080.f * percentDist(rng);
		B.frameNumber = zeroTo5(rng);
		boidList.push_back(B);
	}
}

void Flock::addFlock(int flockSize)
{
	std::random_device rd;
	rng.seed(rd());
	std::uniform_real_distribution<float> percentDist(0.f, 1.f);
	std::uniform_int_distribution<u_int> zeroTo5(0, 5);

	cohesion_radius_squared = cohesion_radius * cohesion_radius;
	alignment_radius_squared = alignment_radius * alignment_radius;
	seperation_radius_squared = seperation_radius * seperation_radius;

	//create and randomize data for a bunch of boids
	for (int i = 0; i < flockSize; ++i)
	{
		Boid B;
		B.angle = percentDist(rng) * 360.f;
		B.dir.x = cos(glm::radians(B.angle));
		B.dir.y = sin(glm::radians(B.angle));
		B.position.x = 1920.f * percentDist(rng);
		B.position.y = 1080.f * percentDist(rng);
		B.frameNumber = zeroTo5(rng);
		boidList.push_back(B);
	}
}

glm::vec2 Flock::Cohesion()
{
	glm::vec2 sumPosition = glm::vec2(0, 0);
	if (localBoidList.size() == 0) {
		return sumPosition;
	}
	for (auto& B : localBoidList)
	{
		sumPosition += boidList[B].virtual_position;
	}

	glm::vec2 avg = glm::vec2(sumPosition.x / localBoidList.size(),
		sumPosition.y / localBoidList.size());
	avg = avg - boidList[currentBoid].position;
	return glm::normalize(avg);
}

std::vector<Boid>* Flock::GetBoidList()
{
	return &boidList;
}

glm::vec2 Flock::Alignment()
{
	glm::vec2 sumDirection = glm::vec2(0, 0);
	for (auto& B : localBoidList)
	{
		sumDirection += boidList[B].dir;
	}
	return glm::normalize(sumDirection);
}

glm::vec2 Flock::Seperation()
{
	glm::vec2 newDir;
	glm::vec2 temp;

	for (auto& B : localBoidList)
	{
		temp = boidList[currentBoid].position - boidList[B].virtual_position; //make vector pointing from local boid towards current boid

		newDir += (seperation_radius / glm::length(temp)) * glm::normalize(temp);
	}

	newDir = glm::normalize(newDir);

	if (isnan(newDir.x) || isnan(newDir.y)) //could just check the x or the y
	{
		return boidList[currentBoid].dir;
		assert(false);
	}
	else
		return newDir;
}

float DistanceSquared(glm::vec2 A, glm::vec2 B)
{
	//delta x squared + delta y squared
	return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

void Flock::Update(float seconds)
{
	glm::vec2 cohesion, alignment, seperation;


	for (currentBoid = 0; currentBoid < boidList.size(); currentBoid++)
	{
		localBoidList.clear(); //get rid of any old list from previous update

		//populate local list of currentBoid for Cohesion
		for (int index = 0; index < boidList.size(); index++)
		{
			if (index == currentBoid) continue;
			//check if index boid is within local radius of currentBoid


			for (float x_offset = -1920.f; x_offset < 1921.f; x_offset += 1920.f)
				for (float y_offset = -1080.f; y_offset < 1081.f; y_offset += 1080.f)
				{
					glm::vec2 newPosition = boidList[index].position;
					newPosition.x += x_offset;
					newPosition.y += y_offset;
					float distance = DistanceSquared(boidList[currentBoid].position, newPosition);
					if (distance < cohesion_radius_squared)
					{
						boidList[index].virtual_position = newPosition;
						localBoidList.push_back(index); //add to local boid list
					}
				}

		}

		cohesion = Cohesion();

		//pare down the local boid list to just ones inside the alignment radius
		for (int i = localBoidList.size() - 1; i >= 0; --i)
		{
			//check if index boid is within local radius of currentBoid
			if (DistanceSquared(boidList[currentBoid].position, boidList[localBoidList[i]].virtual_position) >= alignment_radius_squared)
			{
				localBoidList.erase(localBoidList.begin() + i); //add to local boid list
			}
		}

		alignment = Alignment();

		//pare down the local boid list to just ones inside the seperation radius
		for (int i = localBoidList.size() - 1; i >= 0; --i)
		{
			//check if index boid is within local radius of currentBoid
			if (DistanceSquared(boidList[currentBoid].position, boidList[localBoidList[i]].virtual_position) >= seperation_radius_squared)
			{
				localBoidList.erase(localBoidList.begin() + i); //add to local boid list
			}
		}

		seperation = Seperation();

		glm::vec2 final = cohesion * cohesion_weight
			+ alignment * alignment_weight
			+ seperation * seperation_weight;

		final = glm::normalize(final);

		boidList[currentBoid].newDir = final; //instantaneus turn to face final dir
	}

	////apply all new directions to the boids
	//for (auto& B : boidList)
	//{
	//	B.Update(seconds);
	//}

	for (int i = boidList.size() - 1; i >= 0; --i) {
		if (!boidList[i].Update(seconds)) {
			std::cout << "dead";
			boidList.erase(boidList.begin() + i);
		}
	}
}

void Flock::Draw()
{
	for (auto& B : boidList) B.Draw();
}

bool Boid::Update(float seconds)
{
	if (dead) return false;
	TurnToHeading(newDir, seconds);

	position += dir * seconds * 100.f; //100 pixels/second speed
	thrustTimer += seconds;

	//animation timing
	if (thrustTimer >= 1.f / 20.f)
	{
		//change frames
		frameNumber++;
		if (frameNumber > 5)
			frameNumber = 1;

		thrustTimer -= 1.f / 20.f;
	}

	//wrap around
	if (position.x < 0) position.x += 1920;
	if (position.y < 0) position.y += 1080;
	if (position.x > 1920) position.x -= 1920;
	if (position.y > 1080) position.y -= 1080;
	return true;
}

void Boid::Draw()
{
	angle = glm::degrees(atan2f(dir.y, dir.x));
	enemyShipSprites[frameNumber]->angle = angle - 90;
	enemyShipSprites[frameNumber]->Blit(position.x, position.y);

}

void Boid::TurnToHeading(glm::vec2 desiredHeading, float seconds)
{
	//angle=atan2(-b*x+a*y,a*x+b*y)
	float difference = std::atan2f(-dir.y * desiredHeading.x + dir.x * desiredHeading.y, dir.x * desiredHeading.x
		+ dir.y * desiredHeading.y);
	difference = glm::degrees(difference);
	float amountAllowed = maxTurnSpeed * seconds;

	if (isnan(desiredHeading.x) || isnan(desiredHeading.y))
	{
		return;
	}

	if (abs(difference) <= amountAllowed)
	{
		//we are turning less than the amount allowed, so we can just
		//set our direction to the desired one
		angle += difference;
		dir = desiredHeading;
	}
	else
	{
		//we are trying to turn too far for the time passed,
		//so only change by the amount allowed in that time
		if (difference < 0) angle -= amountAllowed;
		else angle += amountAllowed;
		dir.x = cos(glm::radians(angle));
		dir.y = sin(glm::radians(angle));
	}
}