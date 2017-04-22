#include "AnimusInstanceMaterialNode.h"

AnimusInstanceMaterialNode::AnimusInstanceMaterialNode(int _textureCount)
{
	setNodeType(AnimusNodeType::InstanceMaterial);

	textureCount = _textureCount;
	
	textures = new gli::texture[textureCount];
	TEXs = new GLuint[textureCount];
	program = 0;
	vert = 0;
	frag = 0;
	vertexColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	vertSrc = 0;
	fragSrc = 0;

	boneCount = 0;
	frameCount = 0;
}


AnimusInstanceMaterialNode::~AnimusInstanceMaterialNode()
{
}

//load first, then set up after glutInit
void AnimusInstanceMaterialNode::glSetUp( const glm::mat4 &PV, const glm::vec4 &LightDirection, AnimusAnimationNode *pAnim0, AnimusAnimationNode *pAnim1)
{
	glSetUpShader(pAnim0->bones.size(), pAnim0->length);
	glGenTextures(textureCount, TEXs);
	for (int i = 0; i < textureCount; ++i)
	{
		glSetUpTexture(i);
	}
	glGenBuffers(2, TBOs);
	glGenTextures(2, TBTEXs);
	glSetUpTexBuffer(pAnim0, 0);
	glSetUpTexBuffer(pAnim1, 1);
	glSetUpUniforms(PV, LightDirection, pAnim0, pAnim1);
}

void AnimusInstanceMaterialNode::glSetUpShader(int _boneCount, int _frameCount)
{
	boneCount = _boneCount;
	frameCount = _frameCount;

	GLint maxTexel(0);
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTexel);
	//printf("maxTexel: %d\n", maxTexel);

	vertSrc = "#version 450 core\n"
		"in vec4 vPosition;"
		"in vec3 vNormal;"
		"in vec2 vTexcoord;"
		"out gl_PerVertex"
		"{"
		"vec4 gl_Position;"
		"};"
		"out vec4 fColor;"
		"out vec2 fTexcoord;"
		"uniform mat4 PV;"//different from un-instanced material CONSTANT
		"in mat4 vMatrix;"//different from un-instanced material PRE-INSTANCE
		"in int frame;"//newly added PRE-INSTANCE
		"in int anim;"//newly added PRE-INSTANCE new new new
		"in vec4 boneWeight;"//newly added PRE-VERTEX
		"in ivec4 boneIndex;"//newly added PRE-VERTEX
		"uniform vec4 vColor;"
		"uniform vec4 LightDirection;"
		"uniform samplerBuffer boneTex0;"//newly added CONSTANT
		"uniform samplerBuffer boneTex1;"//newly added CONSTANT
		"uniform int frameMax0;"////newly added CONSTANT
		"uniform int frameMax1;"////newly added CONSTANT
		"void main(void)"
		"{"
		"mat4 finalTransform = mat4(0);"
		"for(int i = 0;i<4;i++)"
		"{"
		"int index;"
		"float weight;"
		"switch(i)"
		"{"
		"case 0: index = boneIndex.x; weight = boneWeight.x; break;"
		"case 1: index = boneIndex.y; weight = boneWeight.y; break;"
		"case 2: index = boneIndex.z; weight = boneWeight.z; break;"
		"case 3: index = boneIndex.w; weight = boneWeight.w; break;"
		"}"
		"vec4 row1, row2, row3, row4;"
		"if(anim==0)"
		"{"
		"row1 = texelFetch(boneTex0, index * frameMax0 * 4 + frame * 4 + 0);"
		"row2 = texelFetch(boneTex0, index * frameMax0 * 4 + frame * 4 + 1);"
		"row3 = texelFetch(boneTex0, index * frameMax0 * 4 + frame * 4 + 2);"
		"row4 = texelFetch(boneTex0, index * frameMax0 * 4 + frame * 4 + 3);"
		"}"
		"else"
		"{"
		"row1 = texelFetch(boneTex1, index * frameMax1 * 4 + frame * 4 + 0);"
		"row2 = texelFetch(boneTex1, index * frameMax1 * 4 + frame * 4 + 1);"
		"row3 = texelFetch(boneTex1, index * frameMax1 * 4 + frame * 4 + 2);"
		"row4 = texelFetch(boneTex1, index * frameMax1 * 4 + frame * 4 + 3);"
		"}"
		"mat4 boneTransform = weight * transpose(mat4(row1, row2, row3, row4));"
		"finalTransform += boneTransform;"
		"}"
		"gl_Position = PV * vMatrix * finalTransform * vPosition;"
		"vec3 worldLight = normalize(LightDirection.xyz);"
		"vec3 worldNormal = normalize(mat3(vMatrix) * vNormal);"
		"fColor = vec4(vColor.xyz * (0.5 * dot(worldNormal, worldLight) + 0.5), vColor.w);"// vColor * clamp(dot(vNormal, LightDirection.xyz), 0.0, 1.0);//vColor;//vec4(worldNormal, 1.0); //vec4(worldLight, 1.0);
		"fTexcoord = vTexcoord;"
		"}";

	fragSrc = "#version 450 core\n"
		"in vec4 fColor;"
		"in vec2 fTexcoord;"
		"out vec4 color;"
		"uniform sampler2D Tex0;"
		"void main(void)"
		"{"
		"vec2 realTexcoord = vec2(fTexcoord.s, 1.0 - fTexcoord.t);"//shiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiit,dds picture use filpped UV coordinates
		"color = fColor * texture(Tex0, realTexcoord);"
		"}";

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vertSrc, 0);
	glCompileShader(vert);

	GLint result;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		printf("vert compile failed\n");
		char log[ShaderInfoLogLength];
		glGetShaderInfoLog(vert, ShaderInfoLogLength, 0, log);
		printf("%s", log);
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragSrc, 0);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		printf("frag compile failed\n");
		char log[ShaderInfoLogLength];
		glGetShaderInfoLog(frag, ShaderInfoLogLength, 0, log);
		printf("%s", log);
	}

	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		printf("program link failed\n");
		char log[ShaderInfoLogLength];
		glGetProgramInfoLog(program, ShaderInfoLogLength, 0, log);
		printf("%s", log);
	}

	glUseProgram(program);//
}

