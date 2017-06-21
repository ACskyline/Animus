#include "AnimusDisplayer.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

const int LEFT_BUTTON = GLFW_MOUSE_BUTTON_1;
const int RIGHT_BUTTON = GLFW_MOUSE_BUTTON_2;
const int MIDDLE_BUTTON = GLFW_MOUSE_BUTTON_3;

const int BUTTON_DOWN = GLFW_PRESS;
const int BUTTON_UP = GLFW_RELEASE;

AnimusMaterialNode AnimusDisplayer::lMat(1);
AnimusInstanceMaterialNode AnimusDisplayer::lIMat(1);

AnimusMeshNode AnimusDisplayer::lMesh;
AnimusInstanceMeshNode AnimusDisplayer::lIMesh;

AnimusTransformNode AnimusDisplayer::lTransformM;

AnimusCameraNode AnimusDisplayer::lCamera;

AnimusLightNode AnimusDisplayer::lLight;

AnimusAnimationNode AnimusDisplayer::lAnim0;

AnimusAnimationNode AnimusDisplayer::lAnim1;

AnimusMarkerNode AnimusDisplayer::lGrid(AnimusMarkerNodeType::Grid);

AnimusMarkerNode AnimusDisplayer::lAxis(AnimusMarkerNodeType::Axis);

AnimusMarkerNode AnimusDisplayer::lCircle0(AnimusMarkerNodeType::Circle);//new new new

glm::vec4 AnimusDisplayer::clearColor(0.3f, 0.3f, 0.3f, 1.0f);

int AnimusDisplayer::windowPositionX = 400;

int AnimusDisplayer::windowPositionY = 300;

int AnimusDisplayer::windowSizeX = WINDOW_INITIAL_WIDTH;

int AnimusDisplayer::windowSizeY = WINDOW_INITIAL_HEIGHT;

int AnimusDisplayer::frame = 0;

int AnimusDisplayer::mouseLastX = 0;

int AnimusDisplayer::mouseLastY = 0;

float AnimusDisplayer::cameraPanStep = 1;

float AnimusDisplayer::cameraOrbitStep = 3.14 / 180;

float AnimusDisplayer::cameraZoomStep = 1;

float AnimusDisplayer::cameraZoomLimit = 1;

bool AnimusDisplayer::mustUpdatePVM = false;

float AnimusDisplayer::instanceCenterZoomStep = 1;

clock_t AnimusDisplayer::begin;

clock_t AnimusDisplayer::end;

glm::mat4 AnimusDisplayer::V;

glm::mat4 AnimusDisplayer::P;

glm::mat4 AnimusDisplayer::M;

glm::vec4 AnimusDisplayer::LightDirection;

