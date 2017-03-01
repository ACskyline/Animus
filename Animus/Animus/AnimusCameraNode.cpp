#include "AnimusCameraNode.h"


AnimusCameraNode::AnimusCameraNode()
{
	setNodeType(AnimusNodeType::Camera);
	cameraType = AnimusCameraType::Perspective;
	perspectiveFovy = 60.0 / 180.0 * 3.14;
	perspectiveAspect = 1;
	perspectiveNear = 1;
	perspectiveFar = 100;
	perspectiveProjectMatrix = glm::mat4(1);
}


AnimusCameraNode::~AnimusCameraNode()
{
}

//all degrees are in radius
glm::mat4 AnimusCameraNode::calculatePerspectiveProjectMatrix()
{
	perspectiveProjectMatrix = glm::perspective<float>(perspectiveFovy, perspectiveAspect, perspectiveNear, perspectiveFar);
	return perspectiveProjectMatrix;
}