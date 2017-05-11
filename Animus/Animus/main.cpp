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