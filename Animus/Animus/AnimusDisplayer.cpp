#include "AnimusDisplayer.h"

const int LEFT_BUTTON = 0;
const int MIDDLE_BUTTON = 1;
const int RIGHT_BUTTON = 2;

const int BUTTON_DOWN = 0;
const int BUTTON_UP = 1;

AnimusMaterialNode AnimusDisplayer::lMat(1);
AnimusInstanceMaterialNode AnimusDisplayer::lIMat(1);

AnimusMeshNode AnimusDisplayer::lMesh;
AnimusInstanceMeshNode AnimusDisplayer::lIMesh;

AnimusTransformNode AnimusDisplayer::lTransformM;

AnimusCameraNode AnimusDisplayer::lCamera;

AnimusLightNode AnimusDisplayer::lLight;

AnimusAnimationNode AnimusDisplayer::lAnim;

AnimusMarkerNode AnimusDisplayer::lGrid(AnimusMarkerNodeType::Grid);

AnimusMarkerNode AnimusDisplayer::lAxis(AnimusMarkerNodeType::Axis);

glm::vec4 AnimusDisplayer::clearColor(0.3f, 0.3f, 0.3f, 1.0f);

int AnimusDisplayer::windowPositionX = 400;

int AnimusDisplayer::windowPositionY = 300;

int AnimusDisplayer::windowSizeX = 800;

int AnimusDisplayer::windowSizeY = 600;

int AnimusDisplayer::frame = 0;

int AnimusDisplayer::mouseLastX = 0;

int AnimusDisplayer::mouseLastY = 0;

float AnimusDisplayer::cameraPanStep = 1;

float AnimusDisplayer::cameraOrbitStep = 3.14 / 180;

float AnimusDisplayer::cameraZoomStep = 1;

bool AnimusDisplayer::mustUpdatePVM = false;

void AnimusDisplayer::initScene()
{	
	lCamera.position = glm::vec3(0, 20, 50);//(0, -50, -20); //(0, 700, 1500);//
	lCamera.perspectiveAspect = (float)windowSizeX / (float)windowSizeY;
	lCamera.perspectiveNear = 0.1f;
	lCamera.perspectiveFar = 2000.0f;//1600.0f;//

	lTransformM.position = glm::vec3(0, 0, 0);

	glm::mat4 PV = lCamera.calculatePerspectiveProjectMatrix()*lCamera.calculatePerspectiveViewMatrix();
	glm::mat4 PVM = PV*lTransformM.calculateModelMatrix();//glm::perspective<float>(60.0f / 180.0f * 3.14f, (float)AnimusDisplayer::windowSizeX / (float)AnimusDisplayer::windowSizeY, 0.1f, 1600.0f) * glm::translate(glm::mat4(), glm::vec3(0.0, -700.0, -1500.0));
	glm::vec4 LightDirection = lLight.direction;//from vertex to the light;

	lIMat.glSetUp(PV, LightDirection);
	glUseProgram(lIMat.program);
	GLint iposition = glGetAttribLocation(lIMat.program, "vPosition");
	GLint inormal = glGetAttribLocation(lIMat.program, "vNormal");//
	GLint itexcoord = glGetAttribLocation(lIMat.program, "vTexcoord");
	GLint imatrix = glGetAttribLocation(lIMat.program, "vMatrix");
	lIMesh.glSetUp(100, glm::vec3(0.0, 0.0, -20.0), glm::vec3(20.0, 0.0, 0.0), iposition, inormal, itexcoord, imatrix);
	glUseProgram(0);

	lMat.glSetUp(PVM, LightDirection);
	glUseProgram(lMat.program);
	GLint position = glGetAttribLocation(lMat.program, "vPosition");
	GLint normal = glGetAttribLocation(lMat.program, "vNormal");//
	GLint texcoord = glGetAttribLocation(lMat.program, "vTexcoord");
	lMesh.glSetUp(position, normal, texcoord);
	glUseProgram(0);

	lAnim.glSetUpBone(PVM);
	lAxis.glSetUpMarker(PVM);
	lGrid.glSetUpMarker(PV);


	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void AnimusDisplayer::renderScene(void)
{
	glm::mat4 PV = lCamera.calculatePerspectiveProjectMatrix()*lCamera.calculatePerspectiveViewMatrix();

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	frame = (++frame) % lAnim.length;

	////////////////////
	//Debug lMesh
	lMesh.glUpdateQuick(frame, &lAnim);
	lMat.glUpdateMatrix(PV*lTransformM.calculateModelMatrix());
	glUseProgram(lMat.program);
	glBindVertexArray(lMesh.VAOs[0]);
	glDrawElements(GL_TRIANGLES, lMesh.vertexIndexCount, GL_UNSIGNED_INT, (void*)0);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);
	/////////////////////

	////////////////////
	//Debug lIMesh
	lIMesh.glUpdateQuick(frame, &lAnim);
	lIMat.glUpdatePV(PV);
	glUseProgram(lIMat.program);
	glBindVertexArray(lIMesh.VAOs[0]);
	glDrawElementsInstanced(GL_TRIANGLES, lIMesh.vertexIndexCount, GL_UNSIGNED_INT, (void*)0, lIMesh.instanceCount);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);
	/////////////////////

	////////////////////
	//Debug lAnim
	lAnim.glUpdateBoneMatrix(PV*lTransformM.calculateModelMatrix());
	glDisable(GL_DEPTH_TEST);
	lAnim.glUpdateBone(frame);
	glUseProgram(lAnim.program);
	glBindVertexArray(lAnim.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lAnim.vertexCount);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
	////////////////////

	
	lAxis.glUpdateMarkerMatrix(PV*lTransformM.calculateModelMatrix());
	glUseProgram(lAxis.program);
	glBindVertexArray(lAxis.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lAxis.vertexCount);
	glBindVertexArray(0);

	lGrid.glUpdateMarkerMatrix(PV);
	glUseProgram(lGrid.program);
	glBindVertexArray(lGrid.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lGrid.vertexCount);
	glBindVertexArray(0);

	glutSwapBuffers();
	glutPostRedisplay();
}

