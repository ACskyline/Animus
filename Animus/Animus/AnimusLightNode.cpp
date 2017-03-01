#include "AnimusLightNode.h"


AnimusLightNode::AnimusLightNode()
{
	setNodeType(AnimusNodeType::Light);
	direction = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);
}


AnimusLightNode::~AnimusLightNode()
{
}
