/*
Author: Cong Vu
Class : ECE4122 - A
Last Date Modified : 12 / 06 / 2022

Description: control cpp file

*/

#include "controls.hpp"

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// access the variable "window" declared in tutorialXXX.cpp.
extern GLFWwindow* window; 


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// Depth init + Z
glm::vec3 position = glm::vec3( 0, 0, 5); 
glm::vec3 origin = glm::vec3( 0, 0, 0 ); 

// Initial horizontal, Vertical, and Field of View
float horizontalAngle = 3.14f;	// toward -Z
float verticalAngle = 30.0f;
float initialFoV = 45.0f;	

// set speed and mouse speed
float speed = 3.0f; // 3.0 second
float mouseSpeed = 0.005f;

float radius = 1500.0f;

void computeMatricesFromInputs()
{

	// glfwGetTime called for first time once
	static double lastTime = glfwGetTime();

	// caculate deltaTime
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	double xpos, ypos, zpos;
	// up vector
	glm::vec3 up(0.0f, 1.0f, 0.0);
	//glm::cross( right, direction );

	// forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS)
	{
	    radius -= 10.0f;
	    //position +  direction * deltaTime * speed;
	}
	
	// backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS)
	{
	  	radius += 10.0f; 
	  	//position -= direction * deltaTime * speed;
	}
	
	// right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
	{
	  	horizontalAngle -= 1.0f;
		//position += right * deltaTime * speed;
	}
	
	// left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS)
	{
	  horizontalAngle += 1.0f;
	  //position -= right * deltaTime * speed;
	}

	xpos = radius * cos(glm::radians(verticalAngle)) * sin(glm::radians(horizontalAngle));
        ypos = radius * sin(glm::radians(verticalAngle));	
        zpos = radius * cos(glm::radians(verticalAngle)) * cos(glm::radians(horizontalAngle));
							       position = glm::vec3(xpos, ypos, zpos);
	float FoV = initialFoV;
	

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 10000.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
							    origin, 			// and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	
	// update lastime for next frame
	lastTime = currentTime;
}
