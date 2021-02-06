#pragma once

#include "DOF.h"
#include "Tokenizer.h"
#include "Cube.h"

class BallJoint
{
private:
	glm::vec3 offset, boxmax, boxmin;
	glm::mat4 loc2Parent = glm::mat4(1.0);
	glm::mat4 loc2World = glm::mat4(1.0);

	DOF* rotX, * rotY, * rotZ;

	std::vector<BallJoint*> children;
	BallJoint* parent = NULL;

	std::string jointName;

	Cube* cube;

public:
	BallJoint();
	~BallJoint();

	void load(Tokenizer* fileToken);
	void update(const glm::mat4& toWorld);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);

	void addChild(BallJoint* child);
	void setParent(BallJoint* _parent) { parent = _parent; }
};