void AnimusDisplayer::initScene()
{	
	lCamera.position = glm::vec3(CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z);
	lCamera.perspectiveAspect = (float)windowSizeX / (float)windowSizeY;
	lCamera.perspectiveNear = CAMERA_NEAR;
	lCamera.perspectiveFar = CAMERA_FAR;

	lTransformM.position = glm::vec3(0, 0, -5);

	P = lCamera.calculatePerspectiveProjectMatrix();
	V = lCamera.calculatePerspectiveViewMatrix();
	M = lTransformM.calculateModelMatrix();
	glm::mat4 PV = P*V;
	glm::mat4 PVM = PV*M;
	LightDirection = lLight.direction;

	lIMat.glSetUp(PV, M, LightDirection, &lAnim0, &lAnim1);
	glUseProgram(lIMat.program);
	GLint iposition = glGetAttribLocation(lIMat.program, "vPosition");
	GLint inormal = glGetAttribLocation(lIMat.program, "vNormal");//
	GLint itexcoord = glGetAttribLocation(lIMat.program, "vTexcoord");
	GLint iboneW = glGetAttribLocation(lIMat.program, "boneWeight");
	GLint iboneI = glGetAttribLocation(lIMat.program, "boneIndex");
	GLint imatrix = glGetAttribLocation(lIMat.program, "vMatrix");//PER-INSTANCE
	GLint iframe = glGetAttribLocation(lIMat.program, "frame");//PER-INSTANCE
	GLint ianim = glGetAttribLocation(lIMat.program, "anim");//PER-INSTANCE
	lIMesh.glSetUp(INSTANCE_INITIAL_COUNT, glm::vec3(0.0, 0.0, -INSTANCE_INITIAL_STRIDE), glm::vec3(INSTANCE_INITIAL_STRIDE, 0.0, 0.0), glm::vec3(0.0, 0.0, -INSTANCE_INITIAL_STRIDE), &lAnim0, iposition, inormal, itexcoord, iboneW, iboneI, imatrix, iframe, ianim);
	glUseProgram(0);

	lMat.glSetUp(PVM, LightDirection);
	glUseProgram(lMat.program);
	GLint position = glGetAttribLocation(lMat.program, "vPosition");
	GLint normal = glGetAttribLocation(lMat.program, "vNormal");//
	GLint texcoord = glGetAttribLocation(lMat.program, "vTexcoord");
	lMesh.glSetUp(position, normal, texcoord);
	glUseProgram(0);

	lAnim0.glSetUpBone(PVM);
	lAxis.glSetUpMarker(PV * glm::scale(glm::mat4(), glm::vec3(MARKER_SCALE)));
	lGrid.glSetUpMarker(PV * glm::scale(glm::mat4(), glm::vec3(MARKER_SCALE)));
	lCircle0.glSetUpMarker(PV * glm::translate(glm::mat4(), lIMesh.center) * glm::scale(glm::mat4(), glm::vec3(lIMesh.radius)));

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void AnimusDisplayer::renderScene(void)
{
	begin = clock();

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	frame = (++frame) % lAnim0.length;

	if (mustUpdatePVM)
	{
		P = lCamera.calculatePerspectiveProjectMatrix();
		V = lCamera.calculatePerspectiveViewMatrix();
		glm::mat4 PV = P*V;

		lMat.glUpdateMatrix(PV*lTransformM.calculateModelMatrix());
		lIMat.glUpdatePV(PV);
		lAnim0.glUpdateBoneMatrix(PV*lTransformM.calculateModelMatrix());
		lAxis.glUpdateMarkerMatrix(PV * glm::scale(glm::mat4(), glm::vec3(MARKER_SCALE)));
		lGrid.glUpdateMarkerMatrix(PV * glm::scale(glm::mat4(), glm::vec3(MARKER_SCALE)));
		lCircle0.glUpdateMarkerMatrix(PV * glm::translate(glm::mat4(), lIMesh.center) * glm::scale(glm::mat4(), glm::vec3(lIMesh.radius)));

		mustUpdatePVM = false;
	}

	////////////////////
	//Debug lMesh
	lMesh.glUpdateQuick(frame, &lAnim0);
	glUseProgram(lMat.program);
	glBindVertexArray(lMesh.VAOs[0]);
	glDrawElements(GL_TRIANGLES, lMesh.vertexIndexCount, GL_UNSIGNED_INT, (void*)0);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);
	/////////////////////

	////////////////////
	//Debug lIMesh
	lIMesh.glUpdate(&lAnim0, &lAnim1);
	glUseProgram(lIMat.program);
	glBindVertexArray(lIMesh.VAOs[0]);
	glDrawElementsInstanced(GL_TRIANGLES, lIMesh.vertexIndexCount, GL_UNSIGNED_INT, (void*)0, lIMesh.instanceCount);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);
	/////////////////////

	////////////////////
	//Debug lAnim
	glDisable(GL_DEPTH_TEST);
	lAnim0.glUpdateBone(frame);
	glUseProgram(lAnim0.program);
	glBindVertexArray(lAnim0.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lAnim0.vertexCount);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
	////////////////////

	////////////////////
	//Debug Marker
	glUseProgram(lAxis.program);
	glBindVertexArray(lAxis.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lAxis.vertexCount);
	glBindVertexArray(0);

	glUseProgram(lGrid.program);
	glBindVertexArray(lGrid.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lGrid.vertexCount);
	glBindVertexArray(0);

	glUseProgram(lCircle0.program);
	glBindVertexArray(lCircle0.VAOs[0]);
	glDrawArrays(GL_LINES, 0, lCircle0.vertexCount);
	glBindVertexArray(0);
	/////////////////////

	end = clock();
	//printf("\r%ffps", (float)CLOCKS_PER_SEC / (end - begin));
}

void AnimusDisplayer::mouseButton(GLFWwindow* window, int button, int action, int mods)
{
	//this function is not used
	printf("button:%d,action:%d,mods:%d\n", button, action, mods);
}

void AnimusDisplayer::mouseMotion(GLFWwindow* window, double x, double y)
{
	//printf("x:%lf,y:%lf\n", x, y);
	int lctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
	int lalt = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
	if (lctrl == GLFW_PRESS)
	{
		mouseMotionInstanceCenterControlMode(x, y);
	}
	if (lalt == GLFW_PRESS)
	{
		mouseMotionCameraMode(x, y);
	}
}

