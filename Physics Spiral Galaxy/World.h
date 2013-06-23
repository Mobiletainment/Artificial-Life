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
#include "SpiralForceGenerator.h"

#include <hash_map>
#include <gtx/rotate_vector.hpp> 

const int PARTICLES_COUNT = 120;
glm::vec3 GRAVITY(0.0f, -10.0f, 0.0f);
const float planeHeight = 250.0f;
//the World
class World 
{
private:
	typedef std::vector<Particle*>::iterator pit; //less writing when iterating over particles
	std::vector<Particle*> _particles;
	glm::vec3 _center;
	float timePassed;
	std::vector<ForceGenerator*> _forceGenerators;
	typedef std::vector<ForceGenerator*>::iterator fit;
	
	UniversalGravitationForceGenerator universalGravitationForceGen;
	DampingForceGenerator dampingForceGenerator;
	SpiralForceGenerator spiralForceGenerator;

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
		_particles.push_back(particle);

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
		
		spiralForceGenerator.center = _center;
		dampingForceGenerator.center = _center;
		dampingForceGenerator.damping = 0.1f;

		RegisterForceGenerator(&universalGravitationForceGen);
	//	RegisterForceGenerator(&spiralForceGenerator);
		
		//RegisterForceGenerator(&dampingForceGenerator);


		_particles.reserve(100);
		float speed = 30.0f;
		
		Particle *particle = new Particle(uniqueIDIncrementor++);
		particle->position = _center;
		//particle->position.y -= 75;

		particle->setMass(999999999999999, true);
		particle->color = glm::vec3(1.0f);
		registerParticle(particle);

		for (int i = 0; i < PARTICLES_COUNT / 4; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);
			 particle->position = glm::vec3(getRandomNumber(_center.x - 140, _center.x - 100), getRandomNumber(_center.y - 140, _center.y - 150), 0);

			 glm::vec3 toCenter = _center - particle->position;
			 toCenter.x = toCenter.x - 100;
			 toCenter.y = toCenter.y + 200;
			 
			 toCenter = glm::normalize(toCenter);
			
			 particle->velocity = toCenter * speed;
			 //particle->accumForce = glm::vec3(getRandomNumber(-2000, -500), getRandomNumber(-20000, -1000), 0);
			 particle->setMass(getRandomNumber(1, 1));
			 particle->life = getRandomNumber(5, 6);
			 particle->color = glm::vec3(getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85));
			 //particle->integrate(0.1f);
			 registerParticle(particle);
		}

		for (int i = 0; i < PARTICLES_COUNT / 4; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);
			 particle->position = glm::vec3(getRandomNumber(_center.x + 140, _center.x + 100), getRandomNumber(_center.y + 140, _center.y + 150), 0);

			 glm::vec3 toCenter = _center - particle->position;
			 toCenter.x = toCenter.x + 100;
			 toCenter.y = toCenter.y - 200;
			 
			 toCenter = glm::normalize(toCenter);
			
			 particle->velocity = toCenter * speed;
			 //particle->accumForce = glm::vec3(getRandomNumber(-2000, -500), getRandomNumber(-20000, -1000), 0);
			 particle->setMass(getRandomNumber(1, 1));
			 particle->life = getRandomNumber(5, 6);
			 particle->color = glm::vec3(getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85));
			 //particle->integrate(0.1f);
			 registerParticle(particle);
		}

		for (int i = 0; i < PARTICLES_COUNT / 4; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);
			 particle->position = glm::vec3(getRandomNumber(_center.x + 140, _center.x + 100), getRandomNumber(_center.y - 140, _center.y - 150), 0);

			 glm::vec3 toCenter = _center - particle->position;
			 toCenter.x = toCenter.x - 200;
			 toCenter.y = toCenter.y - 140;
			 
			 toCenter = glm::normalize(toCenter);
			
			 particle->velocity = toCenter * speed;
			 //particle->accumForce = glm::vec3(getRandomNumber(-2000, -500), getRandomNumber(-20000, -1000), 0);
			 particle->setMass(getRandomNumber(1, 1));
			 particle->life = getRandomNumber(5, 6);
			 particle->color = glm::vec3(getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85));
			 //particle->integrate(0.1f);
			 registerParticle(particle);
		}
		
		for (int i = 0; i < PARTICLES_COUNT / 4; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);
			 particle->position = glm::vec3(getRandomNumber(_center.x - 140, _center.x - 100), getRandomNumber(_center.y + 140, _center.y + 150), 0);

			 glm::vec3 toCenter = _center - particle->position;
			 toCenter.x = toCenter.x + 200;
			 toCenter.y = toCenter.y + 140;
			 
			 toCenter = glm::normalize(toCenter);
			
			 particle->velocity = toCenter * speed;
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
		
		
		for (fit it = _forceGenerators.begin(); it != _forceGenerators.end(); ++it)
		{
			(*it)->Update(deltaTime);
		}
		_particles[0]->accumForce = glm::vec3(0);


		//Integrate and Render the points (in parallel)
		glBegin(GL_POINTS);

		//#pragma omp parallel
		{
			//#pragma omp for
			for (int i = 0; i < _particles.size(); ++i)
			{
				Particle *particle = _particles[i];
				particle->integrate(deltaTime);
				glColor3f(particle->color.r, particle->color.g, particle->color.b);

				int radius = particle->getSize();

				for(int y=-radius; y<=radius; y++) //draw circle for particles
					for(int x=-radius; x<=radius; x++)
						if(x*x+y*y <= radius*radius)
							glVertex3f(particle->position.x+x, particle->position.y+y, 0.0f);

				particle->accumForce = glm::vec3(0);
			}
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