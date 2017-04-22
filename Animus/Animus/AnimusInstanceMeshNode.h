#pragma once
#include "AnimusNode.h"
#include "AnimusAnimationNode.h"
#include "AnimusType.h"
#include <fbxsdk.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define SPEED_FACTOR 1
#define SPEED_MULTIPLE 1
#define FRAME_BEFORE_CHANGE_MULTIPLE 5
#define FRAME_BEFORE_CHANGE_MULTIPLE_IDLE 3
#define FRAME_BEFORE_CHANGE_FACTOR 60

enum struct AnimusInstanceState{
	Walk, Idle, AnimusInstanceStateCount
};

struct AnimusInstance{
	AnimusInstanceState state = AnimusInstanceState::Idle;
	glm::vec3 position;
	int frameBeforeChange = 0;
	glm::vec3 direction;
	float speed;
};

class AnimusInstanceMeshNode :
	public AnimusNode
{
private:
	GLint position;
	GLint normal;
	GLint texcoord;
	GLint boneW;
	GLint boneI;

	GLint vMatrix;
	GLint frame;
	GLint anim;//new new new

	glm::vec3 center;//new new new
	float radius;//new new new

	int vertexExistInBuffer(glm::vec4 *vPosArray, glm::vec2 *vTexArray, glm::vec3 *vNormArray, glm::vec4 vPos, glm::vec2 vTex, glm::vec3 vNorm, int size);
	int vertexExistInVector(glm::vec4 vertex, const std::vector<glm::vec4> &_V);
	int normalExistInVector(glm::vec3 normal, const std::vector<glm::vec3> &_N);
	int texcoordExistInVector(glm::vec2 texcoord, const std::vector<glm::vec2> &_T);
	void parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices);
	void parseFbxMesh(FbxNode* pNode, glm::mat4 parentSRT = glm::mat4(1));
	void parseFbxMeshGlobalSRT(FbxNode* pNode);
	void parseFbxMeshNoSRT(FbxNode* pNode);
	void parseFbxMeshAll(FbxNode* pNode, glm::mat4 parentSRT = glm::mat4(1));

	void glSetUpMesh();
	void glSetUpBuffers(AnimusAnimationNode *lAnim);//new
	void glSetUpAttribs(GLint position, GLint normal, GLint texcoord, GLint boneW, GLint boneI, GLint vMatrix, GLint frame, GLint anim);//new new new
	void glUpdateInstances(int animLength0, int animLength1);
	void glUpdateInstanceBuffer();

	void glSetUpInstances(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &stride);//new
	void glSetUpInstances(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &strideX, const glm::vec3 &strideY);//new new new

	void calInstanceTransformMatrices(int index);//new new new
public:
	AnimusInstanceMeshNode();
	~AnimusInstanceMeshNode();

	std::vector<AnimusInstance> instanceVector;//new new
	glm::mat4 *instanceTransformMatrices;//new
	int *instanceFrame;//new
	int *instanceAnim;//new new new
	int instanceCount;//new

	std::vector<glm::vec4> V;
	std::vector<glm::vec2> T;
	std::vector<glm::vec3> N;
	std::vector<AnimusIndex> AI;
	GLuint VAOs[1];
	GLuint VBOs[3];
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

	void glSetUp(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &stride, AnimusAnimationNode *lAnim, GLint position, GLint normal, GLint texcoord, GLint boneW, GLint boneI, GLint vMatrix, GLint frame, GLint anim);//new new new
	void glSetUp(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &strideX, const glm::vec3 &strideY, AnimusAnimationNode *lAnim, GLint position, GLint normal, GLint texcoord, GLint boneW, GLint boneI, GLint vMatrix, GLint frame, GLint anim);//new new new
	void glUpdate(AnimusAnimationNode* AANode0, AnimusAnimationNode* AANode1);
};

