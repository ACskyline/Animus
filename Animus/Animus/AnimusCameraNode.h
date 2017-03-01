#pragma once
#include "AnimusNode.h"

enum AnimusCameraType
{
	Orthodox, Perspective, AnimusCameraTypeCount
};

class AnimusCameraNode :
	public AnimusNode
{
public:
	AnimusCameraType cameraType;
	float perspectiveFovy;
	float perspectiveAspect;
	float perspectiveNear;
	float perspectiveFar;
	glm::mat4 perspectiveProjectMatrix;
	AnimusCameraNode();
	~AnimusCameraNode();
	glm::mat4 calculatePerspectiveProjectMatrix();
};

