#include "AnimusTransformNode.h"


AnimusTransformNode::AnimusTransformNode()
{
	position = glm::vec3(0);
	//all degrees are in radius
	rotation = glm::vec3(0);
	scale = glm::vec3(1);
	setNodeType(AnimusNodeType::Transform);
	modelMatrix = glm::mat4(1);
	inverseModelMatrix = glm::mat4(1);
}


AnimusTransformNode::~AnimusTransformNode()
{
}

glm::mat4 AnimusTransformNode::calculateModelMatrix()
{
	modelMatrix = glm::translate(glm::mat4(), position)
		* glm::rotate(glm::mat4(), rotation[2], glm::vec3(0, 0, 1))
		* glm::rotate(glm::mat4(), rotation[1], glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(), rotation[0], glm::vec3(1, 0, 0))
		* glm::scale(glm::mat4(), scale);

	return modelMatrix;
}

//use this to calculate camera view matrix
glm::mat4 AnimusTransformNode::calculateInverseModelMatrix()
{
	//ViewMatrix should be the matrix that transform vertice from world coordinates to camera coordinates. 
	//So it should be the inverse of camera's SRT. 
	//Since glm use left-multiplication(i.e. Matrix*Vector) just like opengl does, we need to arrange the order of intended matrix
	//So that the sequence of the multiplication happens from right to left.
	//That is inverse(T * R * S), which is inverse(T * Rz * Ry * Rx * S), 
	//which is inverse(S) * inverse(Rx) * inverse(Ry) * inverse(Rz) * inverse(T),
	//which is 1/S * -Rx * -Ry * -Rz * -T.
	glm::vec3 s = scale;
	glm::vec3 r = rotation;
	glm::vec3 t = position;
	s[0] = 1 / s[0];
	s[1] = 1 / s[1];
	s[2] = 1 / s[2];
	r[0] = -r[0];
	r[1] = -r[1];
	r[2] = -r[2];
	t[0] = -t[0];
	t[1] = -t[1];
	t[2] = -t[2];
	inverseModelMatrix = glm::scale(glm::mat4(), s)
		* glm::rotate(glm::mat4(), r[0], glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(), r[1], glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(), r[2], glm::vec3(0, 0, 1))
		* glm::translate(glm::mat4(), t);

	return inverseModelMatrix;
}

glm::mat4 AnimusTransformNode::calculateViewMatrix()
{
	return calculateInverseModelMatrix();
}