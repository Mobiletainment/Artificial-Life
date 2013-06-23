#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

const float G = 6.674e-11;

class UniversalGravitationForceGenerator : public ForceGenerator
{
public:

	UniversalGravitationForceGenerator()
	{

	}

	void Update(float dt)
	{
		//glm::vec3 = _center = glm::vec3(width * 0.5f, height * 0.5f, 0.0f);
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			//Newton's Law of Universal Gravitation
			//F12 = -G * m1*m2 * r12 / |r12|^2
			float max = 0.0f;
			for (iterator other = _particles.begin(); other != _particles.end(); ++other)
			{
				if (it != other)
				{
					glm::vec3 direction = (*other)->position - (*it)->position;
					float distanceSquared = direction.x * direction.x + direction.y * direction.y;

					if (distanceSquared > 10000.0f)
						distanceSquared = 10000.0f;

					glm::vec3 attraction = glm::vec3(0);

					if (distanceSquared > 0.0f)
					{
						direction = normalize(direction);
						attraction = ((*it)->getMass() * G) * (*other)->getMass()  * direction / distanceSquared; //F = m * g
					
						(*it)->accumForce += attraction; //apply Newton's universal law of gravity
					}
					//cout << (*it)->uniqueID << " -> " << (*other)->uniqueID << ": " << glm::length(attraction) << endl;
					//++i;
				}
			}

			//cout << max << endl;
		}
	}






};