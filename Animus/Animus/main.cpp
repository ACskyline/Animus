////////////////////////////////////////////
//This program is the first step to an anim
//ation editor. My current work is to build 
//my own interface to load .obj and .fbx f-
//iles. The index type used in this program 
//is GLuint.
////////////////////////////////////////////
#include "AnimusDisplayer.h"
#include "AnimusLoader.h"

int main(int argc, char** argv)
{
	AnimusLoader::loadObj("monkey_triangle.obj");
	AnimusDisplayer::setUp(AnimusLoader::V, AnimusLoader::T, AnimusLoader::N, AnimusLoader::VI, AnimusLoader::TI, AnimusLoader::NI);
	AnimusDisplayer::mainLoop(argc, argv);

	return 0;
}