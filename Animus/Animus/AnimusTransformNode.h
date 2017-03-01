#pragma once
#include "AnimusNode.h"
class AnimusTransformNode :
	public AnimusNode
{
private:
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 modelMatrix;
	glm::mat4 inverseModelMatrix;
	AnimusTransformNode();
	~AnimusTransformNode();
	glm::mat4 calculateModelMatrix();
	glm::mat4 calculateInverseModelMatrix();
	glm::mat4 calculateViewMatrix();
};

