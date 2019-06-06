#pragma once

#include "../valve_sdk/sdk.hpp"
#include <intrin.h>
#include <DirectXMath.h>

#define RAD2DEG(x) DirectX::XMConvertToDegrees(x)
#define DEG2RAD(x) DirectX::XMConvertToRadians(x)
#define M_PI 3.14159265358979323846
#define PI_F	((float)(M_PI))

#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TICKS_TO_TIME(t) (g_GlobalVars->interval_per_tick * (t) )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

namespace Math
{
	inline float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)&x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)&i;
	}

	//--------------------------------------------------------------------------------
	void FixAngles(QAngle& angles) 
	{
		NormalizeAngles(angles);
		ClampAngles(angles);
	}

	inline float NormalizeAngle(float flAng)
	{
		// rotate left by 180 degrees
		flAng = flAng + 180.0f;

		// normalize to -360..360 degrees
		flAng = fmod(flAng, 360.0f);
		if (flAng != flAng) // http://stackoverflow.com/a/570694
			// edge case error handling
			flAng = 0.0f;

		// rotate negative angles left by 360 degrees to get the positive equivalent
		if (flAng < 0.0f)
			flAng += 360.0f;

		// rotate right by 180 degrees
		flAng -= 180.0f;

		return flAng;
	}

	float VectorDistance(const Vector& v1, const Vector& v2);
	QAngle CalcAngle(const Vector& src, const Vector& dst);
	Vector CalcAngleV(const Vector& src, const Vector& dst);
	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle);
	float RandomFloat(float min, float max);
	void VectorAngles(const Vector& forward, Vector& up, QAngle& angles);
	inline float DotProduct(const Vector& a, const Vector& b)
	{
		return (a.x*b.x + a.y*b.y + a.z*b.z);
	}
	inline float DotProduct(const QAngle& a, const Vector& b)
	{
		return (a.pitch*b.x + a.yaw*b.y + a.roll*b.z);
	}
	Vector CrossProduct2(const Vector &a, const Vector &b);
	void NormalizeAngles(QAngle& angles);
	void NormalizePitch(float & pitch);
	int RandomInt(int min, int max);
	template<class T>
	void Normalize3(T& vec)
	{
		for (auto i = 0; i < 3; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] >  180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}
	
	void inline SinCos(float x, float* s, float* c)
	{
		__asm
		{
			fld dword ptr[x]
			fsincos
			mov edx, dword ptr[c]
			mov eax, dword ptr[s]
			fstp dword ptr[edx]
			fstp dword ptr[eax]
		}
	}
	void AngleVector(const Vector &angles, Vector& forward);

    void ClampAngles(QAngle& angles);
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
    void AngleVectors(const QAngle &angles, Vector& forward);
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up);
    void VectorAngles(const Vector& forward, QAngle& angles);
    bool WorldToScreen(const Vector& in, Vector& out);

	//float TicksToTime(int tick);

	template <class T, class U>
	static T Clamp(T in, U low, U high) 
	{
		if (in <= low)
			return low;

		if (in >= high)
			return high;

		return in;
	}


	// SIMD Math

	template <typename T = float>
	static T Minimum(const T & a, const T & b)
	{
		// check type.
		static_assert(std::is_arithmetic<T>::value, "Math::Minimum only supports integral types.");
		return (T)_mm_cvtss_f32(
			_mm_min_ss(_mm_set_ss((float)a),
				_mm_set_ss((float)b))
		);
	}

	template <typename T = float>
	static T Maximum(const T & a, const T & b) 
	{
		// check type.
		static_assert(std::is_arithmetic<T>::value, "Math::Maximum only supports integral types.");
		return (T)_mm_cvtss_f32(
			_mm_max_ss(_mm_set_ss((float)a),
				_mm_set_ss((float)b))
		);
	}
}