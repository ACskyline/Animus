#include "AnimusNode.h"

int AnimusNode::NodeCount = 0;

AnimusNode::AnimusNode()
{
	nodeType = AnimusNodeType::AnimusNodeTypeCount;
	nodeIndex = NodeCount++;//start at 0
}


AnimusNode::~AnimusNode()
{
}

AnimusNodeType AnimusNode::getNodeType()
{
	return nodeType;
}


void AnimusNode::setNodeType(AnimusNodeType _nodeType)
{
	nodeType = _nodeType;
}