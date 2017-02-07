#include "AnimusLoader.h"

std::vector<glm::vec4> AnimusLoader::V;

std::vector<glm::vec2> AnimusLoader::T;

std::vector<glm::vec3> AnimusLoader::N;

std::vector<AnimusIndex> AnimusLoader::AI;

//return -1 if not exist, return index if exist
int AnimusLoader::vertexExist(glm::vec4 vertex)
{
	int result = -1;
	for (int i = 0; i < V.size(); i++)
	{
		if (V[i] == vertex)
		{
			result = i;
			break;
		}
	}
	return result;
}

//return -1 if not exist, return index if exist
int AnimusLoader::normalExist(glm::vec3 normal)
{
	int result = -1;
	for (int i = 0; i < N.size(); i++)
	{
		if (N[i] == normal)
		{
			result = i;
			break;
		}
	}
	return result;
}

//return -1 if not exist, return index if exist
int AnimusLoader::texcoordExist(glm::vec2 texcoord)
{
	int result = -1;
	for (int i = 0; i < T.size(); i++)
	{
		if (T[i] == texcoord)
		{
			result = i;
			break;
		}
	}
	return result;
}

//default value is -1. vIndices, tIndices and nIndices have same count
void AnimusLoader::parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices)
{
	char discard;
	char peek;
	int count;
	GLuint data;

	//One vertex in one loop
	do
	{
		AnimusIndex AItemp;

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			AItemp.VI = data - 1;//index start at 1 in an .obj file but at 0 in an array
			ss >> discard;
		}
		else
		{
			//push default value
			AItemp.VI = -1;
		}

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			AItemp.TI = data - 1;//index start at 1 in an .obj file but at 0 in an array
			ss >> discard;
		}
		else
		{
			//push default value
			AItemp.TI = -1;
			AItemp.hasTexcoord = false;
		}

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			AItemp.NI = data - 1;//index start at 1 in an .obj file but at 0 in an array
			//no discard here because it is the end for this vertex
		}
		else
		{
			//push default value
			AItemp.NI = -1;
			AItemp.hasNormal = false;
		}

		aIndices.push_back(AItemp);

	} while (!ss.eof());
}

