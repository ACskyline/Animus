//This class is deprecated now
#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <fbxsdk.h>
#include "Dependencies\glew\glew.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtx\rotate_vector.hpp"
#include "AnimusType.h"
#include "AnimusMeshNode.h"

class AnimusLoader
{
private:
	static int vertexExist(glm::vec4 vertex, const std::vector<glm::vec4> &_V);
	static int normalExist(glm::vec3 normal, const std::vector<glm::vec3> &_N);
	static int texcoordExist(glm::vec2 texcoord, const std::vector<glm::vec2> &_T);
	static void parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices);
	static void parseFbxNode(FbxNode* pNode, AnimusMeshNode * lMesh, glm::mat4 parentSRT);
public:
	AnimusLoader();
	~AnimusLoader();
	static int loadObj(char* fileName, AnimusMeshNode * lMesh);
	static int loadFbx(char* fileName, AnimusMeshNode * lMesh);
	static void printALL(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<AnimusIndex> &AI);
};

