#include <glm.hpp>
#include <vector>

//The smallest part of the swarm
struct Particle 
{
	glm::vec3 _position;	 //the article's current position
	float _distance;			 //the distance to the target
	glm::vec3 _bestPosition; //the best position the particle has been so far
	glm::vec3 _bestNeighborPosition; //the best position of a neighbor within range
	glm::vec3 _velocity;		//Current Velocity	
};