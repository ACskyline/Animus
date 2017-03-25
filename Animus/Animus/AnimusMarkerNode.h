#pragma once
#include "AnimusNode.h"
#include <cstdio>

enum AnimusMarkerNodeType
{
	Axis, Grid, AnimusMarkerNodeTypeCount
};

class AnimusMarkerNode
	: public AnimusNode
{
private:
	void glSetUpMarkerMesh();
	void glSetUpMarkerBuffer();
	void glSetUpMarkerShaders();
	void glSetUpMarkerUniform(const glm::mat4 &vertexMatrix);
	void glSetUpMarkerAttrib(GLint _position, GLint _color);

public:
	AnimusMarkerNodeType markerNodeType;
	GLuint VAOs[1];
	GLuint VBOs[1];
	glm::vec4 *vertexPositions;
	glm::vec4 *vertexColors;
	int vertexCount;
	GLuint program;
	GLuint vert;
	GLuint frag;
	GLint position;
	GLint color;
	char* vertSrc;
	char* fragSrc;

	int gridLineInterval;
	int gridLineCount;
	glm::vec4 gridColor;
	int axisLength;
	glm::vec4 axisColorX;
	glm::vec4 axisColorY;
	glm::vec4 axisColorZ;


	void glSetUpMarker(const glm::mat4 &vertexMatrix);
	void glUpdateMarkerMatrix(const glm::mat4 &vertexMatrix);

	AnimusMarkerNode(AnimusMarkerNodeType _markerNodeType);
	~AnimusMarkerNode();
};

