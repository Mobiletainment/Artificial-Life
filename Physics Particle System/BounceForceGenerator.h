#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

class BounceForceGenerator : public ForceGenerator
{
public:
	float damping; //cheap and dirty member access

	BounceForceGenerator(float damping = -0.3f)
	{
		this->damping = damping;
	}

	

	void Update(float dt)
	{
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			ApplyTo(*it);	//apply force
		}
	}

	void ApplyTo(Particle *particle)
	{
		//Bump on floor, bounce in invert direction but with damping
		particle->velocity.y = particle->velocity.y * damping;
		particle->accumForce.y = particle->accumForce.y * damping; //invert forces
	}

};