#include "quaternions.hpp"

namespace dk
{
	Quat::Quat() : x(0), y(0), z(0), w(0) {}

	Quat::Quat(float x_a, float y_a, float z_a, float w_a) : x(x_a), y(y_a), z(z_a), w(w_a) {}

	Quat::Quat(const Quat& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	Quat::Quat(const Vec_t<3>& euler) : x(0), y(0), z(0), w(0)
	{
		euler_angles(euler);
	}

	Quat::Quat(const Vec_t<4>& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

	bool Quat::operator==(const Quat& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool Quat::operator==(const Vec_t<4>& vec) const
	{
		return x == vec.x && y == vec.y && z == vec.z && w == vec.w;
	}

	bool Quat::operator!=(const Quat& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	bool Quat::operator!=(const Vec_t<4>& vec) const
	{
		return x != vec.x || y != vec.y || z != vec.z || w != vec.w;
	}

	Quat& Quat::operator=(const Quat& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}

	Quat& Quat::operator=(const Vec_t<4>& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.w;
		return *this;
	}

	float Quat::magnitude() const
	{
		return static_cast<float>(std::sqrt(square_magnitude()));
	}

	Mat_t<4, 4> Quat::as_matrix() const
	{
		const float ys = y * y;
		const float zs = z * z;
		const float ws = w * w;

		return 
		{
			1.0f - (2.0f * (ys * ws)), 2.0f * ((y * z) - (w * x)), 2.0f * ((y * w) + (y * x)), 0.0f,
			2.0f * ((y * z) + (w * x)), 1.0f - (2.0f * (ys + ws)), 2.0f * ((z * w) - (y * x)), 0.0f,
			2.0f * ((y * w) - (y * x)), 2.0f * ((z * w) + (y * x)), 1.0f - (2.0f * (ys * zs)), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	Quat& Quat::euler_angles(const Vec_t<3>& euler)
	{
		const float cy = static_cast<float>(std::cos(euler.y * DUCK_RAD_CONST * 0.5f));
		const float sy = static_cast<float>(std::sin(euler.y * DUCK_RAD_CONST * 0.5f));
		const float cr = static_cast<float>(std::cos(euler.z * DUCK_RAD_CONST * 0.5f));
		const float sr = static_cast<float>(std::sin(euler.z * DUCK_RAD_CONST * 0.5f));
		const float cp = static_cast<float>(std::cos(euler.x * DUCK_RAD_CONST * 0.5f));
		const float sp = static_cast<float>(std::sin(euler.x * DUCK_RAD_CONST * 0.5f));

		w = cy * cr * cp + sy * sr * sp;
		x = cy * sr * cp - sy * cr * sp;
		y = cy * cr * sp + sy * sr * cp;
		z = sy * cr * cp - cy * sr * sp;
		return *this;
}

	Vec_t<3> Quat::as_euler() const
	{
		Vec_t<3> euler(0);

		// roll (z-axis rotation)
		const float sinr = 2.0f * (w * x + y * z);
		const float cosr = 1.0f - (2.0f * (x * x + y * y));
		euler.z = std::atan2(sinr, cosr);

		// pitch (x-axis rotation)
		const float sinp = 2.0f * (w * y - z * x);
		if (std::fabs(sinp) >= 1.0f)
			euler.x = std::copysign(DUCK_PI / 2.0f, sinp); // use 90 degrees if out of range
		else
			euler.x = std::asin(sinp);

		// yaw (y-axis rotation)
		const float siny = 2.0f * (w * z + x * y);
		const float cosy = 1.0f - (2.0f * (y * y + z * z));
		euler.y = std::atan2(siny, cosy);
		euler /= DUCK_RAD_CONST;

		return euler;
	}
}

#if DUCK_USE_SIMD
namespace dk
{
	Quat Quat::operator+(const Quat& other) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Quat Quat::operator+(const Vec_t<4>& vec) const
	{
		__m128 v1 = _mm_add_ps({ x, y, z, w }, { vec.x, vec.y, vec.z, vec.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Quat Quat::operator-(const Quat& other) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Quat Quat::operator-(const Vec_t<4>& vec) const
	{
		__m128 v1 = _mm_sub_ps({ x, y, z, w }, { vec.x, vec.y, vec.z, vec.w });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Quat Quat::operator*(const Quat& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, x, x, x }, { other.x, other.y, other.z, other.w });
		__m128 v2 = _mm_mul_ps({ y, y, y, y }, { other.y, other.x, other.w, other.z });
		__m128 v3 = _mm_mul_ps({ z, z, z, z }, { other.z, other.w, other.x, other.y });
		__m128 v4 = _mm_mul_ps({ w, w, w, w }, { other.w, other.z, other.y, other.x });

		return
		{
			v1.m128_f32[0] - v2.m128_f32[0] - v3.m128_f32[0] - v4.m128_f32[0],
			v1.m128_f32[1] + v2.m128_f32[1] + v3.m128_f32[1] - v4.m128_f32[1],
			v1.m128_f32[2] - v2.m128_f32[2] + v3.m128_f32[2] + v4.m128_f32[2],
			v1.m128_f32[3] + v2.m128_f32[3] - v3.m128_f32[3] + v4.m128_f32[3]
		};
	}

	Quat Quat::operator*(const Vec_t<4>& vec) const
	{
		__m128 v1 = _mm_mul_ps({ x, x, x, x }, { vec.x, vec.y, vec.z, vec.w });
		__m128 v2 = _mm_mul_ps({ y, y, y, y }, { vec.y, vec.x, vec.w, vec.z });
		__m128 v3 = _mm_mul_ps({ z, z, z, z }, { vec.z, vec.w, vec.x, vec.y });
		__m128 v4 = _mm_mul_ps({ w, w, w, w }, { vec.w, vec.z, vec.y, vec.x });

		return
		{
			v1.m128_f32[0] - v2.m128_f32[0] - v3.m128_f32[0] - v4.m128_f32[0],
			v1.m128_f32[1] + v2.m128_f32[1] + v3.m128_f32[1] - v4.m128_f32[1],
			v1.m128_f32[2] - v2.m128_f32[2] + v3.m128_f32[2] + v4.m128_f32[2],
			v1.m128_f32[3] + v2.m128_f32[3] - v3.m128_f32[3] + v4.m128_f32[3]
		};
	}

	Quat Quat::operator*(float value) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { value, value, value, value });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Quat Quat::operator/(float value) const
	{
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { value, value, value, value });
		return { v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3] };
	}

	Quat& Quat::operator+=(const Quat& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	Quat& Quat::operator+=(const Vec_t<4>& vec)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_add_ps(*v1, { vec.x, vec.y, vec.z, vec.w });
		return *this;
	}

	Quat& Quat::operator-=(const Quat& other)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { other.x, other.y, other.z, other.w });
		return *this;
	}

	Quat& Quat::operator-=(const Vec_t<4>& vec)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_sub_ps(*v1, { vec.x, vec.y, vec.z, vec.w });
		return *this;
	}

	Quat& Quat::operator*=(const Quat& other)
	{
		__m128 v1 = _mm_mul_ps({ x, x, x, x }, { other.x, other.y, other.z, other.w });
		__m128 v2 = _mm_mul_ps({ y, y, y, y }, { other.y, other.x, other.w, other.z });
		__m128 v3 = _mm_mul_ps({ z, z, z, z }, { other.z, other.w, other.x, other.y });
		__m128 v4 = _mm_mul_ps({ w, w, w, w }, { other.w, other.z, other.y, other.x });

		x = v1.m128_f32[0] - v2.m128_f32[0] - v3.m128_f32[0] - v4.m128_f32[0];
		y = v1.m128_f32[1] + v2.m128_f32[1] + v3.m128_f32[1] - v4.m128_f32[1];
		z = v1.m128_f32[2] - v2.m128_f32[2] + v3.m128_f32[2] + v4.m128_f32[2];
		w = v1.m128_f32[3] + v2.m128_f32[3] - v3.m128_f32[3] + v4.m128_f32[3];
		return *this;
	}

	Quat& Quat::operator*=(const Vec_t<4>& vec)
	{
		__m128 v1 = _mm_mul_ps({ x, x, x, x }, { vec.x, vec.y, vec.z, vec.w });
		__m128 v2 = _mm_mul_ps({ y, y, y, y }, { vec.y, vec.x, vec.w, vec.z });
		__m128 v3 = _mm_mul_ps({ z, z, z, z }, { vec.z, vec.w, vec.x, vec.y });
		__m128 v4 = _mm_mul_ps({ w, w, w, w }, { vec.w, vec.z, vec.y, vec.x });

		x = v1.m128_f32[0] - v2.m128_f32[0] - v3.m128_f32[0] - v4.m128_f32[0];
		y = v1.m128_f32[1] + v2.m128_f32[1] + v3.m128_f32[1] - v4.m128_f32[1];
		z = v1.m128_f32[2] - v2.m128_f32[2] + v3.m128_f32[2] + v4.m128_f32[2];
		w = v1.m128_f32[3] + v2.m128_f32[3] - v3.m128_f32[3] + v4.m128_f32[3];
		return *this;
	}

	Quat& Quat::operator*=(float value)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_mul_ps(*v1, { value, value, value, value });
		return *this;
	}

	Quat& Quat::operator/=(float value)
	{
		__m128* v1 = reinterpret_cast<__m128*>(data);
		*v1 = _mm_div_ps(*v1, { value, value, value, value });
		return *this;
	}

	float Quat::square_magnitude() const
	{
		__m128 v1 = { x, y, z, w };
		v1 = _mm_mul_ps(v1, v1);
		return v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3];
	}

