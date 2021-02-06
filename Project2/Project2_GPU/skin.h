#pragma once
#include "core.h"
#include "skeleton.h"

class Skin
{
private:
	std::string fileName;
	GLuint shaderID;
	GLuint vao, ebo;
#ifdef SHADER_SKINING
	GLuint vbos[5];
#else
	GLuint vbos[2];
#endif

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	//std::vector<glm::vec3> currPositions;
	//std::vector<glm::vec3> currNormals;

	std::vector<glm::ivec3> triangles;
	std::vector<std::vector<std::pair<int, float>>> skinweights;
	std::vector<GLint> skinWeightNum;
	std::vector<glm::ivec4> skinJointsIdx;
	std::vector<glm::vec4> skinJointsWeight;

	std::vector<glm::mat4> bindings;
	std::vector<glm::mat4> invBindings;
	std::vector<glm::mat4> posMat;
	std::vector<glm::mat4> normMat;

	Skeleton* skel;

public:
	Skin(std::string _fileName, Skeleton* _skel, GLint _programID);
	~Skin();

	void load();
	void ckeckNormals();
	void invBindingMat();
	void setSkeleton(Skeleton* _skel) { skel = _skel; }
	void update();
	void draw(const glm::mat4& viewProjMtx);
};