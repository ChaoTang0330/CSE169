#pragma once

#include "core.h"

#include "Tokenizer.h"

class Keyframe
{
private:
	float time;
	float value;
	float tangent[2] = {0};
	int rule[2] = {0};

	float A, B, C, D;
	float deltaT;

public:
	static const int FLAT, LINEAR, SMOOTH, FIXED, NOT_DEFINE;

	Keyframe(Tokenizer* fileToken);
	~Keyframe();

	void iniTangent(Keyframe* prevFrame, Keyframe* nextFrame);
	void iniCubic(Keyframe* nextFrame);

	float getTime() { return time; }
	float getValue() { return value; }
	float getTangentIn() { return tangent[0]; }
	float getTangentOut() { return tangent[1]; }

	float evaluate(float t);
};