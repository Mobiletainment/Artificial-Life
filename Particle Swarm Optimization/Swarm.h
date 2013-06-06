#pragma once
#ifndef SWARM_H
#define SWARM_H

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

const float A = 0.3f;
const float B = 0.3f;
const float C = 0.3f;

const float S = 4;
const float U = 0.5;
const float T = 1.5;

//the swarm
class Swarm 
{
private:
	std::vector<Particle> _particles;

public:
	static int mouseX;
	static int mouseY;
	int _width;
	int _height;

	
	Swarm( int width, int height ) : _width(width), _height(height)
	{
		srand((unsigned int)time(0));

		for (int i = 0; i < NBR_PARTICLES; ++i)
		{
			Particle particle;
			
			particle._position = glm::vec3(getRandomNumber(50,_width-50),getRandomNumber(50,_height-50),0);
			particle._bestPosition = particle._position;
			float mass = (float) getRandomNumber(5, 20);
			particle.setMass(mass);
			particle._damping = 0.90f;

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

			// Der Geschwindigkeitsvektor wird dabei als Summe der drei motivierten Richtungen des Partikels berechnet:
			// vi(t+1) = a * vi(t) + b * ( xi(p) - xi(t) ) + c * ( xj(t) - xi(t) )

			// Um neue „unerforschte“ Bereiche zu explorerieren ist noch ein zusätzlicher Anteil an „Zufall“ nötig
			//  -> Die Geschwindigkeitsvektoren für die beste beobachtete Position und den besten Nachbarn werden mit Unschärfe versehen.
			// vi(t+1) = a * vi(t) + rs * b * ( xi(p) - xi(t) ) + rt * c * ( xj(t) - xi(t) )

			//Die Unschärfefaktoren rs und rt sind Zufallswerte aus den Intervallen [0,s] und [u,t].
			float rs = getRandomNumberFloat(0, S);
			float rt = getRandomNumberFloat(U, T);

			sum = A * dirCurrent + rs * B * dirBest + rt * C * dirBestNeighbor;

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