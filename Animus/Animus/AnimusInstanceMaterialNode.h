#pragma once
#include <sstream>
#include "AnimusNode.h"
#include "Dependencies\gli\gli.hpp"
#include "AnimusAnimationNode.h"
#include <sstream>

#define ShaderInfoLogLength 640
#define RowPerMatrix 4 //total num of rows of a matrix which is sent to shader using texture buffer

class AnimusInstanceMaterialNode :
	public AnimusNode
{
private:
	void glSetUpShader(int _boneCount, int _frameCount);
	void glSetUpTexture(int index = 0);
	void glSetUpUniforms(const glm::mat4 &PV, const glm::vec4 &LightDirection, AnimusAnimationNode *pAnim0, AnimusAnimationNode *pAnim1);
	void glSetUpTexBuffer(AnimusAnimationNode *pAnim, int slot);
public:

	int textureCount;
	int boneCount;
	int frameCount;
	gli::texture *textures;

	GLuint TBOs[2];// new new new
	GLuint TBTEXs[2];//new new new

	GLuint *TEXs;
	GLuint program;
	GLuint vert;
	GLuint frag;
	glm::vec4 vertexColor;
	//std::string vertStr;
	const char *vertSrc;
	const char *fragSrc;

	AnimusInstanceMaterialNode(int _textureCount = 0);
	~AnimusInstanceMaterialNode();

	void glSetUp(const glm::mat4 &PV, const glm::vec4 &LightDirection, AnimusAnimationNode *pAnim0, AnimusAnimationNode *pAnim1);
	void glUpdatePV(const glm::mat4 &PV);
	int loadDdsTex(char* filename, int index = 0);
};

