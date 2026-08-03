#pragma once
#include <algorithm>
namespace Bloodforge::MathUtils
{
	inline float Lerp(float start, float end, float t)
	{
		t = std::clamp(t, 0.f, 1.0f);
		float smoothT = t * t * (3.0f - 2.0f * t);
		return start + (end - start) * smoothT;
	}

	inline float SmoothLerp(float start, float end, float t)
	{
		t = std::clamp(t, 0.f, 1.0f);
		return start + (end - start) * t;
	}
}