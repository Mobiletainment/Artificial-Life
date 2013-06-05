#pragma once
#ifndef HEXAGON_H
#define HEXAGON_H

#include "config.h"
#include <glm.hpp>
#include <GL\glfw.h>


class Hexagon
{
public:
	float _x;
	float _y;
	float _size;
	float _angle;
	float _x_i;
	float _y_i;
	int _coord_x;
	int _coord_y;

	glm::vec3 _color;

	Hexagon( void ) 
	{
		_x = 0;
		_y = 0;
		_x_i = 0;
		_y_i = 0;
		_size = 0;
	}

	void initialize( int x, int y, float size, float _hex_width, float _hex_height, float _hex_horiz, float _hex_vert )
	{
		setGridPosition(x,y);
		_size = size;
		if(y % 2 == 0) {
			//No Offset in x;
			_x = _coord_x + (_hex_horiz * x);
		} else {
			//Offset in x = _hex_vert / 2.0f;
			_x = _coord_x + (_hex_horiz / 2.0f) + (_hex_horiz * x);
		}
		_y = _coord_y + (_hex_vert * y);
	}

	void setGridPosition( int x, int y )
	{
		_coord_x = x;
		_coord_y = y;
		
	}

	void setPosition( float x, float y ) 
	{
		_x = x;
		_y = y;
	}


	void setColor( glm::vec3 color )
	{
		_color = color;
	}

	void render( float deltaTime ) 
	{
		glBegin(GL_POLYGON);
		glColor3f(_color.x,_color.y,_color.z);
		for (int i = 0; i <= 6; i++) {
			_angle = 2 * PI / 6 * (i + 0.5);
			_x_i = _x + _size * cos(_angle);
			_y_i = _y + _size * sin(_angle);
			glVertex3f(_x_i, _y_i, 0.0f);		
		}
		glEnd();
	}
};

#endif HEXAGON_H