#include "AnimusAnimationNode.h"

AnimusAnimationNode::AnimusAnimationNode()
{
	setNodeType(AnimusNodeType::Animation);
	name = "AnimusAnimation";
	vertexPositions = 0;
	vertexCount = 0;
	program = 0;
	vert = 0;
	frag = 0;
	vertSrc = "#version 450 core\n"
		"in vec4 vPosition;"
		"out gl_PerVertex"
		"{"
		"vec4 gl_Position;"
		"};"
		"out vec4 fColor;"
		"uniform mat4 vMatrix;"
		"void main(void)"
		"{"
		"gl_Position = vMatrix * vPosition;"
		"fColor = vec4(1.0, 0.0, 0.0, 1.0);"
		"}";
	fragSrc = "#version 450 core\n"
		"in vec4 fColor;"
		"out vec4 color;"
		"void main(void)"
		"{"
		"color = fColor;"
		"}";
}

AnimusAnimationNode::~AnimusAnimationNode()
{
	if (vertexPositions)
	{
		delete[]vertexPositions;
	}

	for (int i = 0; i < bones.size(); ++i)
	{
		if (bones[i].animation)
		{
			delete[]bones[i].animation;
		}
	}
}

glm::mat4x4 AnimusAnimationNode::FbxAMatrixToGlmMat(FbxAMatrix fMatrix)
{
	glm::mat4x4 result = glm::mat4x4(
		fMatrix.Get(0, 0), fMatrix.Get(0, 1), fMatrix.Get(0, 2), fMatrix.Get(0, 3),
		fMatrix.Get(1, 0), fMatrix.Get(1, 1), fMatrix.Get(1, 2), fMatrix.Get(1, 3),
		fMatrix.Get(2, 0), fMatrix.Get(2, 1), fMatrix.Get(2, 2), fMatrix.Get(2, 3),
		fMatrix.Get(3, 0), fMatrix.Get(3, 1), fMatrix.Get(3, 2), fMatrix.Get(3, 3)
		);
	return result;
}

void AnimusAnimationNode::glSetUpBoneMesh()
{
	if (vertexPositions != 0)
	{
		delete[]vertexPositions;
	}

	vertexCount = 0;
	vertexPositions = new glm::vec4[bones.size() * 2];

	for (int i = 0; i < bones.size(); ++i)
	{
		int pIndex = bones[i].parentBoneIndex;

		bones[i].position = bones[i].linkTransform * glm::vec4(0, 0, 0, 1);//

		if (pIndex >= 0)
		{
			//bones[i].start = bones[pIndex].globalBindposeInverse * glm::vec4(0, 0, 0, 1); //bones[pIndex].nodeTransform * glm::vec4(0, 0, 0, 1);//bones[pIndex].globalBindposeInverse *bones[pIndex].nodeTransform * glm::vec4(0, 0, 0, 1);
			//bones[i].end = bones[i].globalBindposeInverse * glm::vec4(0, 0, 0, 1); //bones[i].globalBindposeInverse*bones[i].nodeTransform * glm::vec4(0, 0, 0, 1);
			
			vertexPositions[i * 2] = bones[pIndex].position;//
			vertexPositions[i * 2 + 1] = bones[i].position;//
		}
		else
		{
			//bones[i].start = bones[i].globalBindposeInverse * glm::vec4(0, 0, 0, 1); //bones[i].globalBindposeInverse *bones[i].nodeTransform * glm::vec4(0, 0, 0, 1);
			//bones[i].end = bones[i].start;//bones[i].globalBindposeInverse  *bones[i].nodeTransform * glm::vec4(0, 0, 0, 1);

			vertexPositions[i * 2] = bones[i].position;//
			vertexPositions[i * 2 + 1] = bones[i].position;//
		}
		//vertexPositions[i*2] = bones[i].start;
		//vertexPositions[i * 2 + 1] = bones[i].end;
		vertexCount += 2;
	}
}

