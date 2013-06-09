#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

class FrictionForceGenerator : public ForceGenerator
{
public:

	glm::vec3 gravity; //cheap and dirty member access

	void Update(float dt)
	{
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			(*it)->accumForce += (*it)->getMass() * gravity; //F = m * g
		}
	}

};