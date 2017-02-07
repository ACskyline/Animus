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
	//if (AnimusLoader::loadObj("monkey_quad.obj")==0)//faces are presented in both quads and triangles
	//{
	//	AnimusDisplayer::setUp(AnimusLoader::V, AnimusLoader::T, AnimusLoader::N, AnimusLoader::AI);
	//	AnimusDisplayer::mainLoop(argc, argv);
	//}

	if (AnimusLoader::loadFbx("cowboy_bin.fbx")==0)//"cowboy_ascii.fbx" is also supported
	{
		AnimusLoader::printALL();
		AnimusDisplayer::setUp(AnimusLoader::V, AnimusLoader::T, AnimusLoader::N, AnimusLoader::AI);
		AnimusDisplayer::mainLoop(argc, argv);
	}
	return 0;
}