#include "AnimusMeshNode.h"


AnimusMeshNode::AnimusMeshNode()
{
	setNodeType(AnimusNodeType::Mesh);
	V = std::vector<glm::vec4>();
	T = std::vector<glm::vec2>();
	N = std::vector<glm::vec3>();
	AI = std::vector<AnimusIndex>();
	vertexPositions = 0;
	vertexTexcoords = 0;
	vertexNormals = 0;
	vertexIndices = 0;
	vertexCount = 0;
	vertexIndexCount = 0;
}


AnimusMeshNode::~AnimusMeshNode()
{
	if (vertexPositions != 0)
	{
		delete[] vertexPositions;
	}

	if (vertexNormals != 0)
	{
		delete[] vertexNormals;
	}

	if (vertexTexcoords != 0)
	{
		delete[] vertexTexcoords;
	}

	if (vertexIndices != 0)
	{
		delete[] vertexIndices;
	}
}


//return -1 if not exist, return index if exist
int AnimusMeshNode::vertexExistInBuffer(glm::vec4 *vPosArray, glm::vec2 *vTexArray, glm::vec3 *vNormArray, glm::vec4 vPos, glm::vec2 vTex, glm::vec3 vNorm, int size)
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

//return -1 if not exist, return index if exist
int AnimusMeshNode::vertexExistInVector(glm::vec4 vertex, const std::vector<glm::vec4> &_V)
{
	int result = -1;
	for (int i = 0; i < _V.size(); i++)
	{
		if (_V[i] == vertex)
		{
			result = i;
			break;
		}
	}
	return result;
}

//return -1 if not exist, return index if exist
int AnimusMeshNode::normalExistInVector(glm::vec3 normal, const std::vector<glm::vec3> &_N)
{
	int result = -1;
	for (int i = 0; i < _N.size(); i++)
	{
		if (_N[i] == normal)
		{
			result = i;
			break;
		}
	}
	return result;
}

//return -1 if not exist, return index if exist
int AnimusMeshNode::texcoordExistInVector(glm::vec2 texcoord, const std::vector<glm::vec2> &_T)
{
	int result = -1;
	for (int i = 0; i < _T.size(); i++)
	{
		if (_T[i] == texcoord)
		{
			result = i;
			break;
		}
	}
	return result;
}


//default value is -1. vIndices, tIndices and nIndices have same count
void AnimusMeshNode::parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices)
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