void AnimusAnimationNode::glSetUpBoneBuffer()
{
	glGenBuffers(1, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexCount * 4, &(vertexPositions[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnimusAnimationNode::glSetUpBoneShader()
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

void AnimusAnimationNode::glSetUpBoneUniform(const glm::mat4 &vertexMatrix)
{
	//outside uniforms
	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!
}

void AnimusAnimationNode::glSetUpBoneAttrib(GLint _position)
{
	position = _position;

	glGenVertexArrays(1, VAOs);

	glBindVertexArray(VAOs[0]);

	glVertexAttribFormat(position, 4, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(position, 0);//set binding index
	glEnableVertexAttribArray(position);
	glBindVertexBuffer(0, VBOs[0], 0, 4 * sizeof(GLfloat));//use binding index

	glBindVertexArray(0);
}

void AnimusAnimationNode::glUpdateBoneMesh(int frame)
{
	if (vertexPositions != 0)
	{
		delete[]vertexPositions;
	}

	vertexCount = 0;
	vertexPositions = new glm::vec4[bones.size() * 2];

	for (int i = 0; i < bones.size(); ++i)
	{
		int pIndex = bones[i].parentBoneIndex;
		glm::vec4 startTemp;
		glm::vec4 endTemp;

		bones[i].position = bones[i].animation != 0 && frame >= 0 && frame<length ? bones[i].animation[frame].globalTransform * glm::vec4(0, 0, 0, 1) : bones[i].linkTransform * glm::vec4(0, 0, 0, 1);//
		
		if (pIndex >= 0)
		{
			startTemp = bones[pIndex].animation != 0 && frame >= 0 && frame<length ?  bones[pIndex].animation[frame].globalTransform  * bones[i].start : bones[i].start;
			endTemp = bones[pIndex].animation != 0 && frame >= 0 && frame<length ? bones[pIndex].animation[frame].globalTransform  * bones[i].end : bones[i].end;

			vertexPositions[i * 2] = bones[pIndex].position;//
			vertexPositions[i * 2 + 1] = bones[i].position;//
		}
		else
		{
			startTemp = bones[i].animation != 0 && frame >= 0 && frame<length ? bones[i].animation[frame].globalTransform * bones[i].start : bones[i].start;
			endTemp = startTemp;

			vertexPositions[i * 2] = bones[i].position;//
			vertexPositions[i * 2 + 1] = bones[i].position;//
		}
		//vertexPositions[i * 2] = startTemp;
		//vertexPositions[i * 2 + 1] = endTemp;
		vertexCount += 2;
	}
}

void AnimusAnimationNode::glUpdateBoneBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount * 4, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertexCount * 4, &(vertexPositions[0][0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AnimusAnimationNode::glSetUpBone(const glm::mat4 & vertexMatrix)
{
	glSetUpBoneMesh();
	glSetUpBoneBuffer();
	glSetUpBoneShader();
	glSetUpBoneUniform(vertexMatrix);
	GLint position = glGetAttribLocation(program, "vPosition");
	glSetUpBoneAttrib(position);
}

void AnimusAnimationNode::glUpdateBone(int frame)
{
	glUpdateBoneMesh(frame);
	glUpdateBoneBuffer();
}

void AnimusAnimationNode::glUpdateBoneMatrix(const glm::mat4 &vertexMatrix)
{
	//outside uniforms
	glUseProgram(program);
	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!
}

void AnimusAnimationNode::parseFbxBone(FbxNode* inNode, int parentIndex)
{
	int myIndex = -1;
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		AnimusBone currBone;
		currBone.parentBoneIndex = parentIndex;
		currBone.name = inNode->GetName();
		currBone.node = inNode;
		/////////////////////
		//Node Local Transform
		FbxVector4 lT = inNode->LclTranslation;
		FbxVector4 lR = inNode->LclRotation;
		FbxVector4 lS = inNode->LclScaling;
		
		FbxVector4 gT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 gR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 gS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

		currBone.GeoTransform = FbxAMatrix(gT, gR, gS);
		currBone.geoTransform = FbxAMatrixToGlmMat(currBone.GeoTransform);

		if (parentIndex >= 0)
		{
			currBone.NodeTransform = bones[parentIndex].NodeTransform * FbxAMatrix(lT, lR, lS);//
			currBone.nodeTransform = bones[parentIndex].nodeTransform * FbxAMatrixToGlmMat(FbxAMatrix(lT, lR, lS));//
		}
		else
		{
			currBone.NodeTransform = FbxAMatrix(lT, lR, lS);//
			currBone.nodeTransform = FbxAMatrixToGlmMat(FbxAMatrix(lT, lR, lS));//
		}
		////////////////////
		bones.push_back(currBone);
		myIndex = bones.size() - 1;
	}

	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		parseFbxBone(inNode->GetChild(i), myIndex);
	}
}

void AnimusAnimationNode::parseFbxAnimation(FbxNode* inNode, FbxScene* mFBXScene, int stackNum)
{
	for (int i = 0; i < inNode->GetNodeAttributeCount(); ++i)//every attribute in a node
	{
		if (inNode->GetNodeAttributeByIndex(i)->GetAttributeType() == FbxNodeAttribute::eMesh)//bones in mesh nodes can have animation, but bones in other nodes(e.g. hat_end, legIK.l_end, etc.)
		{
			FbxMesh* currMesh = inNode->GetMesh();
			int numOfDeformers = currMesh->GetDeformerCount();
			FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
			FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
			FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);
			FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);

			controlPoints.resize(currMesh->GetControlPointsCount());

			// A deformer is a FBX thing, which contains some clusters
			// A cluster contains a link, which is basically a joint
			// Normally, there is only one deformer in a mesh
			for (int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
			{
				// There are many types of deformers in Maya,
				// We are using only skins, so we see if this is a skin
				FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
				if (!currSkin)
				{
					continue;
				}

				// Get animation information
				// Now only supports one take
				FbxAnimStack* currAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(stackNum);//
				FbxTime::EMode currEMode = mFBXScene->GetGlobalSettings().GetTimeMode();
				FbxString animStackName = currAnimStack->GetName();
				name = animStackName.Buffer();
				FbxTakeInfo* takeInfo = mFBXScene->GetTakeInfo(animStackName);
				FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
				FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
				length = end.GetFrameCount(currEMode) - start.GetFrameCount(currEMode) + 1;
				int numOfClusters = currSkin->GetClusterCount();

				for (int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
				{
					FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
					std::string currJointName = currCluster->GetLink()->GetName();
					int currJointIndex = FindBoneIndexUsingName(currJointName);
					FbxAMatrix transformMatrix;
					FbxAMatrix transformLinkMatrix;
					FbxAMatrix globalBindposeInverseMatrix;

					currCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
					currCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
					globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

					//////////////////////////////////////////////////////////////////////////////
					//Matrix that transform bones
					bones[currJointIndex].LinkTransform = transformLinkMatrix;
					bones[currJointIndex].linkTransform = FbxAMatrixToGlmMat(transformLinkMatrix);
					//////////////////////////////////////////////////////////////////////////////

					// Update the information in mSkeleton 
					bones[currJointIndex].GlobalBindposeInverse = globalBindposeInverseMatrix;
					bones[currJointIndex].globalBindposeInverse = FbxAMatrixToGlmMat(globalBindposeInverseMatrix);

					// Associate each joint with the control points it affects
					int numOfIndices = currCluster->GetControlPointIndicesCount();
					for (int i = 0; i < numOfIndices; ++i)
					{
						AnimusWeight currBlendingIndexWeightPair;
						currBlendingIndexWeightPair.boneIndex = currJointIndex;
						currBlendingIndexWeightPair.weight = currCluster->GetControlPointWeights()[i];
						controlPoints[currCluster->GetControlPointIndices()[i]].weights.push_back(currBlendingIndexWeightPair);
					}

					//bones[currJointIndex].animation = new AnimusFrame[length];
					//for (int i = 0; i < length; ++i)
					//{
					//	FbxTime currTime;
					//	//currTime.SetFrame(i, currEMode);
					//	currTime.SetTime(0, 0, 0, i, 0, currEMode);
					//	bones[currJointIndex].animation[i].frameNum = i;
					//	FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
					//	bones[currJointIndex].animation[i].GlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);//
					//	bones[currJointIndex].animation[i].globalTransform = FbxAMatrixToGlmMat(bones[currJointIndex].animation[i].GlobalTransform);//
					//}
				}

				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//This is the same block as above, only difference is this includes non-cluster bones, which means all bones have animations instead of only those attached to a mesh.
				//EvaluateGlobalTransform only supports the default take, should fix this later
				for (int bone = 0; bone < bones.size(); ++bone)
				{
					bones[bone].animation = new AnimusFrame[length];
					for (int i = 0; i < length; ++i)
					{
						FbxTime currTime;
						//currTime.SetFrame(i, currEMode);
						currTime.SetTime(0, 0, 0, i, 0, currEMode);
						bones[bone].animation[i].frameNum = i;
						FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
						bones[bone].animation[i].GlobalTransform = currentTransformOffset.Inverse() * bones[bone].node->EvaluateGlobalTransform(currTime);//
						bones[bone].animation[i].globalTransform = FbxAMatrixToGlmMat(bones[bone].animation[i].GlobalTransform);//
					}
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			}


		}
	}


	for (int c = 0; c < inNode->GetChildCount(); ++c)
		parseFbxAnimation(inNode->GetChild(c), mFBXScene, stackNum);

}

int AnimusAnimationNode::FindBoneIndexUsingName(std::string name)
{
	int result = -1;
	for (int i = 0; i < bones.size(); ++i)
	{
		if (bones[i].name == name)
		{
			result = i;
			break;
		}
	}
	return result;
}

int AnimusAnimationNode::loadFbxAnimation(char * fileName, int stackNum)
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
			parseFbxBone(lRootNode->GetChild(i));
		}
	}

	if (lRootNode) {
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
		{
			parseFbxAnimation(lRootNode->GetChild(i), lScene, stackNum);
		}
	}

	lSdkManager->Destroy();

	return 0;
}

void AnimusAnimationNode::printAll(char* filename)
{
	std::fstream fs;
	fs.open(filename, std::ios::out);

	fs << "Animation Name:" << name << std::endl;
	fs << "Animation Time:" << length << std::endl;
	fs << std::endl;

	for (int i = 0; i < controlPoints.size(); ++i)
	{
		fs << "point" << i << ":";
		for (int j = 0; j < controlPoints[i].weights.size(); ++j)
		{
			fs << " bone" << controlPoints[i].weights[j].boneIndex << "--" << controlPoints[i].weights[j].weight;
		}
		fs << std::endl;
	}

	fs << std::endl;

	for (int m = 0; m < bones.size(); ++m)
	{
		fs << "bone" << m << "(" << bones[m].name << ")" << ":" << std::endl;
		if (bones[m].parentBoneIndex < 0)
		{
			fs << " parent: bone" << "NONE" << "(" << "NONE" << ")" << std::endl;
		}
		else
		{
			fs << " parent: bone" << bones[m].parentBoneIndex << "(" << bones[bones[m].parentBoneIndex].name << ")" << std::endl;
		}
		fs << " poseS:" << bones[m].GlobalBindposeInverse.GetS().mData[0] << "," << bones[m].GlobalBindposeInverse.GetS().mData[1] << "," << bones[m].GlobalBindposeInverse.GetS().mData[2] << "," << bones[m].GlobalBindposeInverse.GetS().mData[3] << " ";
		fs << "poseR:" << bones[m].GlobalBindposeInverse.GetR().mData[0] << "," << bones[m].GlobalBindposeInverse.GetR().mData[1] << "," << bones[m].GlobalBindposeInverse.GetR().mData[2] << "," << bones[m].GlobalBindposeInverse.GetR().mData[3] << " ";
		fs << "poseT:" << bones[m].GlobalBindposeInverse.GetT().mData[0] << "," << bones[m].GlobalBindposeInverse.GetT().mData[1] << "," << bones[m].GlobalBindposeInverse.GetT().mData[2] << "," << bones[m].GlobalBindposeInverse.GetT().mData[3] << std::endl;
		fs << " nodeS:" << bones[m].NodeTransform.GetS().mData[0] << "," << bones[m].NodeTransform.GetS().mData[1] << "," << bones[m].NodeTransform.GetS().mData[2] << "," << bones[m].NodeTransform.GetS().mData[3] << " ";
		fs << "nodeR:" << bones[m].NodeTransform.GetR().mData[0] << "," << bones[m].NodeTransform.GetR().mData[1] << "," << bones[m].NodeTransform.GetR().mData[2] << "," << bones[m].NodeTransform.GetR().mData[3] << " ";
		fs << "nodeT:" << bones[m].NodeTransform.GetT().mData[0] << "," << bones[m].NodeTransform.GetT().mData[1] << "," << bones[m].NodeTransform.GetT().mData[2] << "," << bones[m].NodeTransform.GetT().mData[3] << std::endl;
		fs << " linkS:" << bones[m].LinkTransform.GetS().mData[0] << "," << bones[m].LinkTransform.GetS().mData[1] << "," << bones[m].LinkTransform.GetS().mData[2] << "," << bones[m].LinkTransform.GetS().mData[3] << " ";
		fs << "linkR:" << bones[m].LinkTransform.GetR().mData[0] << "," << bones[m].LinkTransform.GetR().mData[1] << "," << bones[m].LinkTransform.GetR().mData[2] << "," << bones[m].LinkTransform.GetR().mData[3] << " ";
		fs << "linkT:" << bones[m].LinkTransform.GetT().mData[0] << "," << bones[m].LinkTransform.GetT().mData[1] << "," << bones[m].LinkTransform.GetT().mData[2] << "," << bones[m].LinkTransform.GetT().mData[3] << std::endl;
		fs << " geoS:" << bones[m].GeoTransform.GetS().mData[0] << "," << bones[m].GeoTransform.GetS().mData[1] << "," << bones[m].GeoTransform.GetS().mData[2] << "," << bones[m].GeoTransform.GetS().mData[3] << " ";
		fs << "geoR:" << bones[m].GeoTransform.GetR().mData[0] << "," << bones[m].GeoTransform.GetR().mData[1] << "," << bones[m].GeoTransform.GetR().mData[2] << "," << bones[m].GeoTransform.GetR().mData[3] << " ";
		fs << "geoT:" << bones[m].GeoTransform.GetT().mData[0] << "," << bones[m].GeoTransform.GetT().mData[1] << "," << bones[m].GeoTransform.GetT().mData[2] << "," << bones[m].GeoTransform.GetT().mData[3] << std::endl;
		fs << " Animation:" << std::endl;


		if (bones[m].animation != 0)
		{
			AnimusFrame *p = 0;
			for (int n = 0; n < length; ++n)
			{
				p = &bones[m].animation[n];

				fs << "  f" << p->frameNum << ":";
				fs << "S:" << p->GlobalTransform.GetS().mData[0] << "," << p->GlobalTransform.GetS().mData[1] << "," << p->GlobalTransform.GetS().mData[2] << "," << p->GlobalTransform.GetS().mData[3] << " ";
				fs << "R:" << p->GlobalTransform.GetR().mData[0] << "," << p->GlobalTransform.GetR().mData[1] << "," << p->GlobalTransform.GetR().mData[2] << "," << p->GlobalTransform.GetR().mData[3] << " ";
				fs << "T:" << p->GlobalTransform.GetT().mData[0] << "," << p->GlobalTransform.GetT().mData[1] << "," << p->GlobalTransform.GetT().mData[2] << "," << p->GlobalTransform.GetT().mData[3] << std::endl;

			}
		}
	}

	fs.close();
}