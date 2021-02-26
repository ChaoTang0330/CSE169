#include "player.h"

Player::Player(Skeleton* _skel, Animation* _anim)
{
	skel = _skel;
	anim = _anim;
	time = 0.0f;

	prevT = clock();
}

void Player::update()
{
	clock_t currT = clock();
	time += ((float)currT - (float)prevT) / 1000.0f;
	prevT = currT;
	glm::mat4 T;

	anim->update(time, T, skel);
	skel->update(T);
}