#pragma once

#include <glm.hpp>
#include <vector>

struct Particle 
{
	glm::vec3 position; //the particle's current position
	glm::vec3 velocity; //current velocity	
	glm::vec3 acceleration; //current Acceleration
	glm::vec3 accumForce; //accumulated force
	
	
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

	void integrate(float dt)
	{
		position += velocity * dt; //Update position: s = s + ds -> ds = v * dt
		
		glm::vec3 newAcceleration = accumForce * invMass; // a = F * (1/m) -> a = F * m^-1

		velocity = acceleration + newAcceleration * dt; //a = v/t --> v = a*t

		accumForce = glm::vec3(0); //reset applied forces
	}

private:
	float mass;
	float invMass; //the mass of the particle
};