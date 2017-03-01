#pragma once
#include "AnimusNode.h"
#include <fbxsdk.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class AnimusMeshNode :
	public AnimusNode
{
private:
	int vertexExistInBuffer(glm::vec4 *vPosArray, glm::vec2 *vTexArray, glm::vec3 *vNormArray, glm::vec4 vPos, glm::vec2 vTex, glm::vec3 vNorm, int size);
	int vertexExistInVector(glm::vec4 vertex, const std::vector<glm::vec4> &_V);
	int normalExistInVector(glm::vec3 normal, const std::vector<glm::vec3> &_N);
	int texcoordExistInVector(glm::vec2 texcoord, const std::vector<glm::vec2> &_T);
	void parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices);
	void parseFbxNode(FbxNode* pNode, glm::mat4 parentSRT = glm::mat4(1));
	void parseFbxNodeNoSRT(FbxNode* pNode);
	void glSetUpMesh();//
	void glSetUpBuffers();//
	void glSetUpAttribs(GLint position, GLint normal, GLint texcoord);//
public:
	std::vector<glm::vec4> V;
	std::vector<glm::vec2> T;
	std::vector<glm::vec3> N;
	std::vector<AnimusIndex> AI;
	GLuint VAOs[1];
	GLuint VBOs[1];
	GLuint EBOs[1];
	glm::vec4 *vertexPositions;
	glm::vec2 *vertexTexcoords;
	glm::vec3 *vertexNormals;
	GLuint *vertexIndices;
	int vertexCount;
	int vertexIndexCount;
	AnimusMeshNode();
	~AnimusMeshNode();
	int loadObj(char* fileName);
	int loadFbx(char* fileName);
	int loadFbxNoSRT(char* fileName);
	void printALL(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<AnimusIndex> &AI);
	void glSetUp(GLint position, GLint normal, GLint texcoord);
};

