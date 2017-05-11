#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\glfw\glfw3.h"
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

#define MARKER_SCALE 10
#define CAMERA_POS_X 0
#define CAMERA_POS_Y 50
#define CAMERA_POS_Z 100
#define CAMERA_FAR 3000
#define CAMERA_NEAR 0.1
#define INSTANCE_INITIAL_COUNT 100
#define INSTANCE_INITIAL_STRIDE 20

class AnimusDisplayer
{
private:
	static glm::mat4 P;
	static glm::mat4 V;
	static glm::mat4 M;
	static glm::vec4 LightDirection;
	static void initScene();
	static void renderScene(void);
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
	static float instanceCenterZoomStep;
	static clock_t begin;
	static clock_t end;
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

	static AnimusMarkerNode lCircle0;//new new new

	static void mouseButton(GLFWwindow *window, int button, int action, int mods);
	static void mouseMotion(GLFWwindow *window, double x, double y);
	static void mouseButtonCameraMode(int button, int state, int x, int y);
	static void mouseMotionCameraMode(int x, int y);
	static void mouseButtonInstanceCenterControlMode(int button, int state, int x, int y);
	static void mouseMotionInstanceCenterControlMode(int x, int y);
	AnimusDisplayer();
	~AnimusDisplayer();
	static int mainLoop(int argc, char**argv);
};

