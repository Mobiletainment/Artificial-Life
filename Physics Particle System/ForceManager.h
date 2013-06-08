#pragma once

using namespace std;

#include <vector>
#include "ForceGenerator.h"

class ForceManager 
{
private:
	std::vector<ForceGenerator*> _forceGenerators;
	std::vector<Particle*> _unifiedParticles;
	typedef std::vector<ForceGenerator*>::iterator iterator;

public:

	void Update(float dt)
	{
		//calculate forces
		for (iterator it = _forceGenerators.begin(); it != _forceGenerators.end(); ++it)
		{
			(*it)->Update(dt);
		}
	}

	void RegisterForceGenerator(ForceGenerator* force)
	{
		_forceGenerators.push_back(force);
	}

	void DeRegisterForceGenerator(ForceGenerator* force)
	{
		//expensive Lookup (HashMap etc. would be better
		for (iterator it = _forceGenerators.begin(); it != _forceGenerators.end(); ++it)
		{
			if (force == (*it))
			{
				_forceGenerators.erase(it);
				break;
			}
		}
	}
};