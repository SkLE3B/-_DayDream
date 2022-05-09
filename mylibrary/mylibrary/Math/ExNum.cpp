#include "ExNum.h"

void ExNum::setMax(float value)
{
	mMax = std::max(mMin, value);
	if (mMax < get()) set(mMax);	
}

void ExNum::setMin(float value)
{
	mMin = std::min(value, mMax);
	if (get() < mMin) set(mMin);	
}

void ExNum::set(float value)
{
	value = std::min(value, mMax);
	value = std::max(value, mMin);
	mPre = value;	
}
