#pragma once

#include <glm.hpp>
#include <vector>
#include <random>
#include <ctime>
#include <GL\glfw.h>
#include <iostream>
#include <algorithm> 
#include "Particle.h"

#define RADIUS 100
#define PARTICLES_COUNT 200

const float A = 1.0f; //Koeffizient A beeinflusst die Motivation für die aktuelle Geschwindigkeit
const float B = 1.0f; //Koeffizient B beeinflusst die Motivation für Richtung zur besten beobachteten Position
const float C = 1.0f; //Koeffizient C beeinflusst die Motivation für Richtung zum besten Nachbar

//Die Unschärfefaktoren rs und rt sind Zufallswerte aus den Intervallen [0,s] und [u,t].
const float S = 1;	 //für rs (beeinflust C)
const float U = 1.5; //untere Schranke von rt (beeinflusst C)
const float T = 4.5; //obere Schranke von rt

//the swarm
class Swarm 
{
private:
	typedef std::vector<Particle>::iterator iterator; //less writing when iterating over particles
	std::vector<Particle> _particles;
	float timePassed;

public:
	glm::vec3 mousePosition;
	int _width;
	int _height;

	static Swarm& getInstance()
	{
		static Swarm instance; // Guaranteed to be destroyed & instantiated on first use
		return instance;
	}
	
	Swarm()
	{

	}

	void Initialize( int width, int height )
	{
		this->_width = width;
		this->_height = height;

		timePassed = 0.0f;

		srand((unsigned int)time(0));

		for (int i = 0; i < PARTICLES_COUNT; ++i)
		{
			Particle particle;
			
			particle._position = glm::vec3(getRandomNumber(0, width-1), getRandomNumber(0, height-1),0);
			particle._bestPosition = particle._position;

			_particles.push_back(particle);
		}
	}

	int getRandomNumber(int min, int max) 
	{
		return min + (rand() % (max - min + 1));
	}

	float getRandomNumberFloat(float min, float max)
	{
		return min + (float)rand()/((float)RAND_MAX/(max-min));
	}

	void update(float deltaTime)
	{
		//1. determine best neighbour
		//accelerate finding the best neighbour within radius by presorting based on their distance to the target
		
		for (iterator it = _particles.begin(); it != _particles.end(); ++it) //calculating distance to target
		{
			it->_distance = distanceOf(it->_position);
		}

		//sorting by distance to target
		std::sort(_particles.begin(), _particles.end(), [](Particle one, Particle two) //avoid binding instance compare method and just use a lambda expression
			{
				return one._distance > two._distance;
			}
		);
		

		for(int i = 0; i<PARTICLES_COUNT; ++i)
		{		
			//Richtung zum best-positionierten Nachbar
			for(int j=0; j<PARTICLES_COUNT - 1; ++j) //find the neighbour with the best position
			{
				if(j!=i) //exclude the current particle for the comparison ;) 
				{
					//determine if the particle is within the range where we can hear it
					float distance = glm::length(_particles[j]._position - _particles[i]._position);
					if(distance<RADIUS) //update the direction to the best neighbour
					{
						//Bewertung der besten Position mit Nachbar
						float distanceFromNeighbour = _particles[j]._distance;
						if(distanceFromNeighbour < _particles[i]._distance) 
						{
							_particles[i]._bestNeighborPosition = _particles[j]._position;
						}
					}
				}
			}
		}

		//2. update current position
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			// Der Geschwindigkeitsvektor wird dabei als Summe der drei motivierten Richtungen des Partikels berechnet:
			// vi(t+1) = a * vi(t) + b * ( xi(p) - xi(t) ) + c * ( xj(t) - xi(t) )

			// Um neue „unerforschte“ Bereiche zu explorerieren ist noch ein zusätzlicher Anteil an „Zufall“ nötig
			//  -> Die Geschwindigkeitsvektoren für die beste beobachtete Position und den besten Nachbarn werden mit Unschärfe versehen.
			// vi(t+1) = a * vi(t) + rs * b * ( xi(p) - xi(t) ) + rt * c * ( xj(t) - xi(t) )

			//Die Unschärfefaktoren rs und rt sind Zufallswerte aus den Intervallen [0,s] und [u,t].
			float rs = getRandomNumberFloat(0, S);
			float rt = getRandomNumberFloat(U, T);

			//Richtung zur besten beobachteten Position (die Eigene)
			glm::vec3 dirToBestPos = normalize(it->_bestPosition - it->_position); //normalize the velocity in the direction to the best found position

			glm::vec3 dirToBestNeighbor = normalize(it->_bestNeighborPosition - it->_position); //normalize the velocity in the direction to the best neighbour
			
			//calculate the final direction in which to move
			glm::vec3 velocity = A * it->_velocity + rs * B * dirToBestPos + rt * C * dirToBestNeighbor;
			
			// Update current Position: s = v*t, ds = v*dt
			it->_velocity = velocity; // set new velocity
			it->_position = it->_position + it->_velocity * deltaTime; 

			//Bewertung der neuen Position: ist neue Position besser als die bekannte Beste?
			if(distanceOf(it->_position) < it->_distance) 
			{
				it->_bestPosition = it->_position;
			}

			timePassed += deltaTime;

			if (timePassed >= 1.0f)
			{
				timePassed = 0.0f;
				int elementToPrint = PARTICLES_COUNT * 0.5f;
				std::cout << "Pos: " << (int)_particles[elementToPrint]._position.x << "," << (int)_particles[elementToPrint]._position.y << "\t Velocity: " << (int)_particles[elementToPrint]._velocity.x << "," << (int)_particles[elementToPrint]._velocity.y << std::endl;
			}
			
		}
	}

	glm::vec3 normalize(glm::vec3 vector)
	{
		if (glm::length(vector) > 0) //normalize the velocity for the specified direction
			return glm::normalize(vector);

		return vector;
	}

	float distanceOf(glm::vec3 position)
	{
		return glm::length(mousePosition - position);
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

	static void updateMousePos(int posX, int posY)
	{
		getInstance().updateMousePosImpl(posX, posY);
	}

	void updateMousePosImpl(int mouseX, int mouseY)
	{
		glfwGetMousePos(&mouseX,&mouseY);
		std::cout << "MousePos: " << mouseX << "," << mouseX << std::endl; 
		mousePosition.x = mouseX;
		mousePosition.y = _height-mouseY;
	}
};