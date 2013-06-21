#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

const float G = 100.67f;

class UniversalGravitationForceGenerator : public ForceGenerator
{
public:


	UniversalGravitationForceGenerator()
	{

	}

	void Update(float dt)
	{
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			/*
			Particle *particle = *it;
			float a = dt * 0.05 * (0.10f - glm::length(particle->velocity)) * particle->getMass();
			particle->position.x = particle->position.x * glm::cos(a) - particle->position.y * glm::sin(a);
			particle->position.y = particle->position.y * glm::cos(a) - particle->position.x * glm::sin(a);
			*/

			//Newton's Law of Universal Gravitation
			//F12 = -G * m1*m2 * r12 / |r12|^2

			for (iterator other = _particles.begin(); other != _particles.end(); ++other)
			{
				if (it != other)
				{
					float distance = glm::length((*other)->position - (*it)->position);
					float distaneSquared = distance * distance;
					glm::vec3 direction = (*other)->position - (*it)->position;
					direction = glm::normalize(direction);
					glm::vec3 attraction = (*it)->getMass() * (*other)->getMass() * G * direction / distaneSquared; //F = m * g
					(*it)->accumForce += attraction;
				}
			}
		}
	}




};