void AnimusLoader::parseFbxNode(FbxNode* pNode)
{
	for (int i = 0; i < pNode->GetNodeAttributeCount(); ++i)
	{
		if (pNode->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * lMesh = pNode->GetMesh();
			int controlCount = lMesh->GetControlPointsCount();
			int polygonCount = lMesh->GetPolygonCount();
			int VIoffset = V.size();
			const char* UVsetname = 0;

			if (FbxGeometryElementUV* lElementUV = lMesh->GetElementUV())
			{
				UVsetname = lElementUV->GetName();
			}

			for (int j = 0; j < polygonCount; j++)
			{
				int polygonSize = lMesh->GetPolygonSize(j);
				for (int p = 0; p < polygonSize; p++)
				{
					fbxsdk::FbxVector4 fbxVertex(0.0, 0.0, 0.0, 1.0);
					fbxsdk::FbxVector4 fbxNormal(1.0, 0.0, 0.0, 0.0);
					fbxsdk::FbxVector2 fbxTexcoord(1.0, 1.0);
					AnimusIndex AItemp;

					bool unmapped = false;
					if (bool hasNormal = lMesh->GetPolygonVertexNormal(j, p, fbxNormal))
					{
						AItemp.hasNormal = true;
					}
					else
					{
						AItemp.hasNormal = false;
					}

					fbxVertex = lMesh->GetControlPointAt(lMesh->GetPolygonVertex(j, p));
					if (UVsetname)
					{
						if (bool hasTexcoord = lMesh->GetPolygonVertexUV(j, p, UVsetname, fbxTexcoord, unmapped)&&!unmapped)
						{
							AItemp.hasTexcoord = true;
						}
						else
						{
							AItemp.hasTexcoord = false;
						}
					}

					glm::vec4 vertex(fbxVertex[0], fbxVertex[1], fbxVertex[2], 1);//shiiiiiiiiiiit, fbxsdk leave this w composition as zero
					glm::vec3 normal(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
					glm::vec2 texcoord(fbxTexcoord[0], fbxTexcoord[1]);

					int vi = vertexExist(vertex);
					if (vi >= 0)
					{
						AItemp.VI = vi;
					}
					else
					{
						AItemp.VI = V.size();
						V.push_back(vertex);
					}
					
					int ni = normalExist(normal);
					if (ni >= 0)
					{
						AItemp.NI = ni;
					}
					else
					{
						AItemp.NI = N.size();
						N.push_back(normal);
					}
					
					int ti = texcoordExist(texcoord);
					if (ti >= 0)
					{
						AItemp.TI = ti;
					}
					else
					{
						AItemp.TI = T.size();
						T.push_back(texcoord);
					}

					if (p >= 3)
					{
						AI.push_back(AI[AI.size() - 3]);
						AI.push_back(AI[AI.size() - 2]);//it should be -1, however, since we have already push back a new value, it is -2
					}

					AI.push_back(AItemp);
					
				}
			}


		}
	}

	// Recursively parse the children.
	for (int c = 0; c < pNode->GetChildCount(); ++c)
		parseFbxNode(pNode->GetChild(c));
}

void AnimusLoader::printALL()
{
	for (int i = 0; i < V.size(); i++)
	{
		std::cout << i << ":V:" << V.at(i).x << "," << V.at(i).y << "," << V.at(i).z << "," << V.at(i).w << std::endl;
	}

	for (int i = 0; i < T.size(); i++)
	{
		std::cout << i << ":T:" << T.at(i).x << "," << T.at(i).y << std::endl;
	}

	for (int i = 0; i < N.size(); i++)
	{
		std::cout << i << ":N:" << N.at(i).x << "," << N.at(i).y << "," << N.at(i).z << std::endl;
	}

	for (int i = 0; i < AI.size(); i++)
	{
		std::cout << i << ":VI:" << AI.at(i).VI << "\t";
		std::cout << "TI:" << AI.at(i).TI << "\t";
		std::cout << "NI:" << AI.at(i).NI << std::endl;
	}

}

//V, T and N do not have same count but VI, TI and NI have same count
int AnimusLoader::loadObj(char* fileName)
{
	std::fstream file;
	file.open(fileName, std::ios::in);
	if (file.is_open())
	{
		std::string str;
		while (std::getline(file, str))
		{
			if (str.substr(0, 2) == "v ")
			{
				std::stringstream ss;
				ss << str.substr(2);
				glm::vec4 v;
				ss >> v.x;
				ss >> v.y;
				ss >> v.z;
				v.w = 1.0f;
				V.push_back(v);
			}
			else if (str.substr(0, 3) == "vt ")
			{
				std::stringstream ss;
				ss << str.substr(3);
				glm::vec2 v;
				ss >> v.x;
				ss >> v.y;
				T.push_back(v);
			}
			else if (str.substr(0, 3) == "vn ")
			{
				std::stringstream ss;
				ss << str.substr(3);
				glm::vec3 v;
				ss >> v.x;
				ss >> v.y;
				ss >> v.z;
				N.push_back(v);
			}
			else if (str.substr(0, 2) == "f ")
			{
				std::stringstream ss;
				ss << str.substr(2);
				std::vector<AnimusIndex> aIndices;

				//Parsing
				parseObjFace(ss, aIndices);

				//Collecting(Reassembling)
				//if there are more than 3 vertices for one face then split it in to several triangles
				for (int i = 0; i < aIndices.size(); i++)
				{
					if (i >= 3)
					{
						AI.push_back(aIndices.at(0));
						AI.push_back(aIndices.at(i - 1));
					}
					AI.push_back(aIndices.at(i));
				}
			}
			else if (str[0] == '#')
			{
				//comment
			}
			else
			{
				//others
			}
		}
	}
	else
	{
		std::cout << "can not open" << std::endl;
		return 1;
	}
	file.close();
	return 0;
}

int AnimusLoader::loadFbx(char* fileName)
{
	FbxManager* lSdkManager = FbxManager::Create();

	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	if (!lImporter->Initialize(fileName, -1, lSdkManager->GetIOSettings())) {
		return 1;
	}

	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	lImporter->Import(lScene);

	lImporter->Destroy();

	FbxNode* lRootNode = lScene->GetRootNode();
	if (lRootNode) {
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
		{
			parseFbxNode(lRootNode->GetChild(i));
		}
	}

	lSdkManager->Destroy();

	return 0;
}