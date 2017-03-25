#pragma once
#include "AnimusNode.h"
#include <fbxsdk.h>
#include <string>
#include <vector>
#include <fstream>

struct AnimusWeight
{
	int boneIndex = -1;
	float weight = -1;
};

struct AnimusControlPointWeights
{
	std::vector<AnimusWeight> weights;
};

struct AnimusFrame
{
	int frameNum = -1;
	glm::mat4 globalTransform;
	FbxAMatrix GlobalTransform;
};

struct AnimusBone
{
	int parentBoneIndex = -1;
	std::string name = "AnimusBone";
	AnimusFrame* animation = 0;
	FbxNode *node = 0;
	glm::vec4 start;
	glm::vec4 end;
	glm::vec4 position;
	glm::mat4 globalBindposeInverse;
	glm::mat4 nodeTransform;
	glm::mat4 linkTransform;
	glm::mat4 geoTransform;
	FbxAMatrix GlobalBindposeInverse;
	FbxAMatrix NodeTransform;
	FbxAMatrix LinkTransform;
	FbxAMatrix GeoTransform;
};

class AnimusAnimationNode :
	public AnimusNode
{
private:
	void glSetUpBoneMesh();
	void glSetUpBoneBuffer();
	void glSetUpBoneShader();
	void glSetUpBoneUniform(const glm::mat4 &vertexMatrix);
	void glSetUpBoneAttrib(GLint _position);
	void glUpdateBoneMesh(int frame);
	void glUpdateBoneBuffer();

	void parseFbxAnimation(FbxNode* inNode, FbxScene* mFBXScene, int stackNum);
	void parseFbxBone(FbxNode* inNode, int parentIndex = -1);
	int FindBoneIndexUsingName(std::string _boneName);
	glm::mat4x4 FbxAMatrixToGlmMat(FbxAMatrix fMatrix);
public:
	AnimusAnimationNode();
	~AnimusAnimationNode();

	GLuint VAOs[1];
	GLuint VBOs[1];
	glm::vec4 *vertexPositions;
	int vertexCount;
	GLuint program;
	GLuint vert;
	GLuint frag;
	GLint position;
	char* vertSrc;
	char* fragSrc;
	void glSetUpBone(const glm::mat4 &vertexMatrix);
	void glUpdateBone(int frame);
	void glUpdateBoneMatrix(const glm::mat4 &vertexMatrix);

	std::vector<AnimusControlPointWeights> controlPoints;
	std::vector<AnimusBone> bones;
	int length;
	std::string name;
	int loadFbxAnimation(char * _filename, int stackNum);
	void printAll(char* filename);
};

