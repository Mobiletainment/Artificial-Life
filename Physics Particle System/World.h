#pragma once

#include <omp.h>
#include <glm.hpp>
#include <vector>
#include <random>
#include <ctime>
#include <GL\glfw.h>
#include <iostream>
#include <algorithm> 
#include "Particle.h"
#include "GravityForceGenerator.h"
#include <hash_map>
#include <gtx/rotate_vector.hpp> 

const int PARTICLES_COUNT = 8;
#define GRAVITY -10;

//the World
class World 
{
private:
	typedef std::hash_map<ParticleID, Particle*>::iterator pit; //less writing when iterating over particles
	std::hash_map<ParticleID, Particle*> _particles;
	typedef pair<ParticleID, Particle*> Particle_Pair;

	float timePassed;
	std::vector<ForceGenerator*> _forceGenerators;
	typedef std::vector<ForceGenerator*>::iterator fit;
	GravityForceGenerator gravityForceGenerator;
	ParticleID uniqueIDIncrementor;

public:
	glm::vec3 mousePosition;
	int _width;
	int _height;

	static World& getInstance()
	{
		static World instance; // Guaranteed to be destroyed & instantiated on first use
		return instance;
	}
	
	World()
	{
		uniqueIDIncrementor = 0;
	}

	void RegisterForceGenerator(ForceGenerator* force)
	{
		_forceGenerators.push_back(force);
	}

	void DeRegisterForceGenerator(ForceGenerator* force)
	{
		//expensive Lookup (HashMap etc. would be better
		for (fit it = _forceGenerators.begin(); it != _forceGenerators.end(); ++it)
		{
			if (force == (*it))
			{
				_forceGenerators.erase(it);
				break;
			}
		}
	}

	void Initialize( int width, int height )
	{
		this->_width = width;
		this->_height = height;

		timePassed = 0.0f;

		srand((unsigned int)time(0));

		gravityForceGenerator.gravity.y = GRAVITY;
		_particles.reserve(1000000);

		for (int i = 0; i < PARTICLES_COUNT; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);

			 particle->position = glm::vec3(getRandomNumber(50, width-50), 0.1, 0);
			 particle->accumForce = glm::vec3(0.0f, 1.0f, 0.0f) * (float)getRandomNumber(30000,35000);
			 particle->setMass(4);
			 particle->life = getRandomNumber(5, 6);
			 particle->color = glm::vec3(getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85));
			 _particles.insert(Particle_Pair(particle->uniqueID, particle));
			 gravityForceGenerator.Register(_particles.find(particle->uniqueID)->second);
		}

		RegisterForceGenerator(&gravityForceGenerator);
		
	}

	int getRandomNumber(int min, int max) 
	{
		return min + (rand() % (max - min + 1));
	}

	float getRandomNumberFloat(float min, float max)
	{
		return  min + (float)rand()/((float)RAND_MAX/(max-min));
	}

	void updateAndRender(float deltaTime)
	{
		//calculate and apply forces
		
		for (fit it = _forceGenerators.begin(); it != _forceGenerators.end(); ++it)
		{
			(*it)->Update(deltaTime);
		}


		//Integrate and Render the points (in parallel)
		glBegin(GL_POINTS);
		//#pragma omp for

		vector<ParticleID> particlesToDelete;

		for (pit it = _particles.begin(); it != _particles.end(); ++it)
		{
			Particle *particle = it->second;
			particle->integrate(deltaTime);

			if (particle->position.y <= 0) //Bump on floor, reflect
			{
				particle->position.y = 0;
				particle->accumForce = particle->accumForce * 1.0f; //invert forces
			}

			glColor3f(particle->color.r, particle->color.g, particle->color.b);

			int radius = particle->getMass();

			for(int y=-radius; y<=radius; y++) //draw circle for particles
				for(int x=-radius; x<=radius; x++)
					if(x*x+y*y <= radius*radius)
						glVertex3f(particle->position.x+x, particle->position.y+y+radius, 0.0f);
         
			//cout << "Position: " << (int)particle.position.x << "," << (int)particle.position.y << "\tVelocity: " << (int)particle.velocity.y << endl;

			particle->life -= deltaTime;

			if (particle->life <= 0.0f) //particle's life span is over
			{
				if (particle->getMass() > 1) //detonate it and divide mass
				{
					gravityForceGenerator.DeRegister(particle); //De-Register particle
					detonateParticleAndSpawnNewOnes(particle);  //create new particles from detonating one
				}

				particlesToDelete.push_back(particle->uniqueID);
			}
		}
		glEnd();

		//Remove dead particles
		for (ParticleID key : particlesToDelete)
		{
			cout << "Del: " << key << endl;
			_particles.erase(key);
		}

		//cout << "Particles: " << uniqueIDIncrementor << endl;
		
	}

	inline void detonateParticleAndSpawnNewOnes(Particle *parent)
	{
		int mass = parent->getMass() / 2;
		glm::vec3 childColor = parent->color + 0.15f;
		int life = mass > 1 ? mass * 1.5 : getRandomNumber(5, 20);
		int newParticleCount = 20 * mass;

		glm::vec3 forceDirection(1.0f, 0.0f, 0.0f);
		glm::vec3 normalDirection(0.0, 0.0f, 1.0f);
		float currentRotation = 0.0f;
		float rotationStep = 360.0f / newParticleCount;
		float force = 3000.0f * mass;

		for (int i = 0; i < newParticleCount; ++i)
		{
			Particle *particle = new Particle(uniqueIDIncrementor++);
			particle->position = parent->position;
			
			particle->accumForce = glm::rotate(forceDirection, currentRotation, normalDirection) * force;
			currentRotation += rotationStep;

			particle->setMass(mass);
			particle->life = life;
			particle->color = childColor;
			_particles.insert(Particle_Pair(particle->uniqueID, particle));
			gravityForceGenerator.Register(_particles.find(particle->uniqueID)->second);
		}
	}

	inline glm::vec3 normalize(glm::vec3 &vector)
	{
		if (glm::length(vector) > 0) //normalize the velocity for the specified direction
			return glm::normalize(vector);

		return vector;
	}

	inline float distanceOf(glm::vec3 &position)
	{
		return glm::length(mousePosition - position);
	}

	static void updateMousePos(int posX, int posY)
	{
		getInstance().updateMousePosImpl(posX, posY);
	}

	void updateMousePosImpl(int mouseX, int mouseY)
	{
		glfwGetMousePos(&mouseX,&mouseY);
		
		mousePosition.x = mouseX;
		mousePosition.y = _height-mouseY;
	}
};