void AnimusDisplayer::mouseButton(int button, int state, int x, int y)
{
	mouseLastX = x;
	mouseLastY = y;
	switch (button)
	{
	case LEFT_BUTTON:
		switch (state)
		{
		case BUTTON_DOWN:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Pan;
			break;
		case BUTTON_UP:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Idle;
			break;
		default:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Idle;
			break;
		}
		break;
	case RIGHT_BUTTON:
		switch (state)
		{
		case BUTTON_DOWN:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Orbit;
			break;
		case BUTTON_UP:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Idle;
			break;
		default:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Idle;
			break;
		}
		break;
	case MIDDLE_BUTTON:
		switch (state)
		{
		case BUTTON_DOWN:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Zoom;
			break;
		case BUTTON_UP:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Idle;
			break;
		default:
			AnimusDisplayer::lCamera.cameraState = AnimusCameraState::Idle;
			break;
		}
		break;
	default:
		break;
	}
}

void AnimusDisplayer::mouseMotion(int x, int y)
{
	AnimusCameraState currentState = AnimusDisplayer::lCamera.cameraState;

	if (currentState != AnimusCameraState::Idle)
	{
		int deltaX = x - AnimusDisplayer::mouseLastX;
		int deltaY = y - AnimusDisplayer::mouseLastY;//need to flip
		glm::vec3 zAxis = glm::normalize(AnimusDisplayer::lCamera.center - AnimusDisplayer::lCamera.position);//zAxis is forward for camera, hence using left hand system
		glm::vec3 xAxis = glm::normalize(glm::cross(zAxis, AnimusDisplayer::lCamera.up));
		glm::vec3 yAxis = glm::normalize(glm::cross(xAxis, zAxis));
		glm::vec4 relativePosition = glm::vec4((AnimusDisplayer::lCamera.position - AnimusDisplayer::lCamera.center), 1.0f);
		glm::vec3 currentPosition = AnimusDisplayer::lCamera.position;
		glm::vec3 currentCenter = AnimusDisplayer::lCamera.center;
		glm::vec3 resultPosition;
		glm::vec3 resultCenter;

		switch (currentState)
		{
		case AnimusCameraState::Pan:
			//when panning it is natural to assume we are manipulating the plane, so we need to do an additional flip to both deltaX and deltaY axis
			resultPosition = currentPosition - xAxis * AnimusDisplayer::cameraPanStep * (float)deltaX + yAxis * AnimusDisplayer::cameraPanStep * (float)deltaY;//flip, so total Y flip = 2, total X flip = 1
			resultCenter = currentCenter - xAxis * AnimusDisplayer::cameraPanStep * (float)deltaX + yAxis * AnimusDisplayer::cameraPanStep * (float)deltaY;//flip, so total Y flip = 2, total X flip = 1
			break;
		case AnimusCameraState::Orbit:
			//when orbiting it is natural to assume we are manipulating the plane, so we need to do an additional flip to both deltaX and deltaY
			relativePosition = glm::rotate(glm::mat4(), -AnimusDisplayer::cameraOrbitStep * (float)deltaX, AnimusDisplayer::lCamera.up) * relativePosition;//flip, so total X flip = 1
			relativePosition = glm::rotate(glm::mat4(), -AnimusDisplayer::cameraOrbitStep * (float)deltaY, xAxis) * relativePosition;//flip twice(because rotate is for right hand system), so total Y flip = 3
			resultPosition = currentCenter + (glm::vec3)relativePosition;
			resultCenter = currentCenter;
			break;
		case AnimusCameraState::Zoom:
			resultPosition = currentPosition + zAxis * AnimusDisplayer::cameraZoomStep * (float)deltaX;
			resultCenter = currentCenter;
			break;
		default:
			break;
		}
		AnimusDisplayer::mouseLastX = x;
		AnimusDisplayer::mouseLastY = y;
		AnimusDisplayer::lCamera.position = resultPosition;
		AnimusDisplayer::lCamera.center = resultCenter;
		if (resultPosition.x * currentPosition.x < 0 && resultPosition.z * currentPosition.z < 0)
		{
			AnimusDisplayer::lCamera.up = -AnimusDisplayer::lCamera.up;
		}
		AnimusDisplayer::mustUpdatePVM = true;
	}
}

void AnimusDisplayer::mainLoop(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(windowPositionX, windowPositionY);
	glutInitWindowSize(windowSizeX, windowSizeY);
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Animus_pre_alpha");
	glewInit();

	GLubyte const *v = glGetString(GL_VERSION);
	std::cout << v << std::endl;

	initScene();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);

	glutMainLoop();
}