void AnimusMeshNode::parseFbxNode(FbxNode* pNode, glm::mat4 parentSRT)
{
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	glm::mat4 vertexSRT = parentSRT
		* glm::translate(glm::mat4(), glm::vec3(translation[0], translation[1], translation[2]))
		* glm::rotate(glm::mat4(), (float)rotation[2] / 180.0f * 3.14f, glm::vec3(0.0, 0.0, 1.0))
		* glm::rotate(glm::mat4(), (float)rotation[1] / 180.0f * 3.14f, glm::vec3(0.0, 1.0, 0.0))
		* glm::rotate(glm::mat4(), (float)rotation[0] / 180.0f * 3.14f, glm::vec3(1.0, 0.0, 0.0))
		* glm::scale(glm::mat4(), glm::vec3(scaling[0], scaling[1], scaling[2]));

	glm::mat4 normalSRT = glm::transpose(glm::inverse(vertexSRT));

	for (int i = 0; i < pNode->GetNodeAttributeCount(); ++i)//every attribute in a node
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

			for (int j = 0; j < polygonCount; j++)//every polygon in an eMesh attribute
			{
				int polygonSize = lMesh->GetPolygonSize(j);
				for (int p = 0; p < polygonSize; p++)//every vertex in a polygon
				{
					fbxsdk::FbxVector4 fbxVertex(0.0, 0.0, 0.0, 1.0);
					fbxsdk::FbxVector4 fbxNormal(1.0, 0.0, 0.0, 0.0);
					fbxsdk::FbxVector2 fbxTexcoord(1.0, 1.0);
					AnimusIndex AItemp;

					bool unmapped = false;
					if (bool hasNormal = lMesh->GetPolygonVertexNormal(j, p, fbxNormal))//get normal
					{
						AItemp.hasNormal = true;
					}
					else
					{
						AItemp.hasNormal = false;
					}

					fbxVertex = lMesh->GetControlPointAt(lMesh->GetPolygonVertex(j, p));//get position

					if (UVsetname)
					{
						if (bool hasTexcoord = lMesh->GetPolygonVertexUV(j, p, UVsetname, fbxTexcoord, unmapped) && !unmapped)//get texcoord
						{
							AItemp.hasTexcoord = true;
						}
						else
						{
							AItemp.hasTexcoord = false;
						}
					}

					glm::vec4 vertex(fbxVertex[0], fbxVertex[1], fbxVertex[2], 1);//shiiiiiiiiiiit, fbxsdk leave this w composition as zero
					vertex = vertexSRT * vertex;
					int vi = vertexExistInVector(vertex, V);
					if (vi >= 0)
					{
						AItemp.VI = vi;
					}
					else
					{
						AItemp.VI = V.size();
						V.push_back(vertex);
					}

					glm::vec3 normal(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
					normal = (glm::mat3)normalSRT * normal;
					int ni = normalExistInVector(normal, N);
					if (ni >= 0)
					{
						AItemp.NI = ni;
					}
					else
					{
						AItemp.NI = N.size();
						N.push_back(normal);
					}

					glm::vec2 texcoord(fbxTexcoord[0], fbxTexcoord[1]);
					int ti = texcoordExistInVector(texcoord, T);
					if (ti >= 0)
					{
						AItemp.TI = ti;
					}
					else
					{
						AItemp.TI = T.size();
						T.push_back(texcoord);
					}

					if (p >= 3)//break polygon into mutiple triangles
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
		parseFbxNode(pNode->GetChild(c), vertexSRT);
}

void AnimusMeshNode::parseFbxNodeNoSRT(FbxNode* pNode)
{
	for (int i = 0; i < pNode->GetNodeAttributeCount(); ++i)//every attribute in a node
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

			for (int j = 0; j < polygonCount; j++)//every polygon in an eMesh attribute
			{
				int polygonSize = lMesh->GetPolygonSize(j);
				for (int p = 0; p < polygonSize; p++)//every vertex in a polygon
				{
					fbxsdk::FbxVector4 fbxVertex(0.0, 0.0, 0.0, 1.0);
					fbxsdk::FbxVector4 fbxNormal(1.0, 0.0, 0.0, 0.0);
					fbxsdk::FbxVector2 fbxTexcoord(1.0, 1.0);
					AnimusIndex AItemp;

					bool unmapped = false;
					if (bool hasNormal = lMesh->GetPolygonVertexNormal(j, p, fbxNormal))//get normal
					{
						AItemp.hasNormal = true;
					}
					else
					{
						AItemp.hasNormal = false;
					}

					fbxVertex = lMesh->GetControlPointAt(lMesh->GetPolygonVertex(j, p));//get position

					if (UVsetname)
					{
						if (bool hasTexcoord = lMesh->GetPolygonVertexUV(j, p, UVsetname, fbxTexcoord, unmapped) && !unmapped)//get texcoord
						{
							AItemp.hasTexcoord = true;
						}
						else
						{
							AItemp.hasTexcoord = false;
						}
					}

					glm::vec4 vertex(fbxVertex[0], fbxVertex[1], fbxVertex[2], 1);//shiiiiiiiiiiit, fbxsdk leave this w composition as zero
					int vi = vertexExistInVector(vertex, V);
					if (vi >= 0)
					{
						AItemp.VI = vi;
					}
					else
					{
						AItemp.VI = V.size();
						V.push_back(vertex);
					}

					glm::vec3 normal(fbxNormal[0], fbxNormal[1], fbxNormal[2]);
					int ni = normalExistInVector(normal, N);
					if (ni >= 0)
					{
						AItemp.NI = ni;
					}
					else
					{
						AItemp.NI = N.size();
						N.push_back(normal);
					}

					glm::vec2 texcoord(fbxTexcoord[0], fbxTexcoord[1]);
					int ti = texcoordExistInVector(texcoord, T);
					if (ti >= 0)
					{
						AItemp.TI = ti;
					}
					else
					{
						AItemp.TI = T.size();
						T.push_back(texcoord);
					}

					if (p >= 3)//break polygon into mutiple triangles
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
		parseFbxNodeNoSRT(pNode->GetChild(c));
}

void AnimusMeshNode::printALL(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<AnimusIndex> &AI)
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
int AnimusMeshNode::loadObj(char* fileName)
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

int AnimusMeshNode::loadFbx(char* fileName)
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

int AnimusMeshNode::loadFbxNoSRT(char* fileName)
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
			parseFbxNodeNoSRT(lRootNode->GetChild(i));
		}
	}

	lSdkManager->Destroy();

	return 0;
}

void AnimusMeshNode::glSetUp(GLint position, GLint normal, GLint texcoord)
{
	glSetUpMesh();
	glSetUpBuffers();
	glSetUpAttribs(position, normal, texcoord);
}

//load first, then set up after glutInit
void AnimusMeshNode::glSetUpMesh()
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

	vertexIndexCount = AI.size();

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
		int result = vertexExistInBuffer(vertexPositions, vertexTexcoords, vertexNormals, newPosition, newTexcoord, newNormal, vertexCount);//check if the new vertex already exist

		if (result >= 0)//exist
		{
			//set the index
			vertexIndices[i] = result;
		}
		else//does not exist
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

//after glSetUpMesh
void AnimusMeshNode::glSetUpBuffers()
{
	glGenBuffers(1, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertexCount * 4 +
		sizeof(GLfloat) * vertexCount * 3 +
		sizeof(GLfloat) * vertexCount * 2,
		0, GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexCount * 4, &(vertexPositions[0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, sizeof(GLfloat) * vertexCount * 3, &(vertexNormals[0][0]));//
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4 + sizeof(GLfloat) * vertexCount * 3, sizeof(GLfloat) * vertexCount * 2, &(vertexTexcoords[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glGenBuffers(1, EBOs);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*vertexIndexCount, vertexIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//after glSetUpBuffers
void AnimusMeshNode::glSetUpAttribs(GLint position, GLint normal, GLint texcoord)
{
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

	glVertexAttribFormat(texcoord, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(texcoord, 2);//set binding index
	glEnableVertexAttribArray(texcoord);
	glBindVertexBuffer(2, VBOs[0], sizeof(GLfloat) * vertexCount * 4 + sizeof(GLfloat) * vertexCount * 3, 2 * sizeof(GLfloat));//use binding index

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);

	glBindVertexArray(0);
}