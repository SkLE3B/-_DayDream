#include "Fader.h"

void Fader::Fadein(float fadeValue)
{
	valueAdd(fadeValue);
}

void Fader::Fadeout(float fadeValue)
{
	valueMin(fadeValue);
}

void Fader::set(float value)
{
	value = min(value, maxValue);
	value = max(value, minValue);
	preValue = value;
}
