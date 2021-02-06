#pragma once

#include "joint.h"

class Skeleton
{
private:
	BallJoint* jointRoot;
	std::string fileName;

	glm::mat4 toWorld;
public:
	Skeleton(std::string _fileName, const glm::mat4& _toWorld);
	~Skeleton();

	void load();
	void update(const glm::mat4& _toWorld);
	void draw(const glm::mat4& viewProjMtx, GLuint shader);
};