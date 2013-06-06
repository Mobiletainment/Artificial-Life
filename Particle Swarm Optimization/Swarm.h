#pragma once

#include <glm.hpp>
#include <vector>
#include <random>
#include <ctime>
#include <GL\glfw.h>
#include <iostream>
#include "Particle.h"

#define RADIUS 10
#define NBR_PARTICLES 1000
#define MAX_NEIGHBORS 10

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

		for (int i = 0; i < NBR_PARTICLES; ++i)
		{
			Particle particle;
			
			particle._position = glm::vec3(getRandomNumber(0, width-1), getRandomNumber(0, height-1),0);
			particle._bestPosition = particle._position;
			float mass = (float) getRandomNumber(5, 20);
			particle.setMass(mass);

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
		glm::vec3 dirCurrent(0);
		
		glm::vec3 dirBest(0);
		glm::vec3 dirBestNeighbor(0);
		glm::vec3 currentPosition(0);
		glm::vec3 currentBestPosition(0);
		glm::vec3 currentBestNeighborPosition(0);
		
		int neighborCount = 0;
		float tmpDirection = 0.0f;
		float bestDirection = FLT_MAX;

		glm::vec3 velocity(0);

		for(int i = 0; i<NBR_PARTICLES; ++i) //determine best neighbour
		{
			bestDirection = FLT_MAX;
			currentPosition = _particles[i]._position;
			currentBestPosition = _particles[i]._bestPosition;

			//Aktuelle Flugrichtung
			dirCurrent = _particles[i]._velocity; 
			//Richtung zur meisten Nahrung
			
			dirBest = normalize(currentBestPosition - currentPosition); //normalize the velocity in the direction to the best found position

			//Richtung zum lautesten Nachbarn
			neighborCount = 0;
			for(int j=0;j<NBR_PARTICLES - 1; ++j)
			{
				if(j!=i) 
				{
					tmpDirection = glm::length(_particles[j]._position - currentPosition);
					if(tmpDirection>RADIUS) 
					{
						//Nachbar gefunden
						neighborCount++;
						//Bewertung des Nachbarn
						tmpDirection = distanceOf(_particles[j]._position);
						if(tmpDirection < bestDirection) 
						{
							bestDirection = tmpDirection;
							currentBestNeighborPosition = _particles[j]._position;
						}
					}
				}
				if(neighborCount > MAX_NEIGHBORS) {
					break;
				}
			}
			dirBestNeighbor = normalize(currentBestNeighborPosition - currentPosition); //normalize the velocity in the direction to the best neighbour

			// Der Geschwindigkeitsvektor wird dabei als Summe der drei motivierten Richtungen des Partikels berechnet:
			// vi(t+1) = a * vi(t) + b * ( xi(p) - xi(t) ) + c * ( xj(t) - xi(t) )

			// Um neue „unerforschte“ Bereiche zu explorerieren ist noch ein zusätzlicher Anteil an „Zufall“ nötig
			//  -> Die Geschwindigkeitsvektoren für die beste beobachtete Position und den besten Nachbarn werden mit Unschärfe versehen.
			// vi(t+1) = a * vi(t) + rs * b * ( xi(p) - xi(t) ) + rt * c * ( xj(t) - xi(t) )

			//Die Unschärfefaktoren rs und rt sind Zufallswerte aus den Intervallen [0,s] und [u,t].
			float rs = getRandomNumberFloat(0, S);
			float rt = getRandomNumberFloat(U, T);

			velocity = A * dirCurrent + rs * B * dirBest + rt * C * dirBestNeighbor;

			timePassed += deltaTime;

			if (timePassed >= 1.0f)
			{
				timePassed = 0.0f;
				if (i == 0)
					std::cout << "Pos: " << (int)_particles[i]._position.x << "," << (int)_particles[i]._position.y << "\t Velocity: " << (int)velocity.x << "," << (int)velocity.y << std::endl;
			}
		
			_particles[i].integrate(deltaTime, velocity);
			
			//Bewertung: aktuelle Position besser als die bekannte Beste?
			if(distanceOf(currentPosition) < distanceOf(currentBestPosition)) 
			{
				_particles[i]._bestPosition = currentPosition;
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