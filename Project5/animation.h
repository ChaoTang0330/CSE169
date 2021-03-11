#pragma once

#include "channel.h"
#include "skeleton.h"

class Animation
{
private:
	std::string fileName;
	float time_start, time_end;

	std::vector<Channel*> channels;

public:
	Animation(std::string _filename);
	~Animation();

	void load();
	void update(float& t, glm::mat4& T, Skeleton* skel);
};
