#include <GL\glfw.h>
#include <stdlib.h>
#include <iostream>
#include "swarm.h"
#include "Timer.h"

#pragma comment(lib, "GLFW.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

static const int WIDTH = 1024;
static const int HEIGHT = 768;

int main( void )
{
	int running = GL_TRUE;
	// Initialize GLFW
	if( !glfwInit() )
	{
		exit( EXIT_FAILURE );
	}
	// Open an OpenGL window
	glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );
	if( !glfwOpenWindow( WIDTH,HEIGHT,0,0,0,0,0,0, GLFW_WINDOW ) )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}


	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,  WIDTH,0, HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glfwSwapInterval(1);
	
	Swarm &swarm = Swarm::getInstance();
	swarm.Initialize(WIDTH,HEIGHT);
	//_gf.init();

	glfwSetMousePosCallback(&Swarm::updateMousePos);
	Timer t;
	t.start();
	float currentTime = t.getElapsedTimeInMilliSec();
	// Main loop
	while( running )
	{
		static float lastTime = t.getElapsedTimeInMilliSec();
		currentTime = t.getElapsedTimeInMilliSec();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		swarm.update(deltaTime);
		swarm.render(deltaTime);
		//_gf.swap();

		// Swap front and back rendering buffers
		glfwSwapBuffers();
		// Check if ESC key was pressed or window was closed
		running = !glfwGetKey( GLFW_KEY_ESC ) &&
			glfwGetWindowParam( GLFW_OPENED );
	}
	// Close window and terminate GLFW
	glfwTerminate();
	// Exit program
	exit( EXIT_SUCCESS );
}