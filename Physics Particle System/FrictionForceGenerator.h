#pragma once

#include <glm.hpp>
#include "ForceGenerator.h"

//ist auf unsere Schiefe Plane zugeschneidert, needs refactoring ;)
class FrictionForceGenerator : public ForceGenerator
{
public:
	float us; //Haftreibung
	float uk; //Gleitreibung
	glm::vec3 gravity; //Zur Berechnung der Normalkraft benötigt
	float cosTheta; //Winkel der Ebene
	float sinTheta; //Versuch

	void Update(float dt)
	{
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			ApplyTo(*it);
		}
	}

	void ApplyTo(Particle *particle)
	{
		float Fn = particle->getMass() * gravity.y * cosTheta;
		particle->accumForce.y *= us * Fn; //Ffriction = u * Fn //Haftreibung
		particle->accumForce.x *= uk * particle->getMass() * gravity.y * sinTheta; //Gleitreibung (Faked)
	}

};