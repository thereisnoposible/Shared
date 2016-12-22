#pragma once
#include <math.h>

namespace Math
{
	class Vector3
	{
	public:
		Vector3(){};
		Vector3(float x_, float y_, float z_){ x = x_; y = y_; z = z_; }
		float x;
		float y;
		float z;

		Vector3 operator- (const Vector3& v) const
		{
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		Vector3 operator- () const
		{
			return Vector3(-x, -y, -z);
		}

		Vector3 operator+ (const Vector3& v) const
		{
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		float length()
		{
			return sqrtf(x*x + y*y + z*z);
		}
	};
}