void AnimusDisplayer::mouseButtonCameraMode(int button, int state, int x, int y)
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

void AnimusDisplayer::mouseButtonInstanceCenterControlMode(int button, int state, int x, int y)
{
	mouseLastX = x;
	mouseLastY = y;
	switch (button)
	{
	case LEFT_BUTTON:
		switch (state)
		{
		case BUTTON_DOWN:
			AnimusDisplayer::lIMesh.centerControlState = AnimusInstanceCenterControlState::Move;
			break;
		case BUTTON_UP:
			AnimusDisplayer::lIMesh.centerControlState = AnimusInstanceCenterControlState::Idle;
			break;
		default:
			AnimusDisplayer::lIMesh.centerControlState = AnimusInstanceCenterControlState::Idle;
			break;
		}
		break;
	case RIGHT_BUTTON:
		switch (state)
		{
		case BUTTON_DOWN:
			//do nothing
			break;
		case BUTTON_UP:
			//do nothing
			break;
		default:
			//do nothing
			break;
		}
		break;
	case MIDDLE_BUTTON:
		switch (state)
		{
		case BUTTON_DOWN:
			AnimusDisplayer::lIMesh.centerControlState = AnimusInstanceCenterControlState::Zoom;
			break;
		case BUTTON_UP:
			AnimusDisplayer::lIMesh.centerControlState = AnimusInstanceCenterControlState::Idle;
			break;
		default:
			AnimusDisplayer::lIMesh.centerControlState = AnimusInstanceCenterControlState::Idle;
			break;
		}
		break;
	default:
		break;
	}
}

void AnimusDisplayer::mouseMotionCameraMode(int x, int y)
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
			if ((resultPosition.x - currentCenter.x) * (currentPosition.x - currentCenter.x) < 0 && (resultPosition.z - currentCenter.z) * (currentPosition.z - currentCenter.z) < 0)
			{
				AnimusDisplayer::lCamera.up = -AnimusDisplayer::lCamera.up;
			}
			break;
		case AnimusCameraState::Zoom:
			resultPosition = currentPosition + zAxis * AnimusDisplayer::cameraZoomStep * (float)deltaX;
			if (deltaX > 0 && glm::length(resultPosition - currentPosition) + AnimusDisplayer::cameraZoomLimit >= glm::length(currentCenter - currentPosition))//closing in && longer
			{
				resultPosition = currentPosition;
			}
			resultCenter = currentCenter;
			break;
		default:
			break;
		}
		AnimusDisplayer::mouseLastX = x;
		AnimusDisplayer::mouseLastY = y;
		AnimusDisplayer::lCamera.position = resultPosition;
		AnimusDisplayer::lCamera.center = resultCenter;

		AnimusDisplayer::mustUpdatePVM = true;
	}
}

void AnimusDisplayer::mouseMotionInstanceCenterControlMode(int x, int y)
{
	AnimusInstanceCenterControlState currentState = AnimusDisplayer::lIMesh.centerControlState;

	if (currentState != AnimusInstanceCenterControlState::Idle)
	{
		int deltaX = x - AnimusDisplayer::mouseLastX;
		int deltaY = y - AnimusDisplayer::mouseLastY;//need to flip

		switch (currentState)
		{
		case AnimusInstanceCenterControlState::Move:
			{
				glm::vec4 rayDirTemp = glm::vec4((float)x / windowSizeX * 2 - 1, 1 - (float)y / windowSizeY * 2, -1, 1);
				rayDirTemp = glm::inverse(AnimusDisplayer::P) * rayDirTemp;
				rayDirTemp = glm::normalize(glm::inverse(AnimusDisplayer::V) * glm::vec4(rayDirTemp.x, rayDirTemp.y, -1, 0));
				glm::vec3 rayDir = glm::vec3(rayDirTemp.x, rayDirTemp.y, rayDirTemp.z);
				glm::vec3 rayPoint = AnimusDisplayer::lCamera.position;
				glm::vec3 planeNormal = glm::vec3(0, 1, 0);
				glm::vec3 planePoint = glm::vec3(0, 0, 0);
				float t = (planeNormal.x * (planePoint.x - rayPoint.x) + planeNormal.y * (planePoint.y - rayPoint.y) + planeNormal.z * (planePoint.z - rayPoint.z)) / (planeNormal.x * rayDir.x + planeNormal.y * rayDir.y + planeNormal.z * rayDir.z);
				AnimusDisplayer::lIMesh.center = rayPoint + t * rayDir;
			}
			break;
		case AnimusInstanceCenterControlState::Zoom:
			AnimusDisplayer::lIMesh.radius += deltaX * AnimusDisplayer::instanceCenterZoomStep;
			break;
		default:
			break;
		}
		AnimusDisplayer::mouseLastX = x;
		AnimusDisplayer::mouseLastY = y;

		AnimusDisplayer::mustUpdatePVM = true;
	}
}

