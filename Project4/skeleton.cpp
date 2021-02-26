#include "skeleton.h"

Skeleton::Skeleton(std::string _fileName, GLuint _shaderID)
{
	fileName = _fileName;
	toWorld = glm::mat4(1.0f);
	shaderID = _shaderID;
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
		jointRoot->load(fileToken, joints);
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

void Skeleton::draw(const glm::mat4& viewProjMtx)
{
	jointRoot->draw(viewProjMtx , shaderID); // * toWorld
}

glm::mat4 Skeleton::getWorldMat(int jointNum)
{
	return joints[jointNum]->getWorldMat();
}

std::string Skeleton::getJointName(int jointNum)
{
	return joints[jointNum]->getJointName();
}

void Skeleton::getJointRot(int jointNum, glm::vec3& rot)
{
	joints[jointNum]->getRot(rot);
}

void Skeleton::setJointRot(int jointNum, glm::vec3& rot)
{
	joints[jointNum]->setRot(glm::vec3(rot[0], rot[1], rot[2]));
}

void Skeleton::getJointRotMin(int jointNum, glm::vec3& rotMin)
{
	joints[jointNum]->getRotMin(rotMin);
}

void Skeleton::getJointRotMax(int jointNum, glm::vec3& rotMax)
{
	joints[jointNum]->getRotMax(rotMax);
}