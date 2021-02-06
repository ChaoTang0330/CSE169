#include "skin.h"

Skin::Skin(std::string _fileName, Skeleton* _skel, GLint _programID)
{ 
	fileName = _fileName;
	skel = _skel;
	shaderID = _programID;

	load();
	//ckeckNormals();
	invBindingMat();

	// Generate a vertex array (VAO), vertex buffer objects (VBO) and EBO.
	glGenVertexArrays(1, &vao);
#ifdef SHADER_SKINING
	glGenBuffers(5, vbos);
#else
	glGenBuffers(2, vbos);
#endif // SHADER_SKINING
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// 1st VBO, positions
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	//b. Pass in the position data.
#ifdef SHADER_SKINING
	glBufferData(GL_ARRAY_BUFFER,
			sizeof(glm::vec3) * positions.size(),
			positions.data(),
			GL_STATIC_DRAW);
#else
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * positions.size(),
		nullptr,
		GL_STREAM_DRAW);
#endif // SHADER_SKINING

	// c. Enable vertex attribute 0. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	//d. How to read VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//2nd VBO, normals
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	//b. Pass in the data.
#ifdef SHADER_SKINING
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * normals.size(),
		normals.data(),
		GL_STATIC_DRAW);
#else
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * normals.size(),
		nullptr,
		GL_STREAM_DRAW);
#endif // SHADER_SKINING
	// c. Enable vertex attribute 1. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(1);
	//d. How to read VBO
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


#ifdef SHADER_SKINING
	//3nd VBO, number of weights of each vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLint) * skinWeightNum.size(),
		skinWeightNum.data(),
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(GLint), 0);

	//4th VBO, joint index of each vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::ivec4) * skinJointsIdx.size(),
		skinJointsIdx.data(),
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, 4 * sizeof(GLint), 0);

	//5th VBO, joint weights of each vertex
	glBindBuffer(GL_ARRAY_BUFFER, vbos[4]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec4) * skinJointsWeight.size(),
		skinJointsWeight.data(),
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

#endif // SHADER_SKINING


	//vertex index
	//a. Bind to the EBO. We will use it to store the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//b. Pass in the data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(glm::ivec3) * triangles.size(), 
		triangles.data(), 
		GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

Skin::~Skin()
{
#ifdef SHADER_SKINING
	glDeleteBuffers(5, vbos);
#else
	glDeleteBuffers(2, vbos);
#endif
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void Skin::load()
{
	std::cout << "Loading " + fileName << std::endl;
	Tokenizer fileToken;
	fileToken.Open(fileName.c_str());

	char tokenBuff[20];
	fileToken.GetToken(tokenBuff);

	while (tokenBuff[0] != '\0')
	{
		if (strcmp(tokenBuff, "positions") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff); // "{"
			while (count > 0)
			{
				glm::vec3 pos;
				pos.x = fileToken.GetFloat();
				pos.y = fileToken.GetFloat();
				pos.z = fileToken.GetFloat();
				positions.push_back(pos);
				count--;
			}
			fileToken.GetToken(tokenBuff); // "}"
		}
		else if (strcmp(tokenBuff, "normals") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff); // "{"
			while (count > 0)
			{
				glm::vec3 norm;
				norm.x = fileToken.GetFloat();
				norm.y = fileToken.GetFloat();
				norm.z = fileToken.GetFloat();
				normals.push_back(glm::normalize(norm));
				count--;
			}
			fileToken.GetToken(tokenBuff); // "}"
		}
		else if (strcmp(tokenBuff, "triangles") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff); // "{"
			while (count > 0)
			{
				glm::vec3 tri;
				tri.x = fileToken.GetInt();
				tri.y = fileToken.GetInt();
				tri.z = fileToken.GetInt();
				triangles.push_back(tri);
				count--;
			}
			fileToken.GetToken(tokenBuff); // "}"
		}
		else if (strcmp(tokenBuff, "skinweights") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff); // "{"
			while (count > 0)
			{
				skinweights.push_back(std::vector<std::pair<int, float>> ());
				for (int bindsCount = fileToken.GetInt(); bindsCount > 0; bindsCount--)
				{
					int jointIdx = fileToken.GetInt();
					float weight = fileToken.GetFloat();
					skinweights.back().push_back({jointIdx, weight});
				}
				count--;
			}
			fileToken.GetToken(tokenBuff); // "}"
		}
		else if (strcmp(tokenBuff, "bindings") == 0)
		{
			int count = fileToken.GetInt();
			fileToken.GetToken(tokenBuff); // "{"
			while (count > 0)
			{
				fileToken.GetToken(tokenBuff);
				if (strcmp(tokenBuff, "matrix") == 0)
				{
					bindings.push_back(glm::mat4(1.0));
					fileToken.GetToken(tokenBuff); // "{"
					for (int i = 0; i < 4; i++)
					{
						bindings.back()[i].x = fileToken.GetFloat();
						bindings.back()[i].y = fileToken.GetFloat();
						bindings.back()[i].z = fileToken.GetFloat();
					}
					fileToken.GetToken(tokenBuff); // "}"
				}
				count--;
			}
			fileToken.GetToken(tokenBuff); // "}"
		}

		//next token
		fileToken.GetToken(tokenBuff);
	}

	fileToken.Close();

	invBindings.resize(bindings.size());
	posMat.resize(bindings.size());
	normMat.resize(bindings.size());
	skinWeightNum.resize(skinweights.size());
	skinJointsIdx.resize(skinweights.size());
	skinJointsWeight.resize(skinweights.size());

	for (size_t i = 0; i < skinweights.size(); i++)
	{
		skinWeightNum[i] = skinweights[i].size();

		for (size_t j = 0; j < skinweights[i].size(); j++)
		{
			skinJointsIdx[i][j] = skinweights[i][j].first;
			skinJointsWeight[i][j] = skinweights[i][j].second;
		}
	}
}

