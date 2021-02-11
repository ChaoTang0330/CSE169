#pragma once

#include "animation.h"
#include "skeleton.h"
#include <ctime>

class Player
{
private:
	Skeleton* skel;
	Animation* anim;

	clock_t prevT;
	float time;

	std::vector<BallJoint*> joints;

public:

	Player(Skeleton* _skel, Animation* _anim);

	void update();
};