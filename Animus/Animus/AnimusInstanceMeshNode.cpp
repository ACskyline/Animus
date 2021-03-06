#include "AnimusInstanceMeshNode.h"

AnimusInstanceMeshNode::AnimusInstanceMeshNode()
{
	setNodeType(AnimusNodeType::InstanceMesh);
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

	instanceTransformMatrices = 0;
	instanceFrame = 0;
	instanceAnim = 0;
	instanceCount = 0;

	radius = 500;
	center = glm::vec3(0.0f, 0.0f, 0.0f);

	centerControlState = AnimusInstanceCenterControlState::Idle;

	srand(unsigned(time(0)));
}

AnimusInstanceMeshNode::~AnimusInstanceMeshNode()
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

	if (instanceTransformMatrices != 0)
	{
		delete[] instanceTransformMatrices;
	}

	if (instanceFrame != 0)
	{
		delete[] instanceFrame;
	}

	if (instanceAnim != 0)
	{
		delete[] instanceAnim;
	}
}

//return -1 if not exist, return index if exist
int AnimusInstanceMeshNode::vertexExistInBuffer(glm::vec4 *vPosArray, glm::vec2 *vTexArray, glm::vec3 *vNormArray, glm::vec4 vPos, glm::vec2 vTex, glm::vec3 vNorm, int size)
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
int AnimusInstanceMeshNode::vertexExistInVector(glm::vec4 vertex, const std::vector<glm::vec4> &_V)
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
int AnimusInstanceMeshNode::normalExistInVector(glm::vec3 normal, const std::vector<glm::vec3> &_N)
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
int AnimusInstanceMeshNode::texcoordExistInVector(glm::vec2 texcoord, const std::vector<glm::vec2> &_T)
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
void AnimusInstanceMeshNode::parseObjFace(std::stringstream &ss, std::vector<AnimusIndex> &aIndices)
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

void AnimusInstanceMeshNode::parseFbxMesh(FbxNode* pNode, glm::mat4 parentSRT)
{
	FbxDouble3 lT = pNode->LclTranslation.Get();
	FbxDouble3 lR = pNode->LclRotation.Get();
	FbxDouble3 lS = pNode->LclScaling.Get();

	glm::mat4 vertexSRT = parentSRT
		* glm::translate(glm::mat4(), glm::vec3(lT[0], lT[1], lT[2]))
		* glm::rotate(glm::mat4(), (float)lR[2] / 180.0f * 3.14f, glm::vec3(0.0, 0.0, 1.0))
		* glm::rotate(glm::mat4(), (float)lR[1] / 180.0f * 3.14f, glm::vec3(0.0, 1.0, 0.0))
		* glm::rotate(glm::mat4(), (float)lR[0] / 180.0f * 3.14f, glm::vec3(1.0, 0.0, 0.0))
		* glm::scale(glm::mat4(), glm::vec3(lS[0], lS[1], lS[2]));

	glm::mat4 normalSRT = glm::transpose(glm::inverse(vertexSRT));

	for (int i = 0; i < pNode->GetNodeAttributeCount(); ++i)//every attribute in a node
	{
		if (pNode->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * lMesh = pNode->GetMesh();
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

					///////////////////////////////////////////////////////////////////////////////
					//CI added to match vertices to control points so that animation can be updated
					int ci = lMesh->GetPolygonVertex(j, p);
					fbxVertex = lMesh->GetControlPointAt(ci);//get position
					AItemp.CI = ci;
					///////////////////////////////////////////////////////////////////////////////

					glm::vec3 normal(fbxNormal[0], fbxNormal[1], fbxNormal[2]);//add normal
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

					glm::vec2 texcoord(fbxTexcoord[0], fbxTexcoord[1]);//add texcoord
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

					glm::vec4 vertex(fbxVertex[0], fbxVertex[1], fbxVertex[2], 1);//add position//shiiiiiiiiiiit, fbxsdk leave this w composition as zero
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

					if (p >= 3)//break polygon into mutiple triangles
					{
						AI.push_back(AI[AI.size() - 3]);
						AI.push_back(AI[AI.size() - 2]);//it should be -1, however, since we have already push back a new value, it is -2
					}

					AI.push_back(AItemp);//add index

				}
			}


		}
	}

	// Recursively parse the children.
	for (int c = 0; c < pNode->GetChildCount(); ++c)
		parseFbxMesh(pNode->GetChild(c), vertexSRT);
}

