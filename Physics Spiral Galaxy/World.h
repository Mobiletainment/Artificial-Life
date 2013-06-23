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
#include "UniversalGravitationForceGenerator.h"
#include "DampingForceGenerator.h"

#include <hash_map>
#include <gtx/rotate_vector.hpp> 

const int PARTICLES_COUNT = 1;
glm::vec3 GRAVITY(0.0f, -10.0f, 0.0f);
const float planeHeight = 250.0f;
//the World
class World 
{
private:
	typedef std::hash_map<ParticleID, Particle*>::iterator pit; //less writing when iterating over particles
	std::hash_map<ParticleID, Particle*> _particles;
	typedef pair<ParticleID, Particle*> Particle_Pair;
	glm::vec3 _center;
	float timePassed;
	std::vector<ForceGenerator*> _forceGenerators;
	typedef std::vector<ForceGenerator*>::iterator fit;
	
	UniversalGravitationForceGenerator universalGravitationForceGen;
	DampingForceGenerator dampingForceGenerator;

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

	void registerParticle(Particle *particle)
	{
		_particles.insert(Particle_Pair(particle->uniqueID, particle));

		for (ForceGenerator *forceGenerator : _forceGenerators)
		{
			forceGenerator->Register(particle);
		}
	}

	void DeRegisterParticle(Particle *particle)
	{
		for (ForceGenerator *forceGenerator : _forceGenerators)
		{
			forceGenerator->DeRegister(particle);
		}
	}

	void Initialize( int width, int height )
	{
		this->_width = width;
		this->_height = height;
		_center = glm::vec3(width * 0.5f, height * 0.5f, 0.0f);

		timePassed = 0.0f;

		srand((unsigned int)time(0));

		//RegisterForceGenerator(&gravityForceGenerator);
		RegisterForceGenerator(&universalGravitationForceGen);
		dampingForceGenerator.damping = 0.1f;
		dampingForceGenerator.center = _center;
		RegisterForceGenerator(&dampingForceGenerator);
		//gravityForceGenerator.gravity = GRAVITY;
		//dragForceGenerator.k1 = 0.001;
		//dragForceGenerator.k2 = 0;

		//Auf die Haftreibung (StaticFrictionForceGenerator) wurde verzichtet, da unsere Partikel nicht wieder angestoßen werden
		//staticFrictionForceGenerator.us = 0.7f;
		//staticFrictionForceGenerator.gravity = GRAVITY;

		
		_particles.reserve(1000);

		
		Particle *particle = new Particle(uniqueIDIncrementor++);
		particle->position = _center;
		//particle->position.y -= 75;

		particle->setMass(999999999999999, true);
		particle->color = glm::vec3(1.0f);
		registerParticle(particle);

		for (int i = 0; i < PARTICLES_COUNT; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);

			 particle->position = glm::vec3(getRandomNumber(150, width-150), getRandomNumber(150, height - 150), 0);
			 
			
			 glm::vec3 toCenter = _center - particle->position;
			 //toCenter += 
			 float speed = 0;
			 
			 toCenter = glm::normalize(toCenter);
			 /*
			 float temp = toCenter.x;
			 toCenter.x = toCenter.y;
			 toCenter.y = temp;
			 */
			 particle->velocity = toCenter;
			 //particle->accumForce = glm::vec3(getRandomNumber(-2000, -500), getRandomNumber(-20000, -1000), 0);
			 particle->setMass(getRandomNumber(1, 1));
			 particle->life = getRandomNumber(5, 6);
			 particle->color = glm::vec3(getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85));
			 //particle->integrate(0.1f);
			 registerParticle(particle);
		}

		
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
		
		_particles[0]->position = _center;

		for (fit it = _forceGenerators.begin(); it != _forceGenerators.end(); ++it)
		{
			(*it)->Update(deltaTime);
		}


		//Integrate and Render the points (in parallel)
		glBegin(GL_POINTS);
		for (pit it = _particles.begin(); it != _particles.end(); ++it)
		{
			Particle *particle = it->second;
			particle->integrate(deltaTime);
			glColor3f(particle->color.r, particle->color.g, particle->color.b);

			int radius = particle->getSize();

			for(int y=-radius; y<=radius; y++) //draw circle for particles
				for(int x=-radius; x<=radius; x++)
					if(x*x+y*y <= radius*radius)
						glVertex3f(particle->position.x+x, particle->position.y+y, 0.0f);
		}
		glEnd();
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