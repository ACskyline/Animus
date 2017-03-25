#include "AnimusMaterialNode.h"

AnimusMaterialNode::AnimusMaterialNode(int _textureCount)
{
	setNodeType(AnimusNodeType::Material);
	textureCount = _textureCount;
	textures = new gli::texture[textureCount];
	TEXs = new GLuint[textureCount];
	program = 0;
	vert = 0;
	frag = 0;
	vertexColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vertSrc = "#version 450 core\n"
		"in vec4 vPosition;"
		"in vec3 vNormal;"//
		"in vec2 vTexcoord;"
		"out gl_PerVertex"
		"{"
		"vec4 gl_Position;"
		"};"
		"out vec4 fColor;"
		"out vec2 fTexcoord;"
		"uniform mat4 vMatrix;"
		"uniform vec4 vColor;"
		"uniform vec4 LightDirection;"//
		"void main(void)"
		"{"
		"gl_Position = vMatrix * vPosition;"
		"vec3 worldLight = normalize(LightDirection.xyz);"
		"vec3 worldNormal = normalize(mat3(vMatrix) * vNormal);"
		"fColor = vColor * (0.5 * dot(worldNormal, worldLight) + 0.5);"// vColor * clamp(dot(vNormal, LightDirection.xyz), 0.0, 1.0);//vColor;//vec4(worldNormal, 1.0); //vec4(worldLight, 1.0);
		"fTexcoord = vTexcoord;"
		"}";
	fragSrc = "#version 450 core\n"
		"in vec4 fColor;"
		"in vec2 fTexcoord;"
		"out vec4 color;"
		"uniform sampler2D Tex;"
		"void main(void)"
		"{"
		"vec2 realTexcoord = vec2(fTexcoord.s, 1.0 - fTexcoord.t);"//shiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiit,dds picture use filpped UV coordinates
		"color = fColor * texture(Tex, realTexcoord);"
		//"color = fColor;"
		"}";
}


AnimusMaterialNode::~AnimusMaterialNode()
{
}

//load first, then set up after glutInit
void AnimusMaterialNode::glSetUp(const glm::mat4 &vertexMatrix, const glm::vec4 &LightDirection)
{
	glSetUpShader();
	glGenTextures(textureCount, TEXs);
	for (int i = 0; i < textureCount; ++i)
	{
		glSetUpTexture(i);
	}
	glSetUpUniforms(vertexMatrix, LightDirection);
}

void AnimusMaterialNode::glSetUpUniforms(const glm::mat4 &vertexMatrix, const glm::vec4 &LightDirection)
{
	//outside uniforms
	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!

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

	GLint color = glGetUniformLocation(program, "vColor");
	glUniform4fv(color, 1, &vertexColor[0]);//second parameter indicates the number of targeted vector, not the number of components in your vector!!!!!!
}

void AnimusMaterialNode::glSetUpShader()
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

void AnimusMaterialNode::glSetUpTexture(int index)
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

void AnimusMaterialNode::glUpdateMatrix(const glm::mat4 &vertexMatrix)
{
	//outside uniforms
	glUseProgram(program);
	GLint matrix = glGetUniformLocation(program, "vMatrix");
	glUniformMatrix4fv(matrix, 1, GL_FALSE, &vertexMatrix[0][0]);//second parameter indicates the number of targeted matrices, not the number of components in you matrix!!!!!!
}

int AnimusMaterialNode::loadDdsTex(char* _filename, int index)
{
	textures[index] = gli::load(_filename);
	if (textures[index].empty())
		return 1;

	return 0;
}