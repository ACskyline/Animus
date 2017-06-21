#pragma once
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glew\glew.h"

struct AnimusControlPointPostion
{
	bool ready = false;
	glm::vec4 position;
	glm::mat4x4 positionMatrix;
};

struct AnimusIndex
{
	GLuint VI = -1;
	GLuint TI = -1;
	GLuint NI = -1;
	GLuint CI = -1;
	bool hasNormal = false;
	bool hasTexcoord = false;
};