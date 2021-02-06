#include "skeleton.h"

Skeleton::Skeleton(std::string _fileName, const glm::mat4& _toWorld)
{
	fileName = _fileName;
	toWorld = _toWorld;
	load();
}

Skeleton::~Skeleton()
{
	delete jointRoot;
}

void Skeleton::load()
{
	std::cout << "Loading " + fileName << std::endl;
	Tokenizer* fileToken = new Tokenizer();
	fileToken->Open(fileName.c_str());

	char tokenBuff[20];
	fileToken->GetToken(tokenBuff);
	if (strcmp(tokenBuff, "balljoint") == 0)
	{
		jointRoot = new BallJoint();
		jointRoot->load(fileToken);
	}

	fileToken->Close();
	delete fileToken;

	jointRoot->update(toWorld);
}

void Skeleton::update(const glm::mat4& _toWorld)
{
	toWorld = _toWorld;
	jointRoot->update(toWorld);
}

void Skeleton::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	jointRoot->draw(viewProjMtx * toWorld, shader);
}