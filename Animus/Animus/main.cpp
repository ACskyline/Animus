////////////////////////////////////////////
//This program is the first step to an anim
//ation editor. My current work is to build 
//my own interface to load .obj and .fbx f-
//iles. The index type used in this program 
//is GLuint.
////////////////////////////////////////////
#include "AnimusDisplayer.h"

int main(int argc, char** argv)
{
	//if (AnimusLoader::loadObj("monkey_quad.obj")==0)//faces are presented in both quads and triangles
	//{
	//	AnimusDisplayer::setUp(AnimusLoader::V, AnimusLoader::T, AnimusLoader::N, AnimusLoader::AI);
	//	AnimusDisplayer::mainLoop(argc, argv);
	//}

	//if (AnimusLoader::loadFbx("cowboy_bin.fbx", &AnimusDisplayer::lMesh) == 0)//"cowboy_ascii.fbx" is also supported
	//{
	//	//AnimusLoader::printALL();
	//	AnimusDisplayer::lMesh.setUp();
	//	AnimusDisplayer::mainLoop(argc, argv);
	//}

	//if (AnimusDisplayer::lAnim.loadFbxAnimation("cowboy_bin_-zfyu.fbx") != 0)
	//{
	//	printf("fuck\n");
	//}

	//if (AnimusDisplayer::lMesh.loadFbxMesh("cowboy_bin_-zfyu.fbx") != 0)
	//{
	//	return 1;
	//}

	//if (AnimusDisplayer::lMat.loadDdsTex("texture.dds") != 0)
	//{
	//	return 1;
	//}

	if (AnimusDisplayer::lMat.loadDdsTex("texture.dds") != 0)
	{
		return 1;
	}

	if (AnimusDisplayer::lIMat.loadDdsTex("texture.dds") != 0)
	{
		return 1;
	}

	if (AnimusDisplayer::lAnim0.loadFbxAnimation("cowboy_ascii_-zfyu.fbx", 0) != 0)
	{
		printf("fubar0\n");
	}

	if (AnimusDisplayer::lAnim1.loadFbxAnimation("cowboy_ascii_-zfyu.fbx", 1) != 0)
	{
		printf("fubar1\n");
	}

	if (AnimusDisplayer::lMesh.loadFbxMeshNoSRT("cowboy_ascii_-zfyu.fbx") != 0)
	{
		return 1;
	}
	else
	{
		//printf("AnimusIndexCount: %d\nAnimusVertexCount: %d\nAnimusNormalCount: %d\nAnimusTexcoordCount: %d\n", AnimusDisplayer::lMesh.AI.size(), AnimusDisplayer::lMesh.V.size(), AnimusDisplayer::lMesh.N.size(), AnimusDisplayer::lMesh.T.size());
	}

	if (AnimusDisplayer::lIMesh.loadFbxMeshNoSRT("cowboy_ascii_-zfyu.fbx") != 0)
	{
		return 1;
	}
	else
	{
		//printf("AnimusIndexCount: %d\nAnimusVertexCount: %d\nAnimusNormalCount: %d\nAnimusTexcoordCount: %d\n", AnimusDisplayer::lMesh.AI.size(), AnimusDisplayer::lMesh.V.size(), AnimusDisplayer::lMesh.N.size(), AnimusDisplayer::lMesh.T.size());
	}

	AnimusDisplayer::mainLoop(argc, argv);
	
	return 0;
}