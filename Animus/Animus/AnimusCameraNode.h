#pragma once
#include "AnimusNode.h"

enum AnimusCameraState
{
	Pan, Orbit, Zoom, Idle, AnimusCameraStateCount
};

class AnimusCameraNode :
	public AnimusNode
{
public:
	AnimusCameraState cameraState;
	float perspectiveFovy;
	float perspectiveAspect;
	float perspectiveNear;
	float perspectiveFar;
	glm::vec3 position;
	glm::vec3 center;
	glm::vec3 up;
	glm::mat4 perspectiveProjectMatrix;
	glm::mat4 perspectiveViewMatrix;
	AnimusCameraNode();
	~AnimusCameraNode();
	glm::mat4 calculatePerspectiveProjectMatrix();
	glm::mat4 calculatePerspectiveViewMatrix();
};