void AnimusInstanceMeshNode::parseFbxMeshGlobalSRT(FbxNode* pNode)
{
	FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);
	FbxAMatrix geometryTransformIT = geometryTransform.Inverse().Transpose();

	for (int i = 0; i < pNode->GetNodeAttributeCount(); ++i)//every attribute in a node
	{
		if (pNode->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * lMesh = pNode->GetMesh();
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

					///////////////////////////////////////////////////////////////////////////////
					//CI added to match vertices to control points so that animation can be updated
					int ci = lMesh->GetPolygonVertex(j, p);
					fbxVertex = lMesh->GetControlPointAt(ci);//get position
					AItemp.CI = ci;
					///////////////////////////////////////////////////////////////////////////////

					fbxNormal = geometryTransformIT.MultT(fbxNormal);
					glm::vec3 normal(fbxNormal[0], fbxNormal[1], fbxNormal[2]);//add normal
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

					glm::vec2 texcoord(fbxTexcoord[0], fbxTexcoord[1]);//add texcoord
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

					fbxVertex = geometryTransform.MultT(fbxVertex);
					glm::vec4 vertex(fbxVertex[0], fbxVertex[1], fbxVertex[2], 1);//add position//shiiiiiiiiiiit, fbxsdk leave this w composition as zero
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

					if (p >= 3)//break polygon into mutiple triangles
					{
						AI.push_back(AI[AI.size() - 3]);
						AI.push_back(AI[AI.size() - 2]);//it should be -1, however, since we have already push back a new value, it is -2
					}

					AI.push_back(AItemp);//add index

				}
			}


		}
	}

	// Recursively parse the children.
	for (int c = 0; c < pNode->GetChildCount(); ++c)
		parseFbxMeshGlobalSRT(pNode->GetChild(c));
}

void AnimusInstanceMeshNode::parseFbxMeshNoSRT(FbxNode* pNode)
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

					///////////////////////////////////////////////////////////////////////////////
					//CI added to match vertices to control points so that animation can be updated
					int ci = lMesh->GetPolygonVertex(j, p);
					fbxVertex = lMesh->GetControlPointAt(ci);//get position
					AItemp.CI = ci;
					///////////////////////////////////////////////////////////////////////////////

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
		parseFbxMeshNoSRT(pNode->GetChild(c));
}

