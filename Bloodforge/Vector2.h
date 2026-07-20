#pragma once
#include <numbers>

namespace Bloodforge
{
	struct Vector2
	{
		float X = 0.0f;
		float Y = 0.0f;

		float Magnitude() const;
		float SqrMagnitude() const;
		void Normalize();
		Vector2 Normalized() const;
		// Pass the angle in degrees
		Vector2 Rotate(float angle) const;
		Vector2 GetPerpendicular() const;

		Vector2 operator+(const Vector2 & obj) const;
		Vector2 operator-(const Vector2 & obj) const;
		Vector2 operator*(float scalar) const;
		Vector2 operator/(float scalar) const;

		Vector2& operator+=(const Vector2& obj);
		Vector2& operator-=(const Vector2& obj);
		Vector2& operator*=(float scalar);
		Vector2& operator/=(float scalar);

		bool operator==(const Vector2& obj) const;

		static float Cross(const Vector2& vec1, const Vector2& vec2);
		static float Dot(const Vector2& vec1, const Vector2& vec2);
		static Vector2 Multiply(const Vector2& vec1, const Vector2& vec2);
		static Vector2 Divide(const Vector2& vec1, const Vector2& vec2);

		// Returns degrees between 0 and 180
		static float Angle(const Vector2& vec1, const Vector2& vec2);
		// Returns degrees between -180 and 180
		static float SignedAngle(const Vector2& vec1, const Vector2& vec2);

		// Interpolates between start and end by t
		static Vector2 Lerp(const Vector2& start, const Vector2& end, float t);
		// Interpolates between start and end by t, using smoothing at the start and end of the interpolation
		static Vector2 SmoothLerp(const Vector2& start, const Vector2& end, float t);
		// Interpolates between start and end by t, using smoothing at the end of the interpolation
		static Vector2 SmoothEndLerp(const Vector2& start, const Vector2& end, float t);

		static constexpr float RadiansToDegrees = 180.0f / std::numbers::pi_v<float>;
		static constexpr float DegreesToRadians = std::numbers::pi_v<float> / 180.0f;
	};
}