int AnimusDisplayer::mainLoop(int argc, char**argv)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(windowSizeX, windowSizeY, "Animus", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//glfwSetMouseButtonCallback(window, mouseButton);//No need here
	glfwSetCursorPosCallback(window, mouseMotion);//

	glfwMakeContextCurrent(window);
	glewInit();
	initScene();

	GLubyte const *v = glGetString(GL_VERSION);
	printf("%s\n", v);

	ImGui_ImplGlfwGL3_Init(window, true);

	//bool show_test_window = true;
	//bool show_another_window = false;
	int instanceCountUI = INSTANCE_INITIAL_COUNT;
	ImVec4 clear_color = ImColor(114, 144, 154);
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(150, 400));

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//Must add GL_DEPTH_BUFFER_BIT. Because ImGui's depth will always be the shallowest, it will block everything.

		renderScene();

		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Begin("Tools");
			if (ImGui::SliderInt("Instances", &instanceCountUI, 1, 10000))
			{
				//AnimusDisplayer::lIMesh.glResetAll(instanceCountUI, glm::vec3(0, 0, -INSTANCE_INITIAL_STRIDE), glm::vec3(0, 0, -INSTANCE_INITIAL_STRIDE), &AnimusDisplayer::lAnim0);
				AnimusDisplayer::lIMesh.glResetInstances(instanceCountUI, glm::vec3(0, 0, -INSTANCE_INITIAL_STRIDE), glm::vec3(0, 0, -INSTANCE_INITIAL_STRIDE));
			}
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::TextColored(ImVec4(0.4, 0.3, 0.0, 1.0), "Use mouse + left Alt to move the camera.");
			ImGui::TextColored(ImVec4(0.0, 0.5, 0.3, 1.0), "Use mouse + left Ctrl to move the crowd.");
			if (ImGui::CollapsingHeader("Camera"))
			{
				if (ImGui::TreeNode("position"))
				{
					ImGui::Text("x:%f,y:%f,z:%f", AnimusDisplayer::lCamera.position.x, AnimusDisplayer::lCamera.position.y, AnimusDisplayer::lCamera.position.z);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("focus"))
				{
					ImGui::Text("x:%f,y:%f,z:%f", AnimusDisplayer::lCamera.center.x, AnimusDisplayer::lCamera.center.y, AnimusDisplayer::lCamera.center.z);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("up"))
				{
					ImGui::Text("x:%f,y:%f,z:%f", AnimusDisplayer::lCamera.up.x, AnimusDisplayer::lCamera.up.y, AnimusDisplayer::lCamera.up.z);
					ImGui::TreePop();
				}
			}
			if (ImGui::CollapsingHeader("Circle"))
			{
				if (ImGui::TreeNode("center"))
				{
					ImGui::Text("x:%f,y:%f,z:%f", AnimusDisplayer::lIMesh.center.x, AnimusDisplayer::lIMesh.center.y, AnimusDisplayer::lIMesh.center.z);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("radius"))
				{
					ImGui::Text("r:%f", AnimusDisplayer::lIMesh.radius);
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}

		//// 1. Show a simple window
		//// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
		//{
		//	static float f = 0.0f;
		//	ImGui::Text("Hello, world!");
		//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		//	ImGui::ColorEdit3("clear color", (float*)&clear_color);
		//	if (ImGui::Button("Test Window")) show_test_window ^= 1;
		//	if (ImGui::Button("Another Window")) show_another_window ^= 1;
		//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//}

		//// 2. Show another simple window, this time using an explicit Begin/End pair
		//if (show_another_window)
		//{
		//	ImGui::SetNextWindowSize(ImVec2(10, 10), ImGuiSetCond_FirstUseEver);
		//	ImGui::Begin("Another Window", &show_another_window);
		//	ImGui::Text("Hello");
		//	ImGui::End();
		//}

		//// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
		//if (show_test_window)
		//{
		//	ImGui::SetNextWindowPos(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
		//	ImGui::ShowTestWindow(&show_test_window);
		//}

		// Rendering ImGui
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui::Render();

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();

	return 0;
}