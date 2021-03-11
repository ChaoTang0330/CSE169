#pragma once

#include "core.h"

class DOF
{
private:
	float min = -360;
	float max = 260;
	float value = 0;
	
public:
	DOF() {}
	DOF(float _min, float _max)
		:min(_min), max(_max){}
	~DOF() {}

	void setValue(float _value) 
	{ 
		value = _value;
		value = std::max(min, value);
		value = std::min(max, value);
	}

	float getValue() { return value; }
	void setMin(float _min) { min = _min; }
	float getMin() { return min; }
	float getMax() { return max; }
	void setMax(float _max) { max = _max; }
};