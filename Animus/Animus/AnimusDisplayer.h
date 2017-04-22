#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
#include "Dependencies\glm\gtc\type_ptr.hpp"
#include "AnimusMaterialNode.h"
#include "AnimusMeshNode.h"
#include "AnimusTransformNode.h"
#include "AnimusCameraNode.h"
#include "AnimusLightNode.h"
#include "AnimusAnimationNode.h"
#include "AnimusMarkerNode.h"
#include "AnimusInstanceMaterialNode.h"
#include "AnimusInstanceMeshNode.h"
#include <iostream>
#include <vector>

class AnimusDisplayer
{
private:
	static void initScene();
	static void renderScene(void);
	static void mouseButton(int button, int state, int x, int y);
	static void mouseMotion(int x, int y);
	static int windowPositionX;
	static int windowPositionY;
	static int windowSizeX;
	static int windowSizeY;
	static int frame;
	static glm::vec4 clearColor;
	static int mouseLastX;
	static int mouseLastY;
	static float cameraPanStep;
	static float cameraOrbitStep;
	static float cameraZoomStep;
	static float cameraZoomLimit;
	static bool mustUpdatePVM;
public:
	static AnimusMaterialNode lMat;
	static AnimusMeshNode lMesh;
	static AnimusTransformNode lTransformM;
	static AnimusCameraNode lCamera;
	static AnimusLightNode lLight;
	static AnimusAnimationNode lAnim0;
	static AnimusAnimationNode lAnim1;
	static AnimusMarkerNode lGrid;
	static AnimusMarkerNode lAxis;

	static AnimusInstanceMaterialNode lIMat;
	static AnimusInstanceMeshNode lIMesh;

	AnimusDisplayer();
	~AnimusDisplayer();
	static void mainLoop(int argc, char**argv);
};

