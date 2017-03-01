#pragma once
#include "AnimusType.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"

class AnimusNode
{
private:
	static int NodeCount;
	int nodeIndex;
	AnimusNodeType nodeType;
public:
	AnimusNode();
	~AnimusNode();
	AnimusNodeType getNodeType();
	void setNodeType(AnimusNodeType _nodeType);
};

