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
			if (glm::length((*it)->accumForce) > 100.0f)
			{
				(*it)->accumForce = glm::normalize((*it)->accumForce) * 100.0f;
			}
			cout << (*it)->uniqueID << ": " << glm::length((*it)->accumForce) << " -> " << glm::length((*it)->accumForce) * pow(damping, dt) << endl;
			(*it)->accumForce *= pow(damping, dt);

		}

	}

	




};