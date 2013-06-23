#pragma once

#include <glm.hpp>
#include <vector>

typedef unsigned int ParticleID;
const float damping = 0.2f;
	

struct Particle 
{
	ParticleID uniqueID; //unique identifier for the particle
	glm::vec3 position; //the particle's current position
	glm::vec3 velocity; //current velocity	
	//glm::vec3 acceleration; //current Acceleration
	glm::vec3 accumForce; //accumulated force
	glm::vec3 color; //make it colorful
	float size;

	
	Particle()
	{
	}

	ParticleID getID()
	{
		return uniqueID;
	}

	Particle(ParticleID ID)
	{
		uniqueID = ID;
	}
	
	float life;

	void setMass(float mass, bool blackHole = false)
	{
		this->mass = mass;
		invMass = 1.0f / mass; //pre-calculate for better performance

		if (blackHole)
			size = 10; //super high density for blackhole (extraodinarily mass possible without further increase in size)
		else
			size = glm::sqrt(mass);
	}

	float getMass()
	{
		return mass;
	}

	float getSize()
	{
		return size;
	}

	inline void integrate(float dt)
	{
		position += velocity * dt; //Update position: s = s + ds -> ds = v * dt
		
		glm::vec3 acceleration = accumForce * invMass; // a = F * (1/m) -> a = F * m^-1

		velocity += acceleration * dt; //a = v/t --> v = a*t
		
		accumForce = glm::vec3(0); //reset applied forces
	}

private:
	float mass;
	float invMass; //the mass of the particle
};