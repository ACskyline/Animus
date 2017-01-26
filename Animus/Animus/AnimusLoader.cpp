#include "AnimusLoader.h"

std::vector<glm::vec4> AnimusLoader::V;

std::vector<glm::vec2> AnimusLoader::T;

std::vector<glm::vec3> AnimusLoader::N;

std::vector<GLuint> AnimusLoader::VI;

std::vector<GLuint> AnimusLoader::TI;

std::vector<GLuint> AnimusLoader::NI;

void AnimusLoader::parseFace(std::stringstream &ss, std::vector<GLuint> &vIndices, std::vector<GLuint> &tIndices, std::vector<GLuint> &nIndices)
{
	char discard;
	char peek;
	int count;
	GLuint data;

	//One vertex in one loop
	do
	{
		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			vIndices.push_back(data);
			ss >> discard;
		}
		else
		{
			// do nothing
		}

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			tIndices.push_back(data);
			ss >> discard;
		}
		else
		{
			// do nothing
		}

		ss >> peek;
		if (peek >= '0' && peek <= '9')
		{
			ss.putback(peek);
			ss >> data;
			nIndices.push_back(data);
			//no discard here because it is the end for this vertex
		}
		else
		{
			// do nothing
		}

	} while (!ss.eof());
}

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
				int vertexCount = 0;
				std::vector<GLuint> vIndices, tIndices, nIndices;

				//Parsing
				parseFace(ss, vIndices, tIndices, nIndices);

				//Collecting(Reassembling)
				for (int i = 0; i < vIndices.size(); i++)
				{
					if (i >= 3)
					{
						VI.push_back(vIndices.at(0) - 1);
						VI.push_back(vIndices.at(i - 1) - 1);
					}
					VI.push_back(vIndices.at(i) - 1);
				}
				for (int i = 0; i < tIndices.size(); i++)
				{
					if (i >= 3)
					{
						TI.push_back(tIndices.at(0) - 1);
						TI.push_back(tIndices.at(i - 1) - 1);
					}
					TI.push_back(tIndices.at(i) - 1);
				}
				for (int i = 0; i < nIndices.size(); i++)
				{
					if (i >= 3)
					{
						NI.push_back(nIndices.at(0) - 1);
						NI.push_back(nIndices.at(i - 1) - 1);
					}
					NI.push_back(nIndices.at(i) - 1);
				}
				//for (int i = 0; i < 3; i++)
				//{
				//	if (vIndices[i] != 0)//If current value equals to initial value, no value will be pushed.
				//	{
				//		VI.push_back(vIndices[i] - 1);
				//	}
				//	if (tIndices[i] != 0)//If current value equals to initial value, no value will be pushed.
				//	{
				//		TI.push_back(tIndices[i] - 1);
				//	}
				//	if (nIndices[i] != 0)//If current value equals to initial value, no value will be pushed.
				//	{
				//		NI.push_back(nIndices[i] - 1);
				//	}
				//}
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

void AnimusLoader::printALL(std::vector<glm::vec4> &V, std::vector<glm::vec2> &T, std::vector<glm::vec3> &N, std::vector<GLuint> VI, std::vector<GLuint> TI, std::vector<GLuint> NI)
{
	for (int i = 0; i < V.size(); i++)
	{
		std::cout << "V:" << V.at(i).x << "," << V.at(i).y << "," << V.at(i).z << "," << V.at(i).w << std::endl;
	}

	for (int i = 0; i < T.size(); i++)
	{
		std::cout << "T:" << T.at(i).x << "," << T.at(i).y << std::endl;
	}

	for (int i = 0; i < N.size(); i++)
	{
		std::cout << "N:" << N.at(i).x << "," << N.at(i).y << "," << N.at(i).z << std::endl;
	}

	for (int i = 0; i < VI.size(); i++)
	{
		std::cout << "VI:" << VI.at(i) << std::endl;
	}

	for (int i = 0; i < TI.size(); i++)
	{
		std::cout << "TI:" << TI.at(i) << std::endl;
	}

	for (int i = 0; i < NI.size(); i++)
	{
		std::cout << "NI:" << NI.at(i) << std::endl;
	}
}