	Quat& Quat::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		__m128* v1 = reinterpret_cast<__m128*>(data);
		__m128 v2 = { mag, mag, mag, mag };
		*v1 = _mm_div_ps(*v1, v2);
		return *this;
	}

	Quat Quat::normalized() const
	{
		float mag = magnitude();
		if (mag == 0.0f) return Quat();
		__m128 v1 = _mm_div_ps({ x, y, z, w }, { mag, mag, mag, mag });
		return Quat(v1.m128_f32[0], v1.m128_f32[1], v1.m128_f32[2], v1.m128_f32[3]);
	}

	float Quat::dot(const Quat& other) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { other.x, other.y, other.z, other.w });
		return v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3];
	}

	float Quat::dot(const Vec_t<4>& vec) const
	{
		__m128 v1 = _mm_mul_ps({ x, y, z, w }, { vec.x, vec.y, vec.z, vec.w });
		return v1.m128_f32[0] + v1.m128_f32[1] + v1.m128_f32[2] + v1.m128_f32[3];
	}
}
#else
namespace dk
{
	Quat Quat::operator+(const Quat& other) const
	{
		Quat quat = *this;
		quat.x += other.x;
		quat.y += other.y;
		quat.z += other.z;
		quat.w += other.w;
		return quat;
	}

