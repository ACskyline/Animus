#pragma once
#include <sstream>
#include "AnimusNode.h"
#include "Dependencies\gli\gli.hpp"
class AnimusInstanceMaterialNode :
	public AnimusNode
{
private:
	void glSetUpShader();
	void glSetUpTexture(int index = 0);
	void glSetUpUniforms(const glm::mat4 &PV, const glm::vec4 &LightDirection);
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

	AnimusInstanceMaterialNode(int _textureCount = 0);
	~AnimusInstanceMaterialNode();

	void glSetUp(const glm::mat4 &PV, const glm::vec4 &LightDirection);
	void glUpdatePV(const glm::mat4 &PV);
	int loadDdsTex(char* filename, int index = 0);
};

