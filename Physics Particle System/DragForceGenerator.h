//See Game Physics Engine Development: Chapter 5, Adding General Forces, p. 78
#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

class DragForceGenerator : public ForceGenerator
{
public:

	float k1; //the velocity drag coefficient
	float k2; //the velocity squared drag coefficient

	DragForceGenerator()
	{
		k1 = 0.0f;
		k2 = 0.0f;
	}

	void Update(float dt)
	{
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			glm::vec3 force = (*it)->velocity;
			float dragCoeff = glm::length(force);

			dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;

			normalize(force);

			force *= -dragCoeff;
			(*it)->accumForce += force;
		}
	}

	inline void normalize(glm::vec3 &vector)
	{
		if (glm::length(vector) > 0) //normalize the velocity for the specified direction
			glm::normalize(vector);
	}

};