#pragma once
#include "AnimusNode.h"
#include "AnimusAnimationNode.h"
#include <fbxsdk.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

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
	GLuint CI = -1;//animation
	bool hasNormal = false;
	bool hasTexcoord = false;
};

class AnimusMeshNode :
	public AnimusNode
{
private:
	GLint position;
	GLint normal;
	GLint texcoord;
	int vertexExistInBuffer(glm::vec4 *vPosArray, glm::vec2 *vTexArray, glm::vec3 *vNormArray, glm::vec4 vPos, glm::vec2 vTex, glm::vec3 vNorm, int size);
	int vertexExistInVector(glm::vec4 vertex, const std::vector<glm::vec4> &_V);
	int normalExistInVector(glm::vec3 normal, const std::vector<glm::vec3> &_N);
	int texcoordExistInVector(glm::vec2 texcoord, const std::vector<glm::vec2> &_T);
	void parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices);
	void parseFbxMesh(FbxNode* pNode, glm::mat4 parentSRT = glm::mat4(1));
	void parseFbxMeshGlobalSRT(FbxNode* pNode);
	void parseFbxMeshNoSRT(FbxNode* pNode);
	void parseFbxMeshAll(FbxNode* pNode, glm::mat4 parentSRT = glm::mat4(1));
	void glSetUpMesh();//
	void glSetUpBuffers();//
	void glSetUpAttribs(GLint position, GLint normal, GLint texcoord);//
	void glUpdateMesh(int frame, AnimusAnimationNode* AANode);
	void glUpdateMeshQuick(int frame, AnimusAnimationNode* AANode);
	void glUpdateBuffers();
	void glUpdateAttribs(GLint _position, GLint _normal, GLint _texcoord);
	void glUpdateAttribs();
public:
	AnimusMeshNode();
	~AnimusMeshNode();

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
	int loadObjMesh(char* fileName);
	int loadFbxMesh(char* fileName);
	int loadFbxMeshGlobalSRT(char* fileName);
	int loadFbxMeshNoSRT(char* fileName);
	int loadFbxMeshAll(char* fileName);
	void printALL(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<AnimusIndex> &AI);
	void glSetUp(GLint position, GLint normal, GLint texcoord);
	void glUpdate(int frame, AnimusAnimationNode* AANode, GLint position, GLint normal, GLint texcoord);
	void glUpdate(int frame, AnimusAnimationNode* AANode);
};

