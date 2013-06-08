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
#include "ForceManager.h"
#include "GravityForceGenerator.h"

const int PARTICLES_COUNT = 10;
#define GRAVITY 10;

//the World
class World 
{
private:
	typedef std::vector<Particle>::iterator iterator; //less writing when iterating over particles
	std::vector<Particle> _particles;
	float timePassed;
	ForceManager forceManager;
	GravityForceGenerator gravityForceGenerator;

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

	}

	void Initialize( int width, int height )
	{
		this->_width = width;
		this->_height = height;

		timePassed = 0.0f;

		srand((unsigned int)time(0));

		gravityForceGenerator.gravity.y = 10.0f;

		for (int i = 0; i < PARTICLES_COUNT; ++i)
		{
			 Particle particle;
			 particle.position = glm::vec3(getRandomNumber(0, width), 0, 0);
			 particle.acceleration = glm::vec3(0.0f, 1.0f, 0.0f) * 50.0f;
			 particle.setMass(1.0f);
			 particle.life = getRandomNumber(5, 10);
			 _particles.push_back(particle);
			 gravityForceGenerator.Register(&_particles[i]);
		}

		forceManager.RegisterForceGenerator(&gravityForceGenerator);
		
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
		forceManager.Update(deltaTime); //apply forces


		glBegin(GL_POINTS);

		#pragma omp for
		for (int i = 0; i < _particles.size(); ++i)
		{
			Particle &particle = _particles[i];
			particle.integrate(deltaTime);
			glColor3f(0.7f,0.7f,0.7f);
			glVertex3f(particle.position.x, particle.position.y, 0.0f);
		}

		glEnd();
	}

	void render(float deltaTime)
	{
		//Render the points (in parallel)
		
		
		for(int i = 0; i < PARTICLES_COUNT; ++i)
		{
					
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