void AnimusInstanceMaterialNode::glSetUpUniforms(const glm::mat4 &PV, const glm::vec4 &LightDirection, AnimusAnimationNode *pAnim0, AnimusAnimationNode  *pAnim1)
{
	//outside uniforms
	GLint matrix = glGetUniformLocation(program, "PV");
	//printf("PV: %d\n", matrix);
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &PV[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!

	GLint lightDirection = glGetUniformLocation(program, "LightDirection");//
	glUniform4fv(lightDirection, 1, &LightDirection[0]);//second parameter indicates the number of targeted vector, not the number of components in your vector!!!!!!

	//inside uniforms
	for (int i = 0; i < textureCount; ++i)
	{
		std::stringstream ss;
		ss << "Tex" << i;
		std::string s = ss.str();
		GLint Tex = glGetUniformLocation(program, s.c_str());
		glUniform1i(Tex, i);
	}

	GLint boneTex0 = glGetUniformLocation(program, "boneTex0");
	glUniform1i(boneTex0, textureCount);

	GLint boneTex1 = glGetUniformLocation(program, "boneTex1");
	glUniform1i(boneTex1, textureCount+1);

	GLint frameMax0 = glGetUniformLocation(program, "frameMax0");
	glUniform1i(frameMax0, pAnim0->length);

	GLint frameMax1 = glGetUniformLocation(program, "frameMax1");
	glUniform1i(frameMax1, pAnim1->length);

	GLint color = glGetUniformLocation(program, "vColor");
	glUniform4fv(color, 1, &vertexColor[0]);//second parameter indicates the number of targeted vector, not the number of components in your vector!!!!!!
}

void AnimusInstanceMaterialNode::glSetUpTexture(int index)
{
	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(textures[index].format(), textures[index].swizzles());
	GLenum Target = GL.translate(textures[index].target());

	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(Target, TEXs[index]);

	glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(textures[index].levels() - 1));
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

	glm::tvec3<GLsizei> const Extent(textures[index].extent());
	GLsizei const FaceTotal = static_cast<GLsizei>(textures[index].layers() * textures[index].faces());

	switch (textures[index].target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(Target, static_cast<GLint>(textures[index].levels()), Format.Internal, Extent.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(Target, static_cast<GLint>(textures[index].levels()), Format.Internal, Extent.x, textures[index].target() == gli::TARGET_2D ? Extent.y : FaceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(Target, static_cast<GLint>(textures[index].levels()), Format.Internal, Extent.x, Extent.y, textures[index].target() == gli::TARGET_3D ? Extent.z : FaceTotal);
		break;
	default:
		assert(0);
		break;
	}

	for (std::size_t Layer = 0; Layer < textures[index].layers(); ++Layer)
		for (std::size_t Face = 0; Face < textures[index].faces(); ++Face)
			for (std::size_t Level = 0; Level < textures[index].levels(); ++Level)
			{
				GLsizei const LayerGL = static_cast<GLsizei>(Layer);
				glm::tvec3<GLsizei> Extent(textures[index].extent(Level));
				Target = gli::is_target_cube(textures[index].target()) ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face) : Target;
				switch (textures[index].target())
				{
				case gli::TARGET_1D:
					if (gli::is_compressed(textures[index].format()))
						glCompressedTexSubImage1D(Target, static_cast<GLint>(Level), 0, Extent.x, Format.Internal, static_cast<GLsizei>(textures[index].size(Level)), textures[index].data(Layer, Face, Level));
					else
						glTexSubImage1D(Target, static_cast<GLint>(Level), 0, Extent.x, Format.External, Format.Type, textures[index].data(Layer, Face, Level));
					break;
				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
					if (gli::is_compressed(textures[index].format()))
						glCompressedTexSubImage2D(Target, static_cast<GLint>(Level), 0, 0, Extent.x, textures[index].target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y, Format.Internal, static_cast<GLsizei>(textures[index].size(Level)), textures[index].data(Layer, Face, Level));
					else
						glTexSubImage2D(Target, static_cast<GLint>(Level), 0, 0, Extent.x, textures[index].target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y, Format.External, Format.Type, textures[index].data(Layer, Face, Level));
					break;
				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
					if (gli::is_compressed(textures[index].format()))
						glCompressedTexSubImage3D(Target, static_cast<GLint>(Level), 0, 0, 0, Extent.x, Extent.y, textures[index].target() == gli::TARGET_3D ? Extent.z : LayerGL, Format.Internal, static_cast<GLsizei>(textures[index].size(Level)), textures[index].data(Layer, Face, Level));
					else
						glTexSubImage3D(Target, static_cast<GLint>(Level), 0, 0, 0, Extent.x, Extent.y, textures[index].target() == gli::TARGET_3D ? Extent.z : LayerGL, Format.External, Format.Type, textures[index].data(Layer, Face, Level));
					break;
				default: assert(0); break;
				}
			}
}

void AnimusInstanceMaterialNode::glUpdatePV(const glm::mat4 &PV)
{
	//outside uniforms
	glUseProgram(program);
	GLint matrix = glGetUniformLocation(program, "PV");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &PV[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!
}

void AnimusInstanceMaterialNode::glSetUpTexBuffer(AnimusAnimationNode *pAnim, int slot)//
{
	int boneCount = pAnim->bones.size();
	int animLength = pAnim->length;

	int vecCount = RowPerMatrix * boneCount * animLength;
	int size = sizeof(glm::vec4) * vecCount;
	glm::vec4 *bufferData = new glm::vec4[vecCount];

	for (int i = 0; i < boneCount; ++i)
	{
		for (int j = 0; j < animLength; ++j)
		{
			glm::mat4 boneMat = pAnim->bones[i].animation[j].globalTransform * pAnim->bones[i].globalBindposeInverse;
			int vecNum = i * animLength * RowPerMatrix + j * RowPerMatrix;
			for (int k = 0; k < RowPerMatrix; ++k)
			{
				bufferData[vecNum + k] = glm::vec4(boneMat[0][k], boneMat[1][k], boneMat[2][k], boneMat[3][k]);
			}
		}
	}

	glBindBuffer(GL_TEXTURE_BUFFER, TBOs[slot]);
	glBufferData(GL_TEXTURE_BUFFER, size, &bufferData[0][0], GL_STATIC_DRAW);
	

	glActiveTexture(GL_TEXTURE0 + textureCount + slot);//TextureCount is the num of usual 2D textures used in shader, it does not include texture used by texture buffer. Thus the new texture unit is GL_TEXTURE0 + textureCount.
	glBindTexture(GL_TEXTURE_BUFFER, TBTEXs[slot]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, TBOs[slot]);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	delete[] bufferData;
}

int AnimusInstanceMaterialNode::loadDdsTex(char* _filename, int index)
{
	textures[index] = gli::load(_filename);
	if (textures[index].empty())
		return 1;

	return 0;
}