void AnimusInstanceMeshNode::parseFbxMeshAll(FbxNode* pNode, glm::mat4 parentSRT)
{
	FbxVector4 gT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 gR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 gS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	FbxAMatrix geometryTransform = FbxAMatrix(gT, gR, gS);
	FbxAMatrix geometryTransformIT = geometryTransform.Inverse().Transpose();

	FbxDouble3 lT = pNode->LclTranslation.Get();
	FbxDouble3 lR = pNode->LclRotation.Get();
	FbxDouble3 lS = pNode->LclScaling.Get();

	glm::mat4 vertexSRT = parentSRT
		* glm::translate(glm::mat4(), glm::vec3(lT[0], lT[1], lT[2]))
		* glm::rotate(glm::mat4(), (float)lR[2] / 180.0f * 3.14f, glm::vec3(0.0, 0.0, 1.0))
		* glm::rotate(glm::mat4(), (float)lR[1] / 180.0f * 3.14f, glm::vec3(0.0, 1.0, 0.0))
		* glm::rotate(glm::mat4(), (float)lR[0] / 180.0f * 3.14f, glm::vec3(1.0, 0.0, 0.0))
		* glm::scale(glm::mat4(), glm::vec3(lS[0], lS[1], lS[2]));

	glm::mat4 normalSRT = glm::transpose(glm::inverse(vertexSRT));

	for (int i = 0; i < pNode->GetNodeAttributeCount(); ++i)//every attribute in a node
	{
		if (pNode->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh * lMesh = pNode->GetMesh();
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

					///////////////////////////////////////////////////////////////////////////////
					//CI added to match vertices to control points so that animation can be updated
					int ci = lMesh->GetPolygonVertex(j, p);
					fbxVertex = lMesh->GetControlPointAt(ci);//get position
					AItemp.CI = ci;
					///////////////////////////////////////////////////////////////////////////////

					fbxNormal = geometryTransformIT.MultT(fbxNormal);
					glm::vec3 normal(fbxNormal[0], fbxNormal[1], fbxNormal[2]);//add normal
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

					glm::vec2 texcoord(fbxTexcoord[0], fbxTexcoord[1]);//add texcoord
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

					fbxVertex = geometryTransform.MultT(fbxVertex);
					glm::vec4 vertex(fbxVertex[0], fbxVertex[1], fbxVertex[2], 1);//add position//shiiiiiiiiiiit, fbxsdk leave this w composition as zero
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

					if (p >= 3)//break polygon into mutiple triangles
					{
						AI.push_back(AI[AI.size() - 3]);
						AI.push_back(AI[AI.size() - 2]);//it should be -1, however, since we have already push back a new value, it is -2
					}

					AI.push_back(AItemp);//add index

				}
			}


		}
	}

	// Recursively parse the children.
	for (int c = 0; c < pNode->GetChildCount(); ++c)
		parseFbxMesh(pNode->GetChild(c), vertexSRT);
}

void AnimusInstanceMeshNode::printALL(const std::vector<glm::vec4> &V, const std::vector<glm::vec2> &T, const std::vector<glm::vec3> &N, const std::vector<AnimusIndex> &AI)
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
int AnimusInstanceMeshNode::loadObjMesh(char* fileName)
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

int AnimusInstanceMeshNode::loadFbxMesh(char* fileName)
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
			parseFbxMesh(lRootNode->GetChild(i));
		}
	}

	lSdkManager->Destroy();

	return 0;
}

int AnimusInstanceMeshNode::loadFbxMeshGlobalSRT(char* fileName)
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
			parseFbxMeshGlobalSRT(lRootNode->GetChild(i));
		}
	}

	lSdkManager->Destroy();

	return 0;
}

int AnimusInstanceMeshNode::loadFbxMeshNoSRT(char* fileName)
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
			parseFbxMeshNoSRT(lRootNode->GetChild(i));
		}
	}

	lSdkManager->Destroy();

	return 0;
}

int AnimusInstanceMeshNode::loadFbxMeshAll(char* fileName)
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
			parseFbxMeshAll(lRootNode->GetChild(i));
		}
	}

	lSdkManager->Destroy();

	return 0;
}

void AnimusInstanceMeshNode::glResetAll(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &stride, AnimusAnimationNode *lAnim)
{
	glSetUpMesh();
	glSetUpInstances(_instanceCount, translation, stride);
	glSetUpBuffers(lAnim);
	glSetUpAttribs(position, normal, texcoord, boneW, boneI, vMatrix, frame, anim);
}

void AnimusInstanceMeshNode::glResetInstances(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &stride)
{
	//Don't need to reset mesh
	glSetUpInstances(_instanceCount, translation, stride);
	glResetInstanceBuffer();//Don't need to reset other buffers
	glResetInstanceAttribs();//Don't need to reset other Attribs
}

