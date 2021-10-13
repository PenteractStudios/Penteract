#pragma once

#include "Math/float3.h"

class CurvesGenerator
{
public:

	static float3 SmoothStopFloat3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStop(Clamp01(t)));
	}

	static float3 SmoothStop3Float3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStop3(Clamp01(t)));
	}

	static float3 SmoothStop4Float3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStop4(Clamp01(t)));
	}

	static float3 SmoothStop5Float3(float3 min, float3 max, float t) {
		return float3::Lerp(min, max, SmoothStop5(Clamp01(t)));
	}

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

	// SMOOTHSTOP
	static float SmoothStop(float t) {
		float oneT = 1.f - t;
		return 1.f - oneT * oneT;
	}

	static float SmoothStop3(float t) {
		float oneT = 1.f - t;
		return 1.f - oneT * oneT * oneT;
	}

	static float SmoothStop4(float t) {
		float oneT = 1.f - t;
		oneT *= oneT;
		return 1.f - oneT * oneT;
	}

	static float SmoothStop5(float t) {
		float oneT = 1.f - t;
		float oneT2 = oneT * oneT;
		return 1.f - oneT2 * oneT2 * oneT;
	}

	// SMOOTHSTEP
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