#pragma once
#include "Dependencies\glew\glew.h"

struct AnimusIndex
{
	GLuint VI = -1;
	GLuint TI = -1;
	GLuint NI = -1;
	bool hasNormal = false;
	bool hasTexcoord = false;
};

class AnimusType
{
public:
	AnimusType();
	~AnimusType();
};

