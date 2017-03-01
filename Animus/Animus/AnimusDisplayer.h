#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "AnimusType.h"
#include "AnimusMaterialNode.h"
#include "AnimusMeshNode.h"
#include "AnimusTransformNode.h"
#include "AnimusCameraNode.h"
#include "AnimusLightNode.h"
#include <iostream>
#include <vector>

class AnimusDisplayer
{
private:
	static void init();
	static void renderScene(void);
	static int windowPositionX;
	static int windowPositionY;
	static int windowSizeX;
	static int windowSizeY;
public:
	static AnimusMaterialNode lMat;
	static AnimusMeshNode lMesh;
	static AnimusTransformNode lTransformC;
	static AnimusTransformNode lTransformM;
	static AnimusCameraNode lCamera;
	static AnimusLightNode lLight;
	AnimusDisplayer();
	~AnimusDisplayer();
	static void mainLoop(int argc, char**argv);
};