void AnimusInstanceMeshNode::glSetUp(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &stride, AnimusAnimationNode *lAnim, GLint position, GLint normal, GLint texcoord, GLint boneW, GLint boneI, GLint vMatrix, GLint frame, GLint anim)
{
	glSetUpMesh();
	glSetUpInstances(_instanceCount, translation, stride);
	glSetUpBuffers(lAnim);
	glSetUpAttribs(position, normal, texcoord, boneW, boneI, vMatrix, frame, anim);
}

void AnimusInstanceMeshNode::glSetUp(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &strideX, const glm::vec3 &strideY, AnimusAnimationNode *lAnim, GLint position, GLint normal, GLint texcoord, GLint boneW, GLint boneI, GLint vMatrix, GLint frame, GLint anim)
{
	glSetUpMesh();
	glSetUpInstances(_instanceCount, translation, strideX, strideY);
	glSetUpBuffers(lAnim);
	glSetUpAttribs(position, normal, texcoord, boneW, boneI, vMatrix, frame, anim);
}

void AnimusInstanceMeshNode::glSetUpInstances(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &strideX, const glm::vec3 &strideY)
{
	instanceCount = _instanceCount;
	instanceVector.clear();//
	instanceVector.resize(instanceCount);
	if (instanceFrame != 0)
	{
		delete[] instanceFrame;
	}
	if (instanceAnim != 0)
	{
		delete[] instanceAnim;
	}
	if (instanceTransformMatrices != 0)
	{
		delete[] instanceTransformMatrices;
	}
	instanceFrame = new int[instanceCount];
	instanceAnim = new int[instanceCount];
	instanceTransformMatrices = new glm::mat4[instanceCount];
	int sqrt = glm::sqrt(_instanceCount);

	for (int i = 0; i < instanceCount; ++i)
	{
		instanceFrame[i] = 0;
		instanceAnim[i] = static_cast<int>(AnimusInstanceState::Walk);
		instanceVector[i].position = translation + (float)(i/sqrt)*strideX + (float)(i%sqrt)*strideY;
	}
}

void AnimusInstanceMeshNode::glSetUpInstances(int _instanceCount, const glm::vec3 &translation, const glm::vec3 &stride)
{
	instanceCount = _instanceCount;
	instanceVector.clear();//
	instanceVector.resize(instanceCount);
	if (instanceFrame != 0)
	{
		delete [] instanceFrame;
	}
	if (instanceAnim != 0)
	{
		delete [] instanceAnim;
	}
	if (instanceTransformMatrices != 0)
	{
		delete[] instanceTransformMatrices;
	}
	instanceFrame = new int[instanceCount];
	instanceAnim = new int[instanceCount];
	instanceTransformMatrices = new glm::mat4[instanceCount];
	int sqrt = glm::sqrt(_instanceCount);
	
	glm::vec3 strideNew = glm::length(stride) * glm::cross(glm::normalize(stride), glm::vec3(0, 1, 0));
	for (int i = 0; i < instanceCount; ++i)
	{
		instanceFrame[i] = 0;
		instanceAnim[i] = static_cast<int>(AnimusInstanceState::Walk);
		instanceVector[i].position = translation + (float)(i / sqrt)*stride + (float)(i%sqrt)*strideNew;
	}
}

