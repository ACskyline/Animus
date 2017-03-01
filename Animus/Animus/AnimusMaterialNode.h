#pragma once
#include <sstream>
#include "AnimusNode.h"
#include "Dependencies\gli\gli.hpp"
class AnimusMaterialNode :
	public AnimusNode
{
private:
	void glSetUpShader();
	void glSetUpTexture(int index = 0);
	void glSetUpUniforms(const glm::mat4 &vertexMatrix, const glm::vec4 &LightDirection);
public:
	int textureCount;
	gli::texture *textures;
	GLuint *TEXs;
	GLuint program;
	GLuint vert;
	GLuint frag;
	glm::vec4 vertexColor;
	char *vertSrc;
	char *fragSrc;
	AnimusMaterialNode(int _textureCount = 1);
	~AnimusMaterialNode();
	void glSetUp(const glm::mat4 &vertexMatrix, const glm::vec4 &LightDirection);
	int loadTex(char* filename, int index = 0);
};

