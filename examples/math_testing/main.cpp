#include "math\common.hpp"

int main()
{
	// vec2 tests
	{
		dk_log("VEC2 TESTS");

		dk::vec_t<float, 2> v1 = { 1, 2 };
		dk::vec_t<float, 2> v2 = { 3, 4 };
		dk::vec_t<float, 2> v3 = { 0, 0 };
		
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

		dk::vec3 v1 = { 1, 2, 3 };
		dk::vec3 v2 = { 4, 5, 6 };
		dk::vec3 v3 = { 0, 0, 0 };

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

	std::cin.get();
	return 0;
}