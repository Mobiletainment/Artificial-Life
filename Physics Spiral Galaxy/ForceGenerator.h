#pragma once

using namespace std;

#include "Particle.h"
#include <glm.hpp>
#include <vector>

class ForceGenerator
{
public:

	virtual void Update(float dt) = 0;
	std::vector<Particle*>* GetParticles(void);
	void Clear(void);
	typedef vector<Particle *>::iterator iterator; //less writing when iterating over particles


	void Register(Particle* particle)
	{
		_particles.push_back(particle);
	}

	void DeRegister(Particle* particle)
	{
		//expensive Lookup (HashMap etc. would be better
		for (iterator it = _particles.begin(); it != _particles.end(); ++it)
		{
			if (particle == (*it))
			{
				_particles.erase(it);
				break;
			}
		}
	}

	inline glm::vec3 normalize(glm::vec3 &vector)
	{
		if (glm::length(vector) > 0) //normalize the velocity for the specified direction
			return glm::normalize(vector);

		return vector;
	}

protected:
	vector<Particle *> _particles;
	
};