#include "AnimusDisplayer.h"

AnimusMaterialNode AnimusDisplayer::lMat;

AnimusMeshNode AnimusDisplayer::lMesh;

AnimusTransformNode AnimusDisplayer::lTransformC;

AnimusTransformNode AnimusDisplayer::lTransformM;

AnimusCameraNode AnimusDisplayer::lCamera;

AnimusLightNode AnimusDisplayer::lLight;

int AnimusDisplayer::windowPositionX = 400;

int AnimusDisplayer::windowPositionY = 300;

int AnimusDisplayer::windowSizeX = 800;

int AnimusDisplayer::windowSizeY = 600;

void AnimusDisplayer::init()
{	
	lTransformC.position = glm::vec3(0, 700, 1500);
	lCamera.perspectiveAspect = (float)windowSizeX / (float)windowSizeY;
	lCamera.perspectiveNear = 0.1f;
	lCamera.perspectiveFar = 1600.0f;

	glm::mat4 vertexMatrix = lCamera.calculatePerspectiveProjectMatrix()*lTransformC.calculateInverseModelMatrix()*lTransformM.calculateModelMatrix();//glm::perspective<float>(60.0f / 180.0f * 3.14f, (float)AnimusDisplayer::windowSizeX / (float)AnimusDisplayer::windowSizeY, 0.1f, 1600.0f) * glm::translate(glm::mat4(), glm::vec3(0.0, -700.0, -1500.0));
	glm::vec4 LightDirection = lLight.direction;//from vertex to the light;

	lMat.glSetUp(vertexMatrix, LightDirection);
	glUseProgram(lMat.program);
	GLint position = glGetAttribLocation(lMat.program, "vPosition");
	GLint normal = glGetAttribLocation(lMat.program, "vNormal");//
	GLint texcoord = glGetAttribLocation(lMat.program, "vTexcoord");
	lMesh.glSetUp(position, normal, texcoord);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void AnimusDisplayer::renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(lMat.program);

	glBindVertexArray(lMesh.VAOs[0]);
	glDrawElements(GL_TRIANGLES, lMesh.vertexIndexCount, GL_UNSIGNED_INT, (void*)0);//modified pipeline does not accept GL_QUADS as a parameter
	glBindVertexArray(0);

	glutSwapBuffers();
}

void AnimusDisplayer::mainLoop(int argc, char**argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(windowPositionX, windowPositionY);
	glutInitWindowSize(windowSizeX, windowSizeY);
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Drawing my first triangle");
	glewInit();

	GLubyte const *v = glGetString(GL_VERSION);
	std::cout << v << std::endl;

	init();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutMainLoop();
	glDeleteProgram(lMat.program);
}
