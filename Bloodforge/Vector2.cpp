#include "pch.h"
#include "Vector2.h"
#include <cmath>

namespace Bloodforge
{
    float Vector2::Magnitude() const
    {
		return sqrtf(SqrMagnitude());
    }

    float Vector2::SqrMagnitude() const
    {
        return X * X + Y * Y;
    }

    void Vector2::Normalize()
    {
        float magnitude = Magnitude();
		X /= magnitude;
        Y /= magnitude;
    }

    Vector2 Vector2::Normalized() const
    {
        float magnitude = Magnitude();
        return
        {
            X / magnitude,
            Y / magnitude,
        };
    }

    Vector2 Vector2::Rotate(float angle) const
    {
		float radians = angle * DegreesToRadians;
		float cos = std::cos(radians);
		float sin = std::sin(radians);

        return
        {
            X * cos - Y * sin,
            X * sin + Y * cos,
		};
    }

    Vector2 Vector2::GetPerpendicular() const
    {
        return 
        {
            -Y,
            X,
		};
    }

    Vector2 Vector2::operator+(const Vector2& obj) const
    {
        return
        {
            X + obj.X,
            Y + obj.Y
        };
    }

    Vector2 Vector2::operator-(const Vector2& obj) const
    {
        return
        {
            X - obj.X,
            Y - obj.Y
        };
    }

    Vector2 Vector2::operator*(float scalar) const
    {
        return
        {
            X *scalar,
            Y *scalar,
        };
    }

    Vector2 Vector2::operator/(float scalar) const
    {
        return
        {
            X / scalar,
            Y / scalar,
        };
    }

    Vector2& Vector2::operator+=(const Vector2& obj)
    {
        X += obj.X;
        Y += obj.Y;
        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& obj)
    {
        X -= obj.X;
        Y -= obj.Y;
        return *this;
    }

    Vector2& Vector2::operator*=(float scalar)
    {
        X *= scalar;
        Y *= scalar;
        return *this;
    }

    Vector2& Vector2::operator/=(float scalar)
    {
        X /= scalar;
        Y /= scalar;
        return *this;
    }

    bool Vector2::operator==(const Vector2& obj) const
    {
        return X == obj.X && Y == obj.Y;
    }

    float Vector2::Cross(const Vector2& vec1, const Vector2& vec2)
    {
        return vec1.X * vec2.Y - vec1.Y * vec2.X;
    }

    float Vector2::Dot(const Vector2& vec1, const Vector2& vec2)
    {
        return vec1.X * vec2.X + vec1.Y * vec2.Y;
    }

    Vector2 Vector2::Multiply(const Vector2& vec1, const Vector2& vec2)
    {
        return
        {
            vec1.X * vec2.X,
            vec1.Y * vec2.Y,
        };
    }

    Vector2 Vector2::Divide(const Vector2& vec1, const Vector2& vec2)
    {
        return
        {
            vec1.X / vec2.X,
            vec1.Y / vec2.Y,
        };
    }

    float Vector2::Angle(const Vector2& vec1, const Vector2& vec2)
    {
        float mag1 = vec1.Magnitude();
		if (mag1 == 0.0f) return 0.0f;
        float mag2 = vec2.Magnitude();
		if (mag2 == 0.0f) return 0.0f;

		float dot = Dot(vec1, vec2);
		float theta = dot / (mag1 * mag2);

		return acosf(theta) * RadiansToDegrees;
    }

    float Vector2::SignedAngle(const Vector2& vec1, const Vector2& vec2)
    {
        float dot = Dot(vec1, vec2);
        float cross = Cross(vec1, vec2);

        return std::atan2(cross, dot) * RadiansToDegrees;
    }

    Vector2 Vector2::Lerp(const Vector2& start, const Vector2& end, float t)
    {
        return start + (end - start) * t;
    }

    Vector2 Vector2::SmoothLerp(const Vector2& start, const Vector2& end, float t)
    {
        float smoothT = t * t * (3.0f - 2.0f * t);
        return start + (end - start) * smoothT;
    }

    Vector2 Vector2::SmoothEndLerp(const Vector2& start, const Vector2& end, float t)
    {
        float smoothT = 1.0f - (1.0f - t) * (1.0f - t);
        return start + (end - start) * smoothT;
    }
}