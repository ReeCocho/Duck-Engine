#include "math\common.hpp"

int main()
{
	// vec2 tests
	{
		dk_log("VEC2 TESTS");

		dk::Vec2 v1 = { 1, 2 };
		dk::Vec2 v2 = { 3, 4 };
		dk::Vec2 v3 = { 0, 0 };
		
		v3 = v1 + v2; // (4, 6)
		dk_log(v3.to_string());

		v3 = v1 - v2; // (-2, -2)
		dk_log(v3.to_string());

		v3 = v1 * v2; // (3, 8)
		dk_log(v3.to_string());

		v3 = v1 / v2; // (.333, .5)
		dk_log(v3.to_string());

		v3 = v2;
		v3 += v1; // (4, 6)
		dk_log(v3.to_string());

		v3 -= v1; // (3, 4)
		dk_log(v3.to_string());
		
		v3 *= v1; // (3, 8)
		dk_log(v3.to_string());

		v3 /= v1; // (3, 4)
		dk_log(v3.to_string() << '\n');
	}

	// vec3 tests
	{
		dk_log("VEC3 TESTS");

		dk::Vec3 v1 = { 1, 2, 3 };
		dk::Vec3 v2 = { 4, 5, 6 };
		dk::Vec3 v3 = { 0, 0, 0 };

		v3 = v1 + v2; // (5, 7, 9)
		dk_log(v3.to_string());

		v3 = v1 - v2; // (-3, -3, -3)
		dk_log(v3.to_string());

		v3 = v1 * v2; // (4, 10, 18)
		dk_log(v3.to_string());

		v3 = v1 / v2; // (.25, .4, .5)
		dk_log(v3.to_string());

		v3 = v2;
		v3 += v1; // (5, 7, 9)
		dk_log(v3.to_string());

		v3 -= v1; // (4, 5, 6)
		dk_log(v3.to_string());

		v3 *= v1; // (4, 10, 18)
		dk_log(v3.to_string());

		v3 /= v1; // (4, 5, 6)
		dk_log(v3.to_string() << '\n');
	}

	// vec4 tests
	{
		dk_log("VEC4 TESTS");

		dk::Vec4 v1 = { 1, 2, 3, 4 };
		dk::Vec4 v2 = { 5, 6, 7, 8 };
		dk::Vec4 v3 = { 0, 0, 0, 0 };

		v3 = v1 + v2; // (6, 8, 10, 12)
		dk_log(v3.to_string());

		v3 = v1 - v2; // (-4, -4, -4, -4)
		dk_log(v3.to_string());

		v3 = v1 * v2; // (5, 12, 21, 32)
		dk_log(v3.to_string());

		v3 = v1 / v2; // (.2, .333, .428571, .5)
		dk_log(v3.to_string());

		v3 = v2;
		v3 += v1; // (6, 8, 10, 12)
		dk_log(v3.to_string());

		v3 -= v1; // (5, 6, 7, 8)
		dk_log(v3.to_string());

		v3 *= v1; // (5, 12, 21, 32)
		dk_log(v3.to_string());

		v3 /= v1; // (5, 6, 7, 8)
		dk_log(v3.to_string() << '\n');
	}

	// mat4 tests
	{
		dk_log("MAT4 TESTS");

		dk::Mat4 m1 = 
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		dk::Mat4 m2 = 
		{
			 1,  2,  8,  4,
			 5, 90,  7,  8,
			 9, 10, 11, 12,
			13, 14,  1, 16
		};

		dk::Mat4 m3(0);

		m3 = m1 + m2;
		dk_log(m3.to_string() << '\n');

		m3 = m1 - m2;
		dk_log(m3.to_string() << '\n');

		m3 = m2.inverse() * m2;
		dk_log(m3.to_string() << '\n');
	}

	// mat3 tests
	{
		dk_log("MAT3 TESTS");

		dk::Mat3 m1 =
		{
			1, 0, 0,
			0, 1, 0,
			0, 0, 1
		};

		dk::Mat3 m2 =
		{
			1, 2, 1,
			4, 5, 9,
			7, 8, 9
		};

		dk::Mat3 m3(0);

		m3 = m1 + m2;
		dk_log(m3.to_string() << '\n');

		m3 = m1 - m2;
		dk_log(m3.to_string() << '\n');

		m3 = m2 * m2.inverse();
		dk_log(m3.to_string() << '\n');
	}

	// mat2 tests
	{
		dk_log("MAT2 TESTS");

		dk::Mat2 m1 = 
		{
			1, 0,
			0, 1
		};

		dk::Mat2 m2 = 
		{
			1, 2,
			3, 4
		};

		dk::Mat2 m3(0);

		m3 = m1 + m2;
		dk_log(m3.to_string() << '\n');

		m3 = m1 - m2;
		dk_log(m3.to_string() << '\n');

		m3 = m1 * m2;
		dk_log(m3.to_string() << '\n');
	}

	// quaternion tests
	{
		dk_log("QUAT TESTS");

		dk::Quat q1(dk::Vec3(0.0f, 0.0f, 0.0f));
		dk::Quat q2(dk::Vec3(90.0f, 0.0f, 0.0f));
		dk::Quat q3(dk::Vec3(0.0f, 0.0f, 0.0f));

		dk::Vec3 euler = q2.as_euler();
		dk_log(euler.to_string());
	}

	std::cin.get();
	return 0;
}