	Quat Quat::operator+(const Vec_t<4>& vec) const
	{
		Quat quat = *this;
		quat.x += vec.x;
		quat.y += vec.y;
		quat.z += vec.z;
		quat.w += vec.w;
		return quat;
	}

	Quat Quat::operator-(const Quat& other) const
	{
		Quat quat = *this;
		quat.x -= other.x;
		quat.y -= other.y;
		quat.z -= other.z;
		quat.w -= other.w;
		return quat;
	}

	Quat Quat::operator-(const Vec_t<4>& vec) const
	{
		Quat quat = *this;
		quat.x -= vec.x;
		quat.y -= vec.y;
		quat.z -= vec.z;
		quat.w -= vec.w;
		return quat;
	}

	Quat Quat::operator*(const Quat& other) const
	{
		return 
		{
			(x * other.x) - (y * other.y) - (z * other.z) - (w * other.w),
			(x * other.y) + (y * other.x) + (z * other.w) - (w * other.z),
			(x * other.z) - (y * other.w) + (z * other.x) + (w * other.y),
			(x * other.w) + (y * other.z) - (z * other.y) + (w * other.x)
		};
	}

	Quat Quat::operator*(const Vec_t<4>& vec) const
	{
		return
		{
			(x * vec.x) - (y * vec.y) - (z * vec.z) - (w * vec.w),
			(x * vec.y) + (y * vec.x) + (z * vec.w) - (w * vec.z),
			(x * vec.z) - (y * vec.w) + (z * vec.x) + (w * vec.y),
			(x * vec.w) + (y * vec.z) - (z * vec.y) + (w * vec.x)
		};
	}

