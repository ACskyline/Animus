#include "AnimusDisplayer.h"

GLuint AnimusDisplayer::program = 0;

GLuint AnimusDisplayer::VAOs[1] = { 0 };

GLuint AnimusDisplayer::VBOs[1] = { 0 };

GLuint AnimusDisplayer::EBOs[1] = { 0 };

int AnimusDisplayer::vertexCount = 0;

int AnimusDisplayer::vertexIndexCount = 0;

glm::vec4 *AnimusDisplayer::vertexPositions = 0;

glm::vec2 *AnimusDisplayer::vertexTexcoords = 0;

glm::vec3 *AnimusDisplayer::vertexNormals = 0;

GLuint *AnimusDisplayer::vertexIndices = 0;

glm::vec4 AnimusDisplayer::vertexColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);

glm::vec4 AnimusDisplayer::LightDirection = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);

glm::mat4 AnimusDisplayer::vertexMatrix = glm::perspective<float>(60.0f/180.0f*3.14f, (float)AnimusDisplayer::windowSizeX / (float)AnimusDisplayer::windowSizeY, 0.1f, 100.0f) * glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, -3.0));

int AnimusDisplayer::windowPositionX = 400;

int AnimusDisplayer::windowPositionY = 300;

int AnimusDisplayer::windowSizeX = 800;

int AnimusDisplayer::windowSizeY = 600;

char* AnimusDisplayer::vertSrc =
"#version 450 core\n"
"in vec4 vPosition;"
"in vec3 vNormal;"//
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"};"
"out vec4 fColor;"
"uniform mat4 vMatrix;"
"uniform vec4 vColor;"
"uniform vec4 LightDirection;"//
"void main(void)"
"{"
"gl_Position = vMatrix * vPosition;"
"vec3 worldLight = normalize(LightDirection.xyz);"
"vec3 worldNormal = normalize(mat3(vMatrix) * vNormal);"
"fColor = vColor * (0.5 * dot(worldNormal, worldLight) + 0.5);"// vColor * clamp(dot(vNormal, LightDirection.xyz), 0.0, 1.0);//vColor;//vec4(worldNormal, 1.0); //vec4(worldLight, 1.0);
"}";

char* AnimusDisplayer::fragSrc =
"#version 450 core\n"
"in vec4 fColor;"
"out vec4 color;"
"void main(void)"
"{"
"color = fColor;"
"}";

void AnimusDisplayer::setUp(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<GLuint> &VI, const std::vector<GLuint> &TI, const std::vector<GLuint> &NI)
{
	bool hasTexcoords = false;
	bool hasNormals = false;

	if (VI.size() == TI.size())
	{
		hasTexcoords = true;
	}
	if (VI.size() == NI.size())
	{
		hasNormals = true;
	}

	//reset
	if (vertexPositions != 0)
	{
		delete[]vertexPositions;
		vertexPositions = 0;
	}
	if (vertexTexcoords != 0)
	{
		delete[]vertexTexcoords;
		vertexTexcoords = 0;
	}
	if (vertexNormals != 0)
	{
		delete[]vertexNormals;
		vertexNormals = 0;
	}
	if (vertexIndices != 0)
	{
		delete[]vertexIndices;
		vertexIndices = 0;
	}

	vertexCount = VI.size();
	vertexIndexCount = VI.size();

	vertexPositions = new glm::vec4[vertexCount];
	vertexIndices = new GLuint[vertexIndexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertexPositions[i] = V[VI[i]];
		vertexIndices[i] = i;
	}

	if (hasTexcoords)
	{
		vertexTexcoords = new glm::vec2[vertexCount];
		for (int i = 0; i < vertexCount; i++)
		{
			vertexTexcoords[i] = T[TI[i]];
		}
	}
	if (hasNormals)
	{
		vertexNormals = new glm::vec3[vertexCount];
		for (int i = 0; i < vertexCount; i++)
		{
			vertexNormals[i] = N[NI[i]];
		}
	}

	//reduce same vertex(same vertex means sharing same position, texcoord and normal)
	//
	//
	//
	//
	//
	//
}

void AnimusDisplayer::init()
{
	glEnable(GL_DEPTH_TEST);

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertSrc, 0);
	glCompileShader(vert);

	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragSrc, 0);
	glCompileShader(frag);

	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glUseProgram(program);


	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!

	GLint color = glGetUniformLocation(program, "vColor");
	glUniform4fv(color, 1, &vertexColor[0]);//second parameter indicates the number of targeted vector, not the number of components in your vector!!!!!!

	GLint lightDirection = glGetUniformLocation(program, "LightDirection");//
	glUniform4fv(lightDirection, 1, &LightDirection[0]);//second parameter indicates the number of targeted vector, not the number of components in your vector!!!!!!

	GLint position = glGetAttribLocation(program, "vPosition");

	GLint normal = glGetAttribLocation(program, "vNormal");//


	glGenBuffers(1, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4 + sizeof(GLfloat) * vertexCount * 3, 0, GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexCount * 4, &vertexPositions[0][0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, sizeof(GLfloat) * vertexCount * 3, &vertexNormals[0][0]);//
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, EBOs);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*vertexIndexCount, vertexIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glGenVertexArrays(1, VAOs);

	glBindVertexArray(VAOs[0]);

	glVertexAttribFormat(position, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(position, 0);//set binding index
	glEnableVertexAttribArray(position);
	glBindVertexBuffer(0, VBOs[0], 0, 4 * sizeof(GLfloat));//use binding index

	glVertexAttribFormat(normal, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(normal, 1);//set binding index
	glEnableVertexAttribArray(normal);
	glBindVertexBuffer(1, VBOs[0], sizeof(GLfloat) * vertexCount * 4, 3 * sizeof(GLfloat));//use binding index

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);

	glBindVertexArray(0);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void AnimusDisplayer::renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VAOs[0]);
	glDrawElements(GL_TRIANGLES, vertexIndexCount, GL_UNSIGNED_INT, (void*)0);//modified pipeline does not accept GL_QUADS as a parameter
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
	glDeleteProgram(program);
}