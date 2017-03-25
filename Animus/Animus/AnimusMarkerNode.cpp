#include "AnimusMarkerNode.h"

AnimusMarkerNode::AnimusMarkerNode(AnimusMarkerNodeType _markerNodeType)
{
	setNodeType(AnimusNodeType::Marker);
	markerNodeType = _markerNodeType;
	vertexPositions = 0;
	vertexColors = 0;
	vertexCount = 0;
	vertSrc = "#version 450 core\n"
		"in vec4 vPosition;"
		"in vec4 vColor;"
		"out gl_PerVertex"
		"{"
		"vec4 gl_Position;"
		"};"
		"out vec4 fColor;"
		"uniform mat4 vMatrix;"
		"void main(void)"
		"{"
		"gl_Position = vMatrix * vPosition;"
		"fColor = vColor;"
		"}";
	fragSrc = "#version 450 core\n"
		"in vec4 fColor;"
		"out vec4 color;"
		"void main(void)"
		"{"
		"color = fColor;"
		"}";

	gridLineInterval = 10;
	gridLineCount = 101;
	gridColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	axisLength = 10;
	axisColorX = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	axisColorY = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	axisColorZ = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
}


AnimusMarkerNode::~AnimusMarkerNode()
{
	if (vertexPositions)
	{
		delete[]vertexPositions;
	}
	if (vertexColors)
	{
		delete[]vertexColors;
	}
}

void AnimusMarkerNode::glSetUpMarkerMesh()
{
	float offset = (float)(gridLineCount / 2)*gridLineInterval;
	switch (markerNodeType)
	{
	case AnimusMarkerNodeType::Axis:
		vertexCount = 6;
		vertexPositions = new glm::vec4[vertexCount]{
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			glm::vec4(1.0f * axisLength, 0.0f, 0.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			glm::vec4(0.0f, 1.0f * axisLength, 0.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			glm::vec4(0.0f, 0.0f, 1.0f * axisLength, 1.0f),
		};
		vertexColors = new glm::vec4[vertexCount]{
			axisColorX,
			axisColorX,
			axisColorY,
			axisColorY,
			axisColorZ,
			axisColorZ,
		};
		break;
	case AnimusMarkerNodeType::Grid:
		vertexCount = 4 * gridLineCount;
		vertexPositions = new glm::vec4[vertexCount];
		vertexColors = new glm::vec4[vertexCount];
		for (int i = 0; i < 2 * gridLineCount; ++i)
		{
			if (i < gridLineCount)
			{
				vertexPositions[i * 2] = glm::vec4(-offset + i * gridLineInterval, 0.0f, -offset, 1.0f);
				vertexPositions[i * 2 + 1] = glm::vec4(-offset + i* gridLineInterval, 0.0f, offset, 1.0f);
			}
			else
			{
				vertexPositions[i * 2] = glm::vec4(-offset, 0.0f, -offset + (i - 101)* gridLineInterval, 1.0f);
				vertexPositions[i * 2 + 1] = glm::vec4(offset, 0.0f, -offset + (i - 101)* gridLineInterval, 1.0f);
			}

			vertexColors[i * 2] = gridColor;
			vertexColors[i * 2 + 1] = gridColor;
		}
		break;
	default:
		break;
	}
}

void AnimusMarkerNode::glSetUpMarkerBuffer()
{
	glGenBuffers(1, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4 + sizeof(GLfloat) * vertexCount * 4, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,									sizeof(GLfloat) * vertexCount * 4, &(vertexPositions[0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, sizeof(GLfloat) * vertexCount * 4, &(vertexColors[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnimusMarkerNode::glSetUpMarkerShaders()
{
	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertSrc, 0);
	glCompileShader(vert);

	GLint result;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		printf("vert compile failed\n");
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragSrc, 0);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		printf("frag compile failed\n");
	}

	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	glUseProgram(program);//

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		printf("program link failed\n");
	}
}

void AnimusMarkerNode::glSetUpMarkerUniform(const glm::mat4 &vertexMatrix)
{
	//outside uniforms
	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!
}

void AnimusMarkerNode::glSetUpMarkerAttrib(GLint _position, GLint _color)
{
	position = _position;
	color = _color;

	glGenVertexArrays(1, VAOs);

	glBindVertexArray(VAOs[0]);

	glVertexAttribFormat(position, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(position, 0);//set binding index
	glEnableVertexAttribArray(position);
	glBindVertexBuffer(0, VBOs[0], 0, 4 * sizeof(GLfloat));//use binding index

	glVertexAttribFormat(color, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(color, 1);//set binding index
	glEnableVertexAttribArray(color);
	glBindVertexBuffer(1, VBOs[0], 4 * vertexCount* sizeof(GLfloat), 4 * sizeof(GLfloat));//use binding index

	glBindVertexArray(0);
}

void AnimusMarkerNode::glSetUpMarker(const glm::mat4 &vertexMatrix)
{
	glSetUpMarkerMesh();
	glSetUpMarkerBuffer();
	glSetUpMarkerShaders();
	glSetUpMarkerUniform(vertexMatrix);
	GLint position = glGetAttribLocation(program, "vPosition");
	GLint color = glGetAttribLocation(program, "vColor");
	glSetUpMarkerAttrib(position, color);
}

void AnimusMarkerNode::glUpdateMarkerMatrix(const glm::mat4 &vertexMatrix)
{
	//outside uniforms
	glUseProgram(program);
	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!
}
