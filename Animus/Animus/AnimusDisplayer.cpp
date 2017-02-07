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

glm::vec4 AnimusDisplayer::LightDirection = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);//from vertex to the light

//all degrees are in radius by default in this version of glm
glm::mat4 AnimusDisplayer::vertexMatrix = glm::perspective<float>(60.0f/180.0f*3.14f, (float)AnimusDisplayer::windowSizeX / (float)AnimusDisplayer::windowSizeY, 0.1f, 100.0f) * glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, -5.0)) * glm::rotate(glm::mat4(), 30.0f/180.0f * 3.14f, glm::vec3(1.0, 0.0, 0.0));

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

//return -1 if not exist, return index if exist
int AnimusDisplayer::vertexExist(glm::vec4 *vPosArray, glm::vec2 *vTexArray, glm::vec3 *vNormArray, glm::vec4 vPos, glm::vec2 vTex, glm::vec3 vNorm, int size)
{
	int result = -1;
	for (int i = 0; i < size; i++)
	{
		if (vPosArray[i] == vPos)
		{
			if (vTexArray[i] == vTex)
			{
				if (vNormArray[i] == vNorm)
				{
					result = i;
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}
	}
	return result;
}

void AnimusDisplayer::setUp(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<AnimusIndex> &AI)
{
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

	vertexIndexCount =  AI.size();

	vertexIndices = new GLuint[vertexIndexCount];
	vertexPositions = new glm::vec4[vertexIndexCount];
	vertexTexcoords = new glm::vec2[vertexIndexCount];
	vertexNormals = new glm::vec3[vertexIndexCount];

	vertexCount = 0;

	for (int i = 0; i < vertexIndexCount; i++)
	{
		// if a vertex does not have a VI, TI or NI, it will be set to (1, 1, 1, 1), (1, 1, 1) or (1, 1)
		glm::vec4 newPosition = AI[i].VI >= 0 && AI[i].VI < V.size() ? V[AI[i].VI] : glm::vec4(1, 1, 1, 1);
		glm::vec2 newTexcoord = AI[i].TI >= 0 && AI[i].TI < T.size() ? T[AI[i].TI] : glm::vec2(1, 1);
		glm::vec3 newNormal = AI[i].NI >= 0 && AI[i].NI < N.size() ? N[AI[i].NI] : glm::vec3(1, 1, 1);

		//reduce same vertex(same vertex means sharing same position, texcoord and normal)
		int result = vertexExist(vertexPositions, vertexTexcoords, vertexNormals, newPosition, newTexcoord, newNormal, vertexCount);//check if the new vertex already exist

		if (result >= 0)//exist
		{
			//set the index
			vertexIndices[i] = result;
		}
		else//not exist
		{
			//add the new vertex and set the index
			vertexPositions[vertexCount] = newPosition;
			vertexTexcoords[vertexCount] = newTexcoord;
			vertexNormals[vertexCount] = newNormal;
			vertexIndices[i] = vertexCount;
			vertexCount++;
		}
		
	}
	glm::vec4 *vertexPositionsTemp = new glm::vec4[vertexCount];
	glm::vec2 *vertexTexcoordsTemp = new glm::vec2[vertexCount];
	glm::vec3 *vertexNormalsTemp = new glm::vec3[vertexCount];

	for (int i = 0; i < vertexCount; i++)
	{
		vertexPositionsTemp[i] = vertexPositions[i];
		vertexTexcoordsTemp[i] = vertexTexcoords[i];
		vertexNormalsTemp[i] = vertexNormals[i];
	}

	delete[] vertexPositions;
	delete[] vertexTexcoords;
	delete[] vertexNormals;

	vertexPositions = vertexPositionsTemp;
	vertexTexcoords = vertexTexcoordsTemp;
	vertexNormals = vertexNormalsTemp;

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