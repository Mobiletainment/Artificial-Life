#pragma once
#ifndef HEXAGONMAP_H
#define HEXAGONMAP_H

#include "hexagon.h"
#include <GL\glfw.h>
#include <vector>
#include <iostream>

enum TYPE { SLOW, MEDIUM, FAST };

//A ODD-R Horizontal Layout of Hexagons
//Columns are Q Rows are R
class HexagonMap 
{
public:
	static int mouseX;
	static int mouseY;

	static int WIDTH;
	static int HEIGHT;

	//Static Info all Hexagons share
	float _hex_size;  //Hexagon Size (Half Height)
	float _hex_width; //Hexagon Width
	float _hex_height;//Hexagon Height
	float _hex_horiz; //Horizontal Offset
	float _hex_vert;  //Vertical Offset
	int _columns;
	int _rows;

	std::vector<std::vector<Hexagon>> _hexagonmap;

	static HexagonMap& getInstance()
	{
		static HexagonMap instance; // Guaranteed to be destroyed & instantiated on first use
		return instance;
	}

	
	void initialize( int columns, int rows, float size, int width, int height) 
	{
		WIDTH = width;
		HEIGHT = height;
		_columns = columns;
		_rows = rows;
		setHexagonSize(size);
		_hexagonmap.resize(columns);
		for(int x = 0; x < columns;x++) 
		{
			_hexagonmap[x].resize(rows);
			for(int y = 0; y < rows; y++) 
			{
				//Initialize Hexagon on GRID POSITION
				TYPE t = (TYPE) getRandomNumber(0,3);
				glm::vec3 color;
				switch(t)
				{
				case SLOW:
					color = glm::vec3(0.3,0.3,0.3);
					break;
				case MEDIUM:
					color = glm::vec3(0.6,0.6,0.6);
					break;
				case FAST:
					color = glm::vec3(0.8,0.8,0.8);
					break;
				}
				_hexagonmap[x][y].initialize(x,y,_hex_size,_hex_width,_hex_height,_hex_horiz,_hex_vert);
				_hexagonmap[x][y].setColor(color);
				//_hexagonmap[x][y].setGridPosition(x,y); //already done during initialization
			}
		}
	}

	void render( float deltaTime )
	{

		glPushMatrix();
		glTranslatef(20.0f,20.0f,0.0f);
		for(int x = 0; x < _columns;x++) 
		{
			for(int y = 0; y < _rows; y++) 
			{
				_hexagonmap[x][y].render(deltaTime);
			}
		}
		glPopMatrix();

		glBegin(GL_POINTS);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(mouseX,HEIGHT-mouseY,0.0f);
		glEnd();
	}

	void setHexagonSize( float size )
	{
		_hex_size = size;
		_hex_height = _hex_size * 2;
		_hex_width = sqrt(3.0f)/2.0f * _hex_height; 
		_hex_horiz = _hex_width; 
		_hex_vert = 3/4.0f * _hex_height; 
	}

	int getRandomNumber(int from, int to) 
	{
		int range = (to - from);
		int rnd = from + int((range * rand()) / (RAND_MAX + 1.0));
		//if(rnd == 0) return 1;
		return rnd;
	}

	static void updateMousePos( void )
	{
		glfwGetMousePos(&mouseX,&mouseY);
	}

	static void mouseButtonCallback( int key, int action )
	{
		getInstance().mouseButtonCallbackImpl(key, action);
	}

	void mouseButtonCallbackImpl(int key, int action)
	{
		if(key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
		{
			float mx = mouseX-20;
			float my = (HEIGHT-mouseY)-20;
			int q = (1.0f/3.0f*sqrt(3.0f) * mx - 1/3 * my) / 10.0f;
			int r = 2/3.0f *  my / 10.0f;
			std::cout << "Tile: " << q << "," << r << "\tMousePos: " << mouseX << ", " << HEIGHT-mouseY << std::endl;

		}
		if(key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) 
		{
		}
	}

	/*
	# convert cube to odd-r offset
	q = x + (z - z&1) / 2
	r = z

	# convert odd-r offset to cube
	x = q - (r - r&1) / 2
	z = r
	y = -x-z
	*/

private:
	HexagonMap( void )
	{
		_hex_size	= 0.0f;  
		_hex_width	= 0.0f; 
		_hex_height	= 0.0f;
		_hex_horiz	= 0.0f; 
		_hex_vert	= 0.0f;  
		_columns	= 0;
		_rows		= 0;
		WIDTH = 0;
		HEIGHT = 0;
		srand ( 5 );
	}

	HexagonMap(HexagonMap const&);     // Don't Implement
	void operator=(HexagonMap const&); // Don't implement
};

#endif HEXAGONMAP_H