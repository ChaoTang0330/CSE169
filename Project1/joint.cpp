#include "joint.h"

BallJoint::BallJoint()
{
	rotX = new DOF();
	rotY = new DOF();
	rotZ = new DOF();
}

BallJoint::~BallJoint()
{
	for (BallJoint* child : children)
	{
		child->~BallJoint();
		delete child;
	}
}

void BallJoint::load(Tokenizer* fileToken)
{
	char tokenBuff[20];

	fileToken->GetToken(tokenBuff);
	jointName = tokenBuff;
	fileToken->GetToken(tokenBuff);

	while (1)
	{
		fileToken->GetToken(tokenBuff);

		if (strcmp(tokenBuff, "offset") == 0)
		{
			offset.x = fileToken->GetFloat();
			offset.y = fileToken->GetFloat();
			offset.z = fileToken->GetFloat();
		}
		else if (strcmp(tokenBuff, "boxmin") == 0)
		{
			boxmin.x = fileToken->GetFloat();
			boxmin.y = fileToken->GetFloat();
			boxmin.z = fileToken->GetFloat();
		}
		else if (strcmp(tokenBuff, "boxmax") == 0)
		{
			boxmax.x = fileToken->GetFloat();
			boxmax.y = fileToken->GetFloat();
			boxmax.z = fileToken->GetFloat();
		}
		else if (strcmp(tokenBuff, "rotxlimit") == 0)
		{
			rotX->setMin(fileToken->GetFloat());
			rotX->setMax(fileToken->GetFloat());
		}
		else if (strcmp(tokenBuff, "rotylimit") == 0)
		{
			rotY->setMin(fileToken->GetFloat());
			rotY->setMax(fileToken->GetFloat());
		}
		else if (strcmp(tokenBuff, "rotzlimit") == 0)
		{
			rotZ->setMin(fileToken->GetFloat());
			rotZ->setMax(fileToken->GetFloat());
		}
		else if (strcmp(tokenBuff, "pose") == 0)
		{
			rotX->setValue(fileToken->GetFloat());
			rotY->setValue(fileToken->GetFloat());
			rotZ->setValue(fileToken->GetFloat());
		}
		else if (strcmp(tokenBuff, "balljoint") == 0)
		{
			children.push_back(new BallJoint());
			children.back()->setParent(this);
			children.back()->load(fileToken);
		}
		else if (strcmp(tokenBuff, "}") == 0)
		{
			break;
		}
	}

	cube = new Cube(boxmin, boxmax);
}

void BallJoint::update(const glm::mat4& parent2World)
{
	loc2Parent = glm::eulerAngleX(rotX->getValue());
	loc2Parent = glm::eulerAngleY(rotY->getValue()) * loc2Parent;
	loc2Parent = glm::eulerAngleZ(rotZ->getValue()) * loc2Parent;
	loc2Parent = glm::translate(offset) * loc2Parent;

	loc2World = parent2World * loc2Parent;

	for (BallJoint* child : children)
	{
		child->update(loc2World);
	}
}

void BallJoint::draw(const glm::mat4& viewProjMtx, GLuint shader)
{
	cube->draw(viewProjMtx * loc2World, shader);

	for (BallJoint* child : children)
	{
		child->draw(viewProjMtx, shader);
	}
}

void BallJoint::addChild(BallJoint* child)
{
	children.push_back(child);
	child->setParent(this);
}