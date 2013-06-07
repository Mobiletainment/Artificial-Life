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

const int PARTICLES_COUNT = 10000;
const int NEIGHBOUR_CONT = 100; // used as number of neighbours within social neighbourhood (array index)

const float A = 1.0f; //Koeffizient A beeinflusst die Motivation für die aktuelle Geschwindigkeit
const float B = 1.0f; //Koeffizient B beeinflusst die Motivation für Richtung zur besten beobachteten Position
const float C = 1.0f; //Koeffizient C beeinflusst die Motivation für Richtung zum besten Nachbar

//Die Unschärfefaktoren rs und rt sind Zufallswerte aus den Intervallen [0,s] und [u,t].
const float S = 1;	 //für rs (beeinflust C)
const float U = 0.5; //untere Schranke von rt (beeinflusst C)
const float T = 1.5; //obere Schranke von rt

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
			
			//initialize particle
			particle._position = glm::vec3(getRandomNumber(0, width-1), getRandomNumber(0, height-1),0);
			particle._bestPosition = particle._position;
			particle._distance = 0.0f;
			particle._bestNeighborPosition = glm::vec3(0);
			particle._velocity = glm::vec3(0.0f, 0.0f, 0.0f);

			_particles.push_back(particle);
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

	void update(float deltaTime)
	{
		//1. determine best neighbour

		//accelerate finding the best neighbour by precalculating the distance to the target		
		
		#pragma omp parallel
		{
			#pragma omp for
			for (int i = 0; i < PARTICLES_COUNT; ++i) //calculating distance to target
			{
				_particles[i]._distance = distanceOf(_particles[i]._position);
			}		

			#pragma omp for
			for(int i = 0; i<PARTICLES_COUNT; ++i) //für jedes Partikel den besten Nachbar im sozialen Umkreis updaten
			{	
				int compareWithSocialNeighbours = NEIGHBOUR_CONT;
				//Code is almost duplicated, but nicer in terms of performance

				for (int j = i-1; j >= 0; --j) //all neighbours left from current particle (from nearest to farest index)
				{
					//Bewertung der eigenen besten Position mit der Position des Nachbars
					if(_particles[j]._distance < _particles[i]._distance) 
					{
						_particles[i]._bestNeighborPosition = _particles[j]._position; //neue beste Position
					}

					if(--compareWithSocialNeighbours == 0)
						break; //avoid too much comparisons and compare to a maximum of NEIGHBOUR_COUNT particles for best position


				}

				//exclude the current particle for in order to not compare it with itself
				for (int j = i+1; j < PARTICLES_COUNT; ++j) //all neighbours left from current particle (from nearest to farest index)
				{
					if(compareWithSocialNeighbours-- == 0)
						break; //avoid too much comparisons and compare to a maximum of NEIGHBOUR_COUNT particles for best position

					//Bewertung der eigenen besten Position mit der Position des Nachbars
					if(_particles[j]._distance < _particles[i]._distance) 
					{
						_particles[i]._bestNeighborPosition = _particles[j]._position; //neue beste Position
					}
				}
			}

			//2. update current position
			#pragma omp for
			for (int i = 0; i < PARTICLES_COUNT; ++i)
			{
				Particle &particle = _particles[i];
				// Der Geschwindigkeitsvektor wird dabei als Summe der drei motivierten Richtungen des Partikels berechnet:
				// vi(t+1) = a * vi(t) + b * ( xi(p) - xi(t) ) + c * ( xj(t) - xi(t) )

				// Um neue „unerforschte“ Bereiche zu explorerieren ist noch ein zusätzlicher Anteil an „Zufall“ nötig
				//  -> Die Geschwindigkeitsvektoren für die beste beobachtete Position und den besten Nachbarn werden mit Unschärfe versehen.
				// vi(t+1) = a * vi(t) + rs * b * ( xi(p) - xi(t) ) + rt * c * ( xj(t) - xi(t) )

				//Die Unschärfefaktoren rs und rt sind Zufallswerte aus den Intervallen [0,s] und [u,t].
				float rs = getRandomNumberFloat(0, S);
				float rt = getRandomNumberFloat(U, T);

				//Richtung zur besten beobachteten Position (die Eigene)
				glm::vec3 dirToBestPos = normalize(particle._bestPosition - particle._position); //normalize the velocity in the direction to the best found position

				glm::vec3 dirToBestNeighbor = normalize(particle._bestNeighborPosition - particle._position); //normalize the velocity in the direction to the best neighbour
			
				//calculate the final direction in which to move
				glm::vec3 velocity = A * particle._velocity + rs * B * dirToBestPos + rt * C * dirToBestNeighbor;
			
				// Update current Position: s = v*t, ds = v*dt
				particle._velocity = velocity; // set new velocity
				particle._position = particle._position + particle._velocity * deltaTime; 

				//Bewertung der neuen Position: ist neue Position besser als die bekannte Beste?
				if(distanceOf(particle._position) < particle._distance) 
				{
					particle._bestPosition = particle._position;
				}
				/*
				timePassed += deltaTime;

				if (timePassed >= 1.0f)
				{
					timePassed = 0.0f;
					int elementToPrint = PARTICLES_COUNT * 0.5f;
					std::cout << "Pos: " << (int)_particles[elementToPrint]._position.x << "," << (int)_particles[elementToPrint]._position.y << "\t Velocity: " << (int)_particles[elementToPrint]._velocity.x << "," << (int)_particles[elementToPrint]._velocity.y << std::endl;
				}
				*/
			}

			//Render the points (in parallel)
			glBegin(GL_POINTS);
			#pragma omp for
			for(int i = 0; i < PARTICLES_COUNT; ++i)
			{
				glColor3f(0.7f,0.7f,0.7f);
				glVertex3f(_particles[i]._position.x, _particles[i]._position.y, 0.0f);		
			}
			glEnd();
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
		std::cout << "MousePos: " << mouseX << "," << mouseX << std::endl; 
		mousePosition.x = mouseX;
		mousePosition.y = _height-mouseY;
	}
};