#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"


class SpiralForceGenerator : public ForceGenerator
{
public:

	glm::vec3 center;
	float damping;


	void Update(float dt)
	{
		//glm::vec3 = _center = glm::vec3(width * 0.5f, height * 0.5f, 0.0f);
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			glm::vec3 direction = center - (*it)->position;
			direction = normalize(direction);
			glm::vec3 normalDirection = glm::vec3(-direction.y, direction.x, 0);
		
			//(*it)->accumForce += normalDirection * (*it)->getMass() * 0.5f; //apply rotation for spiral galaxy


		}

	}

	




};