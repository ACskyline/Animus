#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"

class AnimusLoader
{
private:
	static void parseFace(std::stringstream &ss, GLuint vIndices[3], GLuint tIndices[3], GLuint nIndices[3]);
public:
	static std::vector<glm::vec4> V;
	static std::vector<glm::vec2> T;
	static std::vector<glm::vec3> N;
	static std::vector<GLuint> VI;
	static std::vector<GLuint> TI;
	static std::vector<GLuint> NI;
	AnimusLoader();
	~AnimusLoader();
	static 	int loadObj(char* fileName);
	static void printALL(std::vector<glm::vec4> &V, std::vector<glm::vec2> &T, std::vector<glm::vec3> &N, std::vector<GLuint> VI, std::vector<GLuint> TI, std::vector<GLuint> NI);
};