void Skin::ckeckNormals()
{
	for (glm::ivec3& triIdx : triangles)
	{
		glm::vec3 triNorm = glm::cross(
			positions[triIdx.y] - positions[triIdx.x],
			positions[triIdx.z] - positions[triIdx.y]);
		triNorm = glm::normalize(triNorm);

		if (glm::dot(triNorm, normals[triIdx.x]) < 0)
		{
			normals[triIdx.x] *= -1;
		}

		if (glm::dot(triNorm, normals[triIdx.y]) < 0)
		{
			normals[triIdx.y] *= -1;
		}

		if (glm::dot(triNorm, normals[triIdx.z]) < 0)
		{
			normals[triIdx.z] *= -1;
		}
	}
}

void Skin::invBindingMat()
{
	for (size_t i = 0; i < bindings.size(); i++)
	{
		invBindings[i] = glm::inverse(bindings[i]);
	}
}

void Skin::update()
{
	if (skel == nullptr) return;

#ifdef SHADER_SKINING
	for (size_t i = 0; i < bindings.size(); i++)
	{
		posMat[i] = skel->getWorldMat(i) * invBindings[i];
		normMat[i] = glm::transpose(glm::inverse(skel->getWorldMat(i)))
			* glm::transpose(bindings[i]);
	}
#else
	//Bind buffer to positions VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	for (size_t i = 0; i < positions.size(); i++)
	{
		glm::vec3 newPos(0.0f);
		for (auto& currW : skinweights[i])
		{
			newPos += glm::vec3(currW.second
				* skel->getWorldMat(currW.first)
				* invBindings[currW.first]
				* glm::vec4(positions[i], 1.0f));
		}
		//currPositions[i] = newPos;
		glBufferSubData(GL_ARRAY_BUFFER,
			sizeof(glm::vec3) * i,
			sizeof(glm::vec3),
			&newPos);
	}


	//Bind buffer to normals VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	for (size_t i = 0; i < normals.size(); i++)
	{
		glm::vec3 newNorm(0.0f);
		for (auto& currW : skinweights[i])
		{
			newNorm += glm::vec3(currW.second
				* glm::transpose(glm::inverse(skel->getWorldMat(currW.first)))
				* glm::transpose(bindings[currW.first])
				* glm::vec4(normals[i], 0.0f));
		}
		//currNormals[i] = glm::normalize(newNorm);
		glBufferSubData(GL_ARRAY_BUFFER,
			sizeof(glm::vec3) * i,
			sizeof(glm::vec3),
			&newNorm);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif // SHADER_SKINING

}

void Skin::draw(const glm::mat4& viewProjMtx)
{
	glUseProgram(shaderID);

	glm::mat4 model(1.0);
	glm::vec3 color(1.0f, 0.95f, 1.0f);

	// get the locations and send the uniforms to the shader
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shaderID, "DiffuseColor"), 1, &color[0]);

#ifdef SHADER_SKINING
	for (size_t i = 0; i < posMat.size(); i++)
	{
		std::string variable = "posMat[" + std::to_string(i) + "]";
		GLint posMatLoc = glGetUniformLocation(shaderID, variable.c_str());
		glUniformMatrix4fv(posMatLoc, 1, GL_FALSE, (float*)&posMat[i]);
	}

	for (size_t i = 0; i < normMat.size(); i++)
	{
		std::string variable = "normMat[" + std::to_string(i) + "]";
		GLint normMatLoc = glGetUniformLocation(shaderID, variable.c_str());
		glUniformMatrix4fv(normMatLoc, 1, GL_FALSE, (float*)&normMat[i]);
	}
#endif // SHADER_SKINING

	// Bind to the VAO.
	glBindVertexArray(vao);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0); // mode, count, type, indices
																			
	// Unbind from the VAO and shader program.
	glBindVertexArray(0);
	glUseProgram(0);
}