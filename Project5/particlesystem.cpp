#include "particlesystem.h"

ParticleSystem::ParticleSystem(GLuint ID, GLuint groundID)
{
	groundSize = 10.0f;
	groundCenter = glm::vec3(0.0f);
	groundTopLeft = groundCenter - glm::vec3(5.0f, 0.0f, 5.0f);
	groundBottomRight = groundCenter + glm::vec3(5.0f, 0.0f, 5.0f);
	ground = new Ground(groundCenter, groundSize, groundID);

	radius = 0.05f;
	mass = 0.01f;
	g = 9.8f;
	airDensity = 1.225f;
	dragConst = 0.1f;
	windSpeed = glm::vec3(0.0f);
	elasticity = 0.8f;
	friction = 0.01f;
	
	creationRate = 30;

	model = glm::scale(glm::vec3(radius));

	initialPos = glm::vec3(0.0f, 5.0f, 0.0f);
	initialPosVar = glm::vec3(0.1f, 0.1f, 0.1f);
	initialVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
	initialVelocityVar = glm::vec3(1.0f, 1.0f, 1.0f);
	initialLifeSpan = 10.0f;
	initialLifeSpanVar = 0.0f;

	programID = ID;
	LoadSphere();

	particles.resize(maxParticles);
	positions.resize(maxParticles);
	colors.resize(maxParticles);

	for (auto& p : particles)
	{
		p = new GeneralParticle(&radius, &mass, &g,
			&airDensity, &dragConst, &windSpeed,
			&elasticity, &friction,
			&groundTopLeft, &groundBottomRight);
	}

	glUseProgram(programID);

	// Generate a vertex array (VAO), vertex buffer objects (VBO) and EBO.
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, vbos);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// 1st VBO, sphere points
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	//b. Pass in the position data.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * spherePoints.size(),
		spherePoints.data(),
		GL_STATIC_DRAW);
	// c. Enable vertex attribute 0. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	//d. How to read VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//2nd VBO, sphere normals
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	//b. Pass in the data.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * sphereNormals.size(),
		sphereNormals.data(),
		GL_STATIC_DRAW);
	// c. Enable vertex attribute 1. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(1);
	//d. How to read VBO
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//3nd VBO, particle positions
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
	//b. Pass in the data.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * maxParticles,
		nullptr,
		GL_STREAM_DRAW);
	// c. Enable vertex attribute 2. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(2);
	//d. How to read VBO
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//4nd VBO, particle colors
	//a. Bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
	//b. Pass in the data.
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(glm::vec3) * maxParticles,
		nullptr,
		GL_STREAM_DRAW);
	// c. Enable vertex attribute 3. Creat channel
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(3);
	//d. How to read VBO
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//vertex index
	//a. Bind to the EBO. We will use it to store the indices.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//b. Pass in the data.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(glm::ivec3) * sphereFaces.size(),
		sphereFaces.data(),
		GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	glUseProgram(0);


	prevT = clock();
	srand((unsigned)prevT);
}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::LoadSphere()
{
	std::vector<glm::vec3> pointsTemp;
	std::vector<glm::vec3> pointsNormalTemp;
	std::vector<glm::ivec3> facesNormalTemp;
	std::vector<glm::ivec3> facesTemp;

	std::ifstream objFile("PointObj/eyeball_s.obj"); // The obj file we are reading.

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;
				// Process the point. For example, you can save it to a.
				point *= 12.5f;
				pointsTemp.push_back(point);

				//coloring
				//ss >> point.x >> point.y >> point.z;
				//pointsColor.push_back(point);

			}
			else if (label == "vn")
			{
				glm::vec3 pointNormal;
				ss >> pointNormal.x >> pointNormal.y >> pointNormal.z;

				pointsNormalTemp.push_back(pointNormal);
			}
			else if (label == "f")
			{
				glm::ivec3 face;
				glm::ivec3 faceNormal;
				char tempChar;
				unsigned tempNumber;

				ss >> face.x >> tempChar >> tempNumber >> tempChar >> faceNormal.x >>
					face.y >> tempChar >> tempNumber >> tempChar >> faceNormal.y >>
					face.z >> tempChar >> tempNumber >> tempChar >> faceNormal.z;

				//start from 0	
				face.x--;
				face.y--;
				face.z--;
				faceNormal.x--;
				faceNormal.y--;
				faceNormal.z--;

				facesTemp.push_back(face);
				facesNormalTemp.push_back(faceNormal);
			}
		}
	}
	else
	{
		std::cerr << "Can't open the file sphere"<< std::endl;
	}

	objFile.close();


	for (int i = 0; i < facesTemp.size(); i++)
	{
		glm::ivec3 tempFace;
		spherePoints.push_back(pointsTemp[facesTemp[i].x]);
		spherePoints.push_back(pointsTemp[facesTemp[i].y]);
		spherePoints.push_back(pointsTemp[facesTemp[i].z]);

		sphereNormals.push_back(pointsNormalTemp[facesNormalTemp[i].x]);
		sphereNormals.push_back(pointsNormalTemp[facesNormalTemp[i].y]);
		sphereNormals.push_back(pointsNormalTemp[facesNormalTemp[i].z]);

		tempFace.x = 3 * i;
		tempFace.y = 3 * i + 1;
		tempFace.z = 3 * i + 2;

		sphereFaces.push_back(tempFace);
	}
}