//load first, then set up after glutInit
void AnimusInstanceMeshNode::glSetUpMesh()
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

		////reduce same vertex(same vertex means sharing same position, texcoord and normal)
		//int result = vertexExistInBuffer(vertexPositions, vertexTexcoords, vertexNormals, newPosition, newTexcoord, newNormal, vertexCount);//check if the new vertex already exist

		//if (result >= 0)//exist
		//{
		//	//set the index
		//	vertexIndices[i] = result;
		//}
		//else//does not exist
		//{
		//	//add the new vertex and set the index
		//	vertexPositions[vertexCount] = newPosition;
		//	vertexTexcoords[vertexCount] = newTexcoord;
		//	vertexNormals[vertexCount] = newNormal;
		//	vertexIndices[i] = vertexCount;
		//	vertexCount++;
		//}

		///////////////////////////////////////////////
		//No Check
		vertexPositions[vertexCount] = newPosition;
		vertexTexcoords[vertexCount] = newTexcoord;
		vertexNormals[vertexCount] = newNormal;
		vertexIndices[i] = vertexCount;
		vertexCount++;
		///////////////////////////////////////////////

	}
	//glm::vec4 *vertexPositionsTemp = new glm::vec4[vertexCount];
	//glm::vec2 *vertexTexcoordsTemp = new glm::vec2[vertexCount];
	//glm::vec3 *vertexNormalsTemp = new glm::vec3[vertexCount];

	//for (int i = 0; i < vertexCount; i++)
	//{
	//	vertexPositionsTemp[i] = vertexPositions[i];
	//	vertexTexcoordsTemp[i] = vertexTexcoords[i];
	//	vertexNormalsTemp[i] = vertexNormals[i];
	//}

	//delete[] vertexPositions;
	//delete[] vertexTexcoords;
	//delete[] vertexNormals;

	//vertexPositions = vertexPositionsTemp;
	//vertexTexcoords = vertexTexcoordsTemp;
	//vertexNormals = vertexNormalsTemp;

}

