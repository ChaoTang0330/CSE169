#pragma once

#include "joint.h"

class Skeleton
{
private:
	BallJoint* jointRoot;
	std::string fileName;

	glm::mat4 toWorld;
	std::vector<BallJoint*> joints;

	GLuint shaderID;
public:
	Skeleton(std::string _fileName, GLuint _shaderID);
	~Skeleton();

	void load();
	void update(const glm::mat4& _toWorld);
	void draw(const glm::mat4& viewProjMtx);

	glm::mat4 getWorldMat(int jointNum);

	size_t getNumOfJoint() { return joints.size(); }

	std::string getJointName(int jointNum);
	void getJointRot(int jointNum, glm::vec3& rot);
	void getJointRotMin(int jointNum, glm::vec3& rotMin);
	void getJointRotMax(int jointNum, glm::vec3& rotMax);
	void setJointRot(int jointNum, glm::vec3& rot);
};