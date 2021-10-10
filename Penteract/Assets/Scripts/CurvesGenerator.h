#pragma once

#include "Math/float3.h"
#include "Math/Interpolate.h"

class CurvesGenerator
{
public:

	static float3 SmoothStepFloat3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStep(Clamp01(t)));
	}

	static float3 SmoothStep5Float3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStep5(Clamp01(t))); 
	}

	static float3 SmoothStep7Float3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStep7(Clamp01(t)));
	}

private:
	
	// Functions obtained from Math/interpolate.h
	static float SmoothStep(float t) {
		float tt = t * t;
		return 3.f * tt - 2.f * tt * t;
	}

	static float SmoothStep5(float t) {
		float tt = t * t;
		return tt * t * (6.f * tt - 15.f * t + 10.f);
	}

	static float SmoothStep7(float t) {
		float tt = t * t;
		float tttt = tt * tt;
		return tttt * (-20.f * tt * t + 70.f * tt - 84.f * t + 35.f);
	}
};

