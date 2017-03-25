#pragma once
#include "AnimusNode.h"

class AnimusLightNode :
	public AnimusNode
{
public:
	glm::vec4 direction;
	glm::vec4 position;
	AnimusLightNode();
	~AnimusLightNode();
};

