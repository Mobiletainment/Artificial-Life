// Note that I use a 3-vector for the coordinates. Cube hex
// coordinates share a lot with vectors in 3-space so if you already
// have a 3-vector class you can reuse it for hexes. Trapezoidal hex
// coordinates (q, r) share a lot with vectors in 2-space and screen
// coordinates are of course vectors in 2-space, so all of these could
// use the same underlying class to share code. However I'm keeping
// some of these separate (and duplicating code) to make the
// explanations in the article easier to follow.

#pragma once

using namespace std;

#include <glm.hpp>
#include <vector>
class Cube
{
public:
	glm::vec3 v;

	Cube(glm::vec3 coordinate)
	{
		v = coordinate;
	}

	bool equals(Cube other)
	{
		return v.x == other.v.x && v.y == other.v.y && v.z == other.v.z;
	}

	Cube scale(float f)
	{
		return Cube(v * f); //allocation on heap necessary?
	}

};