	Quat Quat::operator*(float value) const
	{
		Quat quat = *this;
		quat.x *= value;
		quat.y *= value;
		quat.z *= value;
		quat.w *= value;
		return quat;
	}

	Quat Quat::operator/(float value) const
	{
		Quat quat = *this;
		quat.x /= value;
		quat.y /= value;
		quat.z /= value;
		quat.w /= value;
		return quat;
	}

	Quat& Quat::operator+=(const Quat& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	Quat& Quat::operator+=(const Vec_t<4>& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;
		return *this;
	}

	Quat& Quat::operator-=(const Quat& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}

	Quat& Quat::operator-=(const Vec_t<4>& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;
		return *this;
	}

	Quat& Quat::operator*=(const Quat& other)
	{
		x = (x * other.x) - (y * other.y) - (z * other.z) - (w * other.w);
		y = (x * other.y) + (y * other.x) + (z * other.w) - (w * other.z);
		z = (x * other.z) - (y * other.w) + (z * other.x) + (w * other.y);
		w = (x * other.w) + (y * other.z) - (z * other.y) + (w * other.x);
		return *this;
	}

	Quat& Quat::operator*=(const Vec_t<4>& vec)
	{
		x = (x * vec.x) - (y * vec.y) - (z * vec.z) - (w * vec.w);
		y = (x * vec.y) + (y * vec.x) + (z * vec.w) - (w * vec.z);
		z = (x * vec.z) - (y * vec.w) + (z * vec.x) + (w * vec.y);
		w = (x * vec.w) + (y * vec.z) - (z * vec.y) + (w * vec.x);
		return *this;
	}

	Quat& Quat::operator*=(float value)
	{
		x *= value;
		y *= value;
		z *= value;
		w *= value;
		return *this;
	}

	Quat& Quat::operator/=(float value)
	{
		x /= value;
		y /= value;
		z /= value;
		w /= value;
		return *this;
	}

	float Quat::square_magnitude() const
	{
		return (x*x) + (y*y) + (z*z) + (w*w);
	}

	Quat& Quat::normalize()
	{
		float mag = magnitude();
		if (mag == 0.0f) return *this;
		x /= mag;
		y /= mag;
		z /= mag;
		w /= mag;
		return *this;
	}

	Quat Quat::normalized() const
	{
		Quat quat = *this;
		float mag = magnitude();
		if (mag == 0.0f) return quat;
		quat.x /= mag;
		quat.y /= mag;
		quat.z /= mag;
		quat.w /= mag;
		return quat;
	}

	float Quat::dot(const Quat& other) const
	{
		return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
	}

	float Quat::dot(const Vec_t<4>& vec) const
	{
		return (x * vec.x) + (y * vec.y) + (z * vec.z) + (w * vec.w);
	}
}
#endif