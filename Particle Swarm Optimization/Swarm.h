#pragma once
#ifndef SWARM_H
#define SWARM_H

#include <glm.hpp>
#include <vector>
#include <random>
#include <ctime>
#include <GL\glfw.h>
#include <iostream>

#define RADIUS 10
#define NBR_PARTICLES 1000
#define MAX_NEIGHBORS 10
#define COA 0.3f
#define COB 0.5f
#define COC 0.5f

//The smallest part of the swarm
class Particle 
{
public:
	glm::vec3 _bestPosition;	//Best Position so far
	glm::vec3 _position;		//Current Position
	glm::vec3 _velocity;		//Current Velocity
	glm::vec3 _accForce;		//Current accumulated Force
	glm::vec3 _acceleration;	//Current Acceleration
	float _inverseMass;			//Inverse Mass of Particle
	float _damping;				//Damping of Particle

	Particle( void )
	{
		reset();
	}

	~Particle( void )
	{
		//Kill it with fire
	}

	void integrate( float deltaTime ) 
	{
		_position += _velocity * deltaTime;
		_velocity += _acceleration + (_accForce * _inverseMass) * deltaTime;
		_velocity *= pow(_damping, deltaTime);
		_accForce = glm::vec3(0);
	}

	void setMass(const float mass)
	{
		assert(mass != 0);
		_inverseMass = ((float) 1.0)/mass;
	}

	float getMass() const
	{
		if (_inverseMass == 0) {
			return FLT_MAX;
		} else {
			return ((float)1.0)/_inverseMass;
		}
	}

	void setInverseMass(const float inverseMass)
	{
		_inverseMass = inverseMass;
	}

	float getInverseMass() const
	{
		return _inverseMass;
	}

	void addForce( const glm::vec3 &force )
	{
		_accForce += force;
	}

	bool hasFiniteMass( void ) const
	{
		return _inverseMass >= 0.0f;
	}

	void reset( void )
	{
		_bestPosition	= glm::vec3(0);
		_position		= glm::vec3(0);
		_velocity		= glm::vec3(0);
		_acceleration	= glm::vec3(0);
		_accForce		= glm::vec3(0);
		_inverseMass	= 0.0;
		_damping		= 0.0;
	}
};

//the swarm
class Swarm 
{
public:
	static int mouseX;
	static int mouseY;
	int _width;
	int _height;

	std::vector<Particle> _particles;
	Swarm( int width, int height ) : _width(width), _height(height)
	{
		srand((unsigned)time(0));
		_particles.resize(NBR_PARTICLES);
		for(std::vector<Particle>::iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			(*it)._position = glm::vec3(getRandomNumber(50,_width-50),getRandomNumber(50,_height-50),0);
			(*it)._bestPosition = (*it)._position;
			(*it).setMass((float) getRandomNumber(5,20));
			(*it)._damping = 0.90f;//((float) getRandomNumber(90,95)/100.0f);
		}
	}

	int getRandomNumber(int from, int to) 
	{
		int range = (to - from);
		int rnd = from + int((range * rand()) / (RAND_MAX + 1.0));
		if(rnd == 0) return 1;
		return rnd;
	}

	void update( float deltaTime )
	{
		glm::vec3 dirCurrent(0);
		glm::vec3 mousePosition(mouseX,_height-mouseY,0);
		glm::vec3 dirBest(0);
		glm::vec3 dirBestNeighbor(0);
		glm::vec3 currentPosition(0);
		glm::vec3 currentBestPosition(0);
		glm::vec3 currentBestNeighborPosition(0);
		
		int neighborCount = 0;
		float tmpLength = 0.0f;
		float bestLength = FLT_MAX;

		glm::vec3 sum(0);

		for(int i = 0;i<NBR_PARTICLES; ++i) 
		{
			bestLength = FLT_MAX;
			currentPosition = _particles[i]._position;
			currentBestPosition = _particles[i]._bestPosition;

			//Aktuelle Flugrichtung
			dirCurrent = _particles[i]._velocity; 
			//Richtung zur meisten Nahrung
			dirBest = currentBestPosition - currentPosition;
			//Richtung zum lautesten Nachbarn
			neighborCount = 0;
			for(int j=0;j<NBR_PARTICLES - 1; ++j)
			{
				if(j!=i) 
				{
					tmpLength = glm::length(_particles[j]._position - currentPosition);
					if(tmpLength>RADIUS) 
					{
						//Nachbar gefunden
						neighborCount++;
						//Bewertung des Nachbarn
						tmpLength = glm::length(mousePosition - _particles[j]._position);
						if(tmpLength < bestLength) 
						{
							bestLength = tmpLength;
							currentBestNeighborPosition = _particles[j]._position;
						}
					}
				}
				if(neighborCount > MAX_NEIGHBORS) {
					break;
				}
			}
			dirBestNeighbor = currentBestNeighborPosition - currentPosition;

			sum = dirCurrent*0.5f + dirBest*0.9f*(float)getRandomNumber(1,4) + dirBestNeighbor*0.6f*(float)getRandomNumber(1,4);

			_particles[i].addForce(sum);
			_particles[i].integrate(deltaTime);
			//Bewertung
			if( glm::length(mousePosition-currentPosition) < glm::length(mousePosition-currentBestPosition)) 
			{
				_particles[i]._bestPosition = currentPosition;
			}
		}
	}

	void render( float deltaTime )
	{
		glBegin(GL_POINTS);
		for(std::vector<Particle>::iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			glColor3f(0.7f,0.7f,0.7f);
			glVertex3f((*it)._position.x, (*it)._position.y, 0.0f);		
		}
		glEnd();
	}

	static void updateMousePos( void )
	{
		glfwGetMousePos(&mouseX,&mouseY);
	}

	static void mouse( int key, int action )
	{
		if(key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
		{
			glfwGetMousePos(&mouseX,&mouseY);
		}
		if(key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) 
		{
		}
	}
};

#endif SWARM_H