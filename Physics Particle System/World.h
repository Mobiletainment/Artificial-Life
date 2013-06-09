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
#include "DragForceGenerator.h"
#include "BounceForceGenerator.h"
#include "FrictionForceGenerator.h"

#include <hash_map>
#include <gtx/rotate_vector.hpp> 

const int PARTICLES_COUNT = 3;
glm::vec3 GRAVITY(0.0f, -10.0f, 0.0f);
const float planeHeight = 250.0f;
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
	DragForceGenerator dragForceGenerator;
	BounceForceGenerator bounceForceGenerator;
	FrictionForceGenerator frictionForceGenerator;

	ParticleID uniqueIDIncrementor;

public:
	glm::vec3 mousePosition;
	int _width;
	int _height;
	float _k; //Steigung des Bodens

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

		_k = planeHeight / _width; //bis zum Bildschirmrand steigt der Boden um 30 Pixel

		float hypothenuse = glm::sqrt(planeHeight * planeHeight + _width * _width);

		float cosTheta = _width / hypothenuse;
		float sinTheta = planeHeight / hypothenuse;

		timePassed = 0.0f;

		srand((unsigned int)time(0));

		RegisterForceGenerator(&gravityForceGenerator);
		RegisterForceGenerator(&dragForceGenerator);
	
		gravityForceGenerator.gravity = GRAVITY;
		dragForceGenerator.k1 = 0.001;
		dragForceGenerator.k2 = 0;

		//Auf die Haftreibung (StaticFrictionForceGenerator) wurde verzichtet, da unsere Partikel nicht wieder angestoﬂen werden
		//staticFrictionForceGenerator.us = 0.7f;
		//staticFrictionForceGenerator.gravity = GRAVITY;

		frictionForceGenerator.uk = 0.5f;
		frictionForceGenerator.us = 1.0f; //faked
		frictionForceGenerator.gravity = GRAVITY;
		frictionForceGenerator.cosTheta = cosTheta;
		frictionForceGenerator.sinTheta = sinTheta;

		_particles.reserve(1000000);

		for (int i = 0; i < PARTICLES_COUNT; ++i)
		{
			 Particle *particle = new Particle(uniqueIDIncrementor++);

			 particle->position = glm::vec3(getRandomNumber(150, width-150), 31.0, 0);
			 particle->accumForce = glm::vec3(0.0f, 1.0f, 0.0f) * (float)getRandomNumber(30000,35000);
			 particle->setMass(4);
			 particle->life = getRandomNumber(5, 6);
			 particle->color = glm::vec3(getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85),getRandomNumberFloat(0.2,0.85));
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


		//Integrate and Render the points (in parallel)
		glBegin(GL_POINTS);
		//#pragma omp for

		vector<ParticleID> particlesToDelete;

		for (pit it = _particles.begin(); it != _particles.end(); ++it)
		{
			Particle *particle = it->second;
			particle->integrate(deltaTime);

			//check floor hit or floor contact (but exclude start rockets (Mass >1))
			if (particle->getMass() == 1 && particle->position.y <= getFloorPlane(particle->position.x)) //check floor contact
			{
				if (particle->velocity.y < 0) //if particle hits the floor while flying down, bounce it back
				{
					//Bump on floor, apply bounce force
					particle->position.y = getFloorPlane(particle->position.x);
					bounceForceGenerator.ApplyTo(particle);
				}
				else //normal floor contact, apply friction (Gleitreibung)
				{
					frictionForceGenerator.ApplyTo(particle);
				}
				
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
					detonateParticleAndSpawnNewOnes(particle);  //create new particles from detonating one
					DeRegisterParticle(particle); //De-Register particle
					particle->setMass(1.0f); //prevent further detonations while fading-out effect
					particle->velocity = glm::vec3(0); //necessary because of mass reduction (skyrocketing otherwise)
					particle->accumForce = glm::vec3(0);
				}

				particle->color = particle->color * 0.97f; //fade out effect

				if (particle->life <= -1.0f) //after 1s fade-out, delete the particle
				{
					particlesToDelete.push_back(particle->uniqueID);
				}
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
		
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(_width, planeHeight, 0.0);
		glEnd();
	}

	float getFloorPlane(int x)
	{
		return _k*x;
	}

	inline void detonateParticleAndSpawnNewOnes(Particle *parent)
	{
		int mass = parent->getMass() / 2;
		glm::vec3 childColor = parent->color + 0.15f;
		int life = mass > 1 ? mass * 1.5 : getRandomNumber(10, 25);
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
			registerParticle(particle);
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