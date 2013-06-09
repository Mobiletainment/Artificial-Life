#pragma once

#include <glm.hpp>
#include <vector>

typedef unsigned int ParticleID;

struct Particle 
{
	ParticleID uniqueID; //unique identifier for the particle
	glm::vec3 position; //the particle's current position
	glm::vec3 velocity; //current velocity	
	glm::vec3 acceleration; //current Acceleration
	glm::vec3 accumForce; //accumulated force
	glm::vec3 color; //make it colorful
	
	Particle()
	{
	}

	Particle(ParticleID ID)
	{
		uniqueID = ID;
	}
	
	float life;

	void setMass(float mass)
	{
		this->mass = mass;
		invMass = 1.0f / mass; //pre-calculate for better performance
	}

	float getMass()
	{
		return mass;
	}

	inline void integrate(float dt)
	{
		position += velocity * dt; //Update position: s = s + ds -> ds = v * dt
		
		acceleration += accumForce * invMass; // a = F * (1/m) -> a = F * m^-1

		velocity = acceleration * dt; //a = v/t --> v = a*t

		accumForce = glm::vec3(0); //reset applied forces
	}

private:
	float mass;
	float invMass; //the mass of the particle
};