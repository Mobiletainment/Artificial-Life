#include <glm.hpp>
#include <vector>

//The smallest part of the swarm
class Particle 
{
public:
	glm::vec3 _bestPosition;	//Best Position so far
	glm::vec3 _position;		//Current Position
	glm::vec3 _velocity;		//Current Velocity
	glm::vec3 _accForce;		//Current accumulated Force
	float _inverseMass;			//Inverse Mass of Particle
	
	Particle()
	{
		_bestPosition	= glm::vec3(0);
		_position		= glm::vec3(0);
		_velocity		= glm::vec3(1.0f, 0.0f, 0.0f);
		_accForce		= glm::vec3(0);
		_inverseMass	= 0.0;
	}

	~Particle( void )
	{
		//nothing to clean up
	}

	void integrate(float deltaTime) 
	{
		// s = v*t
		_position += _velocity * deltaTime; //ds = v*dt

		// a = F/m    -->    a = F*(1/m)
		// v = a * t
		_velocity += (_accForce * _inverseMass) * deltaTime;
		
		// damping^dt
		//_velocity *= pow(_damping, deltaTime);
		_accForce = glm::vec3(0);
	}

	void setMass(const float mass)
	{
		assert(mass != 0);
		_inverseMass = 1.0f/mass;
	}

	float getMass() const
	{
		if (_inverseMass == 0) {
			return FLT_MAX;
		} else {
			return 1.0f/_inverseMass;
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
};