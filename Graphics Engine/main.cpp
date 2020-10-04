#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Program.h"
#include "Buffer.h"
#include <string>
#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <soil/SOIL.h>


glm::vec3 cameraPos = { 4.0f, 6.0f, 25.0f };
float delta = 0.05f;
float speed = 5;

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

	

	switch (key)
	{
	case GLFW_KEY_A:
		cameraPos.x -= delta * speed;
		break;
	case GLFW_KEY_D:
		cameraPos.x += delta * speed;
		break;
	case GLFW_KEY_S:
		cameraPos.z += delta * speed;
		break;
	case GLFW_KEY_W:
		cameraPos.z -= delta * speed;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		cameraPos.y -= delta * speed;
		break;
	case GLFW_KEY_SPACE:
		cameraPos.y += delta * speed;
		break;
	default:
		break;
	}
}



void main() {

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(600, 600, "Voxel Engine", 0, 0);
	glfwMakeContextCurrent(window);
	glewInit();
	
	glPointSize(4);


	auto p = CreateProgram(vs, gs, fs);
	int posLocation = glGetAttribLocation(p, "pos");

	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posLocation);
	glUseProgram(p);
	int cameraLocation = glGetUniformLocation(p, "camera");
	

	glfwSetKeyCallback(window, keyCallback);


	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	/* load an image file directly as a new OpenGL texture */
	GLuint tex_2d = SOIL_load_OGL_texture
	(
		"wood.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		auto camera = glm::perspective(45.0f, 1.0f, 0.01f, 40.0f) * glm::lookAt(cameraPos, cameraPos + glm::vec3(0, 0.2f, -1), { 0.0f,1.0f,0.0f });

		glUniformMatrix4fv(cameraLocation, 1, GL_FALSE, &camera[0][0]);


		
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);

}