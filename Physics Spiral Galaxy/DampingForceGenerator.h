#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"


class DampingForceGenerator : public ForceGenerator
{
public:

	glm::vec3 center;
	float damping;


	void Update(float dt)
	{
		//glm::vec3 = _center = glm::vec3(width * 0.5f, height * 0.5f, 0.0f);
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			cout << glm::length((*it)->accumForce) << endl;
			
			float lengthSquared = (*it)->accumForce.x * (*it)->accumForce.x + (*it)->accumForce.y * (*it)->accumForce.y;
			if (lengthSquared > 20000)
			{
				cout << "damping" << endl;
				(*it)->accumForce = normalize((*it)->accumForce) * 40.0f;

			}
			//cout << (*it)->uniqueID << ": " << glm::length((*it)->accumForce) << " -> " << glm::length((*it)->accumForce) * pow(damping, dt) << endl;
			//(*it)->accumForce *= pow(damping, dt);

		}

	}

	




};