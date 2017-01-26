#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include <iostream>
#include <vector>

class AnimusDisplayer
{
private:
	static void init();
	static void renderScene(void);
	static GLuint program;
	static GLuint VAOs[1];
	static GLuint VBOs[1];
	static GLuint EBOs[1];
	static int vertexCount;
	static int vertexIndexCount;
	static glm::vec4 *vertexPositions;
	static glm::vec2 *vertexTexcoords;
	static glm::vec3 *vertexNormals;
	static GLuint *vertexIndices;
	static char *vertSrc;
	static char *fragSrc;
	static glm::vec4 vertexColor;
	static glm::vec4 LightDirection;
	static glm::mat4 vertexMatrix;
	static int windowPositionX;
	static int windowPositionY;
	static int windowSizeX;
	static int windowSizeY;
public:
	AnimusDisplayer();
	~AnimusDisplayer();
	static void setUp(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<GLuint> &VI, const std::vector<GLuint> &TI, const std::vector<GLuint> &NI);
	static void mainLoop(int argc, char**argv);
};

