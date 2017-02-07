#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <fbxsdk.h>
#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "AnimusType.h"

class AnimusLoader
{
private:
	static int vertexExist(glm::vec4 vertex);
	static int normalExist(glm::vec3 normal);
	static int texcoordExist(glm::vec2 texcoord);
	static void parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices);
	static void parseFbxNode(FbxNode* pNode);
public:
	static std::vector<glm::vec4> V;
	static std::vector<glm::vec2> T;
	static std::vector<glm::vec3> N;
	static std::vector<AnimusIndex> AI;
	AnimusLoader();
	~AnimusLoader();
	static int loadObj(char* fileName);
	static int loadFbx(char* fileName);
	static void printALL();
};

