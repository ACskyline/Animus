#include "AnimusCameraNode.h"

AnimusCameraNode::AnimusCameraNode()
{
	setNodeType(AnimusNodeType::Camera);
	cameraState = AnimusCameraState::Idle;
	perspectiveFovy = 60.0 / 180.0 * 3.14;
	perspectiveAspect = 1;
	perspectiveNear = 1;
	perspectiveFar = 100;
	perspectiveProjectMatrix = glm::mat4(1);

	position = glm::vec3(0.0f, 0.0f, 0.0f);
	center = glm::vec3(0.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
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

glm::mat4 AnimusCameraNode::calculatePerspectiveViewMatrix()
{
	perspectiveViewMatrix = glm::lookAt(position, center, up);
	return perspectiveViewMatrix;
}