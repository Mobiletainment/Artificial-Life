#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

const float G = 6.674e-9;

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
			(*it)->accumForce = glm::vec3(0);
			//Newton's Law of Universal Gravitation
			//F12 = -G * m1*m2 * r12 / |r12|^2
			float max = 0.0f;
			for (iterator other = _particles.begin(); other != _particles.end(); ++other)
			{
				if (it != other)
				{
					glm::vec3 direction = (*other)->position - (*it)->position;
					float distance = glm::length(direction);
					float distanceSquared = distance * distance;

					direction = normalize(direction);

					glm::vec3 normalDirection = glm::vec3(-direction.y, direction.x, 0);

					
					glm::vec3 attraction = ((*it)->getMass() * G) * (*other)->getMass()  * direction / distanceSquared; //F = m * g
					
					
					//(*it)->accumForce += normalDirection * (*it)->getMass() * 0.1f; //apply rotation for spiral galaxy
					
					(*it)->accumForce += attraction; //apply Newton's universal law of gravity
					//cout << (*it)->uniqueID << " -> " << (*other)->uniqueID << ": " << glm::length(attraction) << endl;
					//++i;
				}
			}

			//cout << max << endl;
		}
	}

	inline glm::vec3 normalize(glm::vec3 &vector)
	{
		if (glm::length(vector) > 0) //normalize the velocity for the specified direction
			return glm::normalize(vector);

		return vector;
	}




};