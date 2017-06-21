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
	void glSetUpShader();
	void glSetUpTexture(int index = 0);
	void glSetUpUniforms(const glm::mat4 &PV, const glm::mat4 &AdjustMatrix, const glm::vec4 &LightDirection, AnimusAnimationNode *pAnim0, AnimusAnimationNode *pAnim1);
	void glSetUpTexBuffer(AnimusAnimationNode *pAnim, int slot);
public:

	int textureCount;
	//int boneCount;
	//int frameCount;
	gli::texture *textures;

	GLuint TBOs[2];// new new new // one for anim0, one for anim1
	GLuint TBTEXs[2];//new new new // one for anim0, one for anim1

	GLuint *TEXs;
	GLuint program;
	GLuint vert;
	GLuint frag;
	glm::vec4 vertexColor;
	const char *vertSrc;
	const char *fragSrc;

	AnimusInstanceMaterialNode(int _textureCount = 0);
	~AnimusInstanceMaterialNode();

	void glSetUp(const glm::mat4 &PV, const glm::mat4 &AdjustMatrix, const glm::vec4 &LightDirection, AnimusAnimationNode *pAnim0, AnimusAnimationNode *pAnim1);
	void glUpdatePV(const glm::mat4 &PV);
	int loadDdsTex(char* filename, int index = 0);
};