//after glSetUpMesh
void AnimusInstanceMeshNode::glSetUpBuffers(AnimusAnimationNode * lAnim)
{
	glGenBuffers(3, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertexCount * 4 +
		sizeof(GLfloat) * vertexCount * 3 +
		sizeof(GLfloat) * vertexCount * 2,
		0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexCount * 4, &(vertexPositions[0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, sizeof(GLfloat) * vertexCount * 3, &(vertexNormals[0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4 + sizeof(GLfloat) * vertexCount * 3, sizeof(GLfloat) * vertexCount * 2, &(vertexTexcoords[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//instanced attribs are in a different buffer now
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * instanceCount * 16 +
		sizeof(GLint) * instanceCount * 1 +
		sizeof(GLint) * instanceCount * 1,
		0, GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * instanceCount * 16, &(instanceTransformMatrices[0][0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceCount * 16, sizeof(GLint) * instanceCount * 1, &(instanceFrame[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceCount * 16 + sizeof(GLint) * instanceCount * 1, sizeof(GLint) * instanceCount * 1, &(instanceAnim[0]));//
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	////////////////////////////
	//animation-related attribs are in a different buffer now
	glm::vec4* boneWeight = new glm::vec4[vertexCount];
	glm::ivec4* boneIndex = new glm::ivec4[vertexCount];

	for (int i = 0; i < vertexCount; ++i)
	{
		int controlIndex = AI[i].CI;
		int boneI[4];
		float boneW[4];
			
		int size = lAnim->controlPoints[controlIndex].weights.size();
		for (int j = 0; j < 4; ++j)
		{
			boneW[j] = j < size ? lAnim->controlPoints[controlIndex].weights[j].weight : 0.0f;
			boneI[j] = j < size ? lAnim->controlPoints[controlIndex].weights[j].boneIndex : 0;
		}

		boneWeight[i] = glm::vec4(boneW[0], boneW[1], boneW[2], boneW[3]);
		boneIndex[i] = glm::ivec4(boneI[0], boneI[1], boneI[2], boneI[3]);

		//printf("%f,%f,%f,%f\n%d,%d,%d,%d\n", boneW[0], boneW[1], boneW[2], boneW[3], boneI[0], boneI[1], boneI[2], boneI[3]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * vertexCount * 4 +
		sizeof(GLint) * vertexCount * 4,
		0, GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexCount * 4, &(boneWeight[0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, sizeof(GLint) * vertexCount * 4, &(boneIndex[0][0]));//
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] boneWeight;
	delete[] boneIndex;
	/////////////////////////////

	glGenBuffers(1, EBOs);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*vertexIndexCount, vertexIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//after glSetUpBuffers
void AnimusInstanceMeshNode::glSetUpAttribs(GLint _position, GLint _normal, GLint _texcoord, GLint _boneW, GLint _boneI, GLint _vMatrix, GLint _frame, GLint _anim)
{
	int bindingIndex = 0;
	int offset = 0;

	position = _position;
	normal = _normal;
	texcoord = _texcoord;
	boneW = _boneW;
	boneI = _boneI;

	vMatrix = _vMatrix;
	frame = _frame;
	anim = _anim;

	glGenVertexArrays(1, VAOs);

	glBindVertexArray(VAOs[0]);

	glVertexAttribFormat(position, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(position, bindingIndex);//set binding index
	glEnableVertexAttribArray(position);
	glBindVertexBuffer(bindingIndex, VBOs[0], offset, 4 * sizeof(GLfloat));//use binding index
	bindingIndex++;
	offset += sizeof(GLfloat) * vertexCount * 4;

	glVertexAttribFormat(normal, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(normal, bindingIndex);//set binding index
	glEnableVertexAttribArray(normal);
	glBindVertexBuffer(bindingIndex, VBOs[0], offset, 3 * sizeof(GLfloat));//use binding index
	bindingIndex++;
	offset += sizeof(GLfloat) * vertexCount * 3;

	glVertexAttribFormat(texcoord, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(texcoord, bindingIndex);//set binding index
	glEnableVertexAttribArray(texcoord);
	glBindVertexBuffer(bindingIndex, VBOs[0], offset, 2 * sizeof(GLfloat));//use binding index
	bindingIndex++;
	offset = 0;

	//instanced attribs are in a different buffer now
	for (int i = 0; i < 4; ++i)
	{
		glVertexAttribFormat(vMatrix + i, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(vMatrix + i, i + bindingIndex);//set binding index
		glEnableVertexAttribArray(vMatrix + i);
		glBindVertexBuffer(i + bindingIndex, VBOs[1], offset + sizeof(GLfloat) * i * 4, 16 * sizeof(GLfloat));//use binding index
		glVertexAttribDivisor(vMatrix + i, 1);//marked as instanced
	}
	bindingIndex += 4;
	offset += sizeof(GLfloat) * instanceCount * 16;

	glVertexAttribIFormat(frame, 1, GL_INT, 0);
	glVertexAttribBinding(frame, bindingIndex);//set binding index
	glEnableVertexAttribArray(frame);
	glBindVertexBuffer(bindingIndex, VBOs[1], offset, 1 * sizeof(GLint));//use binding index
	glVertexAttribDivisor(frame, 1);//marked as instanced
	bindingIndex++;
	offset += sizeof(GLint) * instanceCount * 1;

	glVertexAttribIFormat(anim, 1, GL_INT, 0);
	glVertexAttribBinding(anim, bindingIndex);//set binding index
	glEnableVertexAttribArray(anim);
	glBindVertexBuffer(bindingIndex, VBOs[1], offset, 1 * sizeof(GLint));//use binding index
	glVertexAttribDivisor(anim, 1);//marked as instanced
	bindingIndex++;
	offset = 0;

	////////////////////////////////////////
	//animation-related attribs
	glVertexAttribFormat(boneW, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(boneW, bindingIndex);//set binding index
	glEnableVertexAttribArray(boneW);
	glBindVertexBuffer(bindingIndex, VBOs[2], offset, 4 * sizeof(GLfloat));//use binding index
	bindingIndex++;
	offset += sizeof(GLfloat) * vertexCount * 4;

	glVertexAttribIFormat(boneI, 4, GL_INT, 0);
	glVertexAttribBinding(boneI, bindingIndex);//set binding index
	glEnableVertexAttribArray(boneI);
	glBindVertexBuffer(bindingIndex, VBOs[2], offset, 4 * sizeof(GLint));//use binding index
	bindingIndex++;
	offset += sizeof(GLint) * vertexCount * 4;
	////////////////////////////////////////

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);

	glBindVertexArray(0);
}

void AnimusInstanceMeshNode::glUpdate(AnimusAnimationNode* AANode0, AnimusAnimationNode *AANode1)//AANode0 is for walk, AANode1 is for idle
{
	glUpdateInstances(AANode0->length, AANode1->length);
	glUpdateInstanceBuffer();
}

void AnimusInstanceMeshNode::glUpdateInstances(int animLength0, int animLength1)//animLength0 is for walk, animLength1 is for idle
{
	for (int i = 0; i < instanceCount; ++i)
	{
		switch (instanceVector[i].state)
		{
		case AnimusInstanceState::Walk:
			if (instanceVector[i].frameBeforeChange > 0)//continue
			{
				instanceVector[i].frameBeforeChange--;
				instanceFrame[i] = ++instanceFrame[i] % animLength0;
			}
			else//new decision
			{
				glm::vec3 dir = center - instanceVector[i].position;
				float length = glm::length(dir);
				if (length > radius)//out of the circle
				{
					instanceVector[i].frameBeforeChange = FRAME_BEFORE_CHANGE_MULTIPLE * FRAME_BEFORE_CHANGE_FACTOR;
					instanceVector[i].speed = SPEED_MULTIPLE * SPEED_FACTOR;
					instanceVector[i].direction = glm::normalize(dir);
					instanceFrame[i] = ++instanceFrame[i] % animLength0;
				}
				else//in the circle
				{
					int chance = rand() % 2;
					if (chance == 0)//idle
					{
						instanceVector[i].frameBeforeChange = rand() % (FRAME_BEFORE_CHANGE_MULTIPLE_IDLE * FRAME_BEFORE_CHANGE_FACTOR);
						instanceVector[i].state = AnimusInstanceState::Idle;
						instanceAnim[i] = static_cast<int>(AnimusInstanceState::Idle);
						instanceFrame[i] = 0;
					}
					else//no idle, continue
					{
						instanceVector[i].frameBeforeChange = rand() % (FRAME_BEFORE_CHANGE_MULTIPLE * FRAME_BEFORE_CHANGE_FACTOR);
						instanceVector[i].speed = (rand() % SPEED_MULTIPLE + 1) * SPEED_FACTOR;
						instanceVector[i].direction = glm::normalize(glm::vec3(2 * ((float)rand() / (float)RAND_MAX - 0.5), 0.0f, 2 * ((float)rand() / (float)RAND_MAX - 0.5)));
						instanceFrame[i] = ++instanceFrame[i] % animLength0;
					}
				}
			}
			calInstanceTransformMatrices(i);
			break;
		case AnimusInstanceState::Idle:
			if (instanceVector[i].frameBeforeChange > 0)//continue
			{
				instanceVector[i].frameBeforeChange--;
				instanceFrame[i] = ++instanceFrame[i] % animLength1;
			}
			else//new decision
			{
				glm::vec3 dir = center - instanceVector[i].position;
				float length = glm::length(dir);
				if (length > radius)//out of the circle
				{
					instanceVector[i].frameBeforeChange = FRAME_BEFORE_CHANGE_MULTIPLE * FRAME_BEFORE_CHANGE_FACTOR;
					instanceVector[i].speed = SPEED_MULTIPLE * SPEED_FACTOR;
					instanceVector[i].direction = glm::normalize(dir);;
					instanceVector[i].state = AnimusInstanceState::Walk;
					instanceAnim[i] = static_cast<int>(AnimusInstanceState::Walk);
					instanceFrame[i] = 0;
				}
				else//in the circle
				{
					instanceVector[i].frameBeforeChange = rand() % (FRAME_BEFORE_CHANGE_MULTIPLE * FRAME_BEFORE_CHANGE_FACTOR);
					instanceVector[i].speed = (rand() % SPEED_MULTIPLE + 1) * SPEED_FACTOR;
					instanceVector[i].direction = glm::normalize(glm::vec3(2 * ((float)rand() / (float)RAND_MAX - 0.5), 0.0f, 2 * ((float)rand() / (float)RAND_MAX - 0.5)));
					instanceVector[i].state = AnimusInstanceState::Walk;
					instanceAnim[i] = static_cast<int>(AnimusInstanceState::Walk);
					instanceFrame[i] = 0;
				}
			}
			break;
		default:
			break;
		}


	}
}

void AnimusInstanceMeshNode::glUpdateInstanceBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	//glBufferData(GL_ARRAY_BUFFER,
	//	sizeof(GLfloat) * instanceCount * 16 +
	//	sizeof(GLint) * instanceCount * 1 +
	//  sizeof(GLint) * instanceCount * 1,
	//	0, GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * instanceCount * 16, &(instanceTransformMatrices[0][0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceCount * 16, sizeof(GLint) * instanceCount * 1, &(instanceFrame[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceCount * 16 + sizeof(GLint) * instanceCount * 1, sizeof(GLint) * instanceCount * 1, &(instanceAnim[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnimusInstanceMeshNode::calInstanceTransformMatrices(int index)
{
	instanceVector[index].position = instanceVector[index].position + instanceVector[index].direction * instanceVector[index].speed;

	glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 newFront3 = instanceVector[index].direction;
	float radius = glm::acos(glm::dot(front, newFront3));
	glm::vec3 axis = glm::cross(front, newFront3);

	instanceTransformMatrices[index] =
		glm::translate(glm::mat4(), instanceVector[index].position) *
		glm::rotate(glm::mat4(), radius, axis);//transformation is relative to local coordinates, so the order is inverted
}

void AnimusInstanceMeshNode::glResetInstanceBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * instanceCount * 16 +
		sizeof(GLint) * instanceCount * 1 +
		sizeof(GLint) * instanceCount * 1,
		0, GL_STATIC_DRAW);//
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * instanceCount * 16, &(instanceTransformMatrices[0][0][0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceCount * 16, sizeof(GLint) * instanceCount * 1, &(instanceFrame[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * instanceCount * 16 + sizeof(GLint) * instanceCount * 1, sizeof(GLint) * instanceCount * 1, &(instanceAnim[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnimusInstanceMeshNode::glResetInstanceAttribs()
{
	int bindingIndex = 3;
	int offset = 0;

	glBindVertexArray(VAOs[0]);

	for (int i = 0; i < 4; ++i)
	{
		glVertexAttribFormat(vMatrix + i, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexAttribBinding(vMatrix + i, i + bindingIndex);//set binding index
		glEnableVertexAttribArray(vMatrix + i);
		glBindVertexBuffer(i + bindingIndex, VBOs[1], offset + sizeof(GLfloat) * i * 4, 16 * sizeof(GLfloat));//use binding index
		glVertexAttribDivisor(vMatrix + i, 1);//marked as instanced
	}
	bindingIndex += 4;
	offset += sizeof(GLfloat) * instanceCount * 16;

	glVertexAttribIFormat(frame, 1, GL_INT, 0);
	glVertexAttribBinding(frame, bindingIndex);//set binding index
	glEnableVertexAttribArray(frame);
	glBindVertexBuffer(bindingIndex, VBOs[1], offset, 1 * sizeof(GLint));//use binding index
	glVertexAttribDivisor(frame, 1);//marked as instanced
	bindingIndex++;
	offset += sizeof(GLint) * instanceCount * 1;

	glVertexAttribIFormat(anim, 1, GL_INT, 0);
	glVertexAttribBinding(anim, bindingIndex);//set binding index
	glEnableVertexAttribArray(anim);
	glBindVertexBuffer(bindingIndex, VBOs[1], offset, 1 * sizeof(GLint));//use binding index
	glVertexAttribDivisor(anim, 1);//marked as instanced

	glBindVertexArray(0);
}