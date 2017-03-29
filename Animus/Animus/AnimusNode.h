#pragma once
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glew\glew.h"

enum AnimusNodeType
{
	Mesh, Animation, Camera, Transform, Light, Material, Marker, AnimusNodeTypeCount
};

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

