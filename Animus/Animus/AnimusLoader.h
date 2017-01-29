#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "AnimusType.h"

class AnimusLoader
{
private:
	static void parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices);
public:
	static std::vector<glm::vec4> V;
	static std::vector<glm::vec2> T;
	static std::vector<glm::vec3> N;
	static std::vector<AnimusIndex> AI;
	AnimusLoader();
	~AnimusLoader();
	static 	int loadObj(char* fileName);
	static void printALL(std::vector<glm::vec4> &V, std::vector<glm::vec2> &T, std::vector<glm::vec3> &N, std::vector<AnimusIndex> AI);
};