void ParticleSystem::Update()
{
	clock_t currT = clock();
	float deltaT = (currT - prevT) / 1000.0f;
	prevT = currT;
	creationInterval += deltaT;
	int newParticleNum = creationInterval * creationRate;
	creationInterval -= newParticleNum / creationRate;

	currParticleNum = 0;
	deltaT /= (float)overSample;
	for (int i = 0; i < maxParticles; i++)
	{
		if (particles[i]->GetStatus())
		{
			for (int j = 0; j < overSample; j++)
			{
				particles[i]->Update(deltaT);
			}
			particles[i]->GetPosition(positions[currParticleNum]);
			particles[i]->GetColor(colors[currParticleNum]);
			currParticleNum++;
		}
	}

	for (int i = 0; i < newParticleNum && currParticleNum < maxParticles; i++)
	{
		int currParticleIdx = FindIdelParticle();
		if (currParticleIdx == -1) break;
		particles[currParticleIdx]->SetPosition(
			glm::vec3(
				sqrt(initialPosVar.x) * randNormal(randEng) + initialPos.x, 
				sqrt(initialPosVar.y) * randNormal(randEng) + initialPos.y,
				sqrt(initialPosVar.z) * randNormal(randEng) + initialPos.z)
		);
		particles[currParticleIdx]->SetVelocity(
			glm::vec3(
				sqrt(initialVelocityVar.x) * randNormal(randEng) + initialVelocity.x,
				sqrt(initialVelocityVar.y) * randNormal(randEng) + initialVelocity.y,
				sqrt(initialVelocityVar.z) * randNormal(randEng) + initialVelocity.z)
		);
		particles[currParticleIdx]->SetLifeSpan(
			sqrt(initialLifeSpanVar) * randNormal(randEng) + initialLifeSpan);
		particles[currParticleIdx]->SetColor(
			glm::vec3(
				rand() / (float)RAND_MAX * 0.5f + 0.5f,
				rand() / (float)RAND_MAX * 0.5f + 0.5f,
				rand() / (float)RAND_MAX * 0.5f + 0.5f)
		);
		particles[currParticleIdx]->SetActive();

		particles[currParticleIdx]->GetPosition(positions[currParticleNum]);
		particles[currParticleIdx]->GetColor(colors[currParticleNum]);
		currParticleNum++;
	}

	//transfer to vbo
	glUseProgram(programID);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, positions.data(), sizeof(glm::vec3) * currParticleNum);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[3]);
	ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, colors.data(), sizeof(glm::vec3) * currParticleNum);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glUseProgram(0);
}

int ParticleSystem::FindIdelParticle()
{
	for (int i = lastIdx; i < maxParticles; i++)
	{
		if (!particles[i]->GetStatus())
		{
			lastIdx++;
			return i;
		}
	}

	for (int i = 0; i < lastIdx; i++)
	{
		if (!particles[i]->GetStatus())
		{
			lastIdx++;
			return i;
		}
	}

	return -1;
}

void ParticleSystem::Draw(const glm::mat4& viewProjMtx)
{
	glUseProgram(programID);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(programID, "viewProj"), 1, GL_FALSE, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, (float*)&model);

	// Bind to the VAO.
	glBindVertexArray(vao);

	glVertexAttribDivisor(0, 0); // vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 0); // normals
	glVertexAttribDivisor(2, 1); // particle center
	glVertexAttribDivisor(3, 1); // particle color

	// draw the points using triangles, indexed with the EBO
	//glDrawElements(GL_TRIANGLES, sphereFaces.size() * 3, GL_UNSIGNED_INT, 0); // mode, count, type, indices
	glDrawElementsInstanced(GL_TRIANGLES, sphereFaces.size() * 3, GL_UNSIGNED_INT, 0, currParticleNum);

	// Unbind from the VAO and shader program.
	glBindVertexArray(0);
	glUseProgram(0);

	ground->Draw(viewProjMtx);
}