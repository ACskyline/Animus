#include "AnimusLightNode.h"


AnimusLightNode::AnimusLightNode()
{
	setNodeType(AnimusNodeType::Light);
	direction = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);
	position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}


AnimusLightNode::~AnimusLightNode()
{
}
