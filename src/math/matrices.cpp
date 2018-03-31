/**
 * @file matrices.cpp
 * @brief Duck math matrix source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "matrices.hpp"

namespace dk
{
	Mat_t<2, 2>::Mat_t()
	{
		data[0][0] = 1;
		data[0][1] = 0;
		data[1][0] = 0;
		data[1][1] = 1;
	}

	Mat_t<2, 2>::Mat_t(float val)
	{
		data[0][0] = val;
		data[1][0] = val;
		data[0][1] = val;
		data[1][1] = val;
	}

	Mat_t<2, 2>::Mat_t(float a, float b, float c, float d)
	{
		data[0][0] = a;
		data[0][1] = b;
		data[1][0] = c;
		data[1][1] = d;
	}

	Mat_t<2, 2>::Mat_t(const Mat_t<2, 2>& other)
	{
		data[0][0] = other.data[0][0];
		data[0][1] = other.data[1][0];
		data[1][0] = other.data[0][1];
		data[1][1] = other.data[1][1];
	}

	Row<float, 2> Mat_t<2, 2>::operator[](size_t index)
	{
		dk_assert(index < 2);
		return Row<float, 2>(data[index]);
	}

	Row<const float, 2> Mat_t<2, 2>::operator[](size_t index) const
	{
		dk_assert(index < 2);
		return Row<const float, 2>(data[index]);
	}

	std::string Mat_t<2, 2>::to_string() const
	{
		std::string str = "";

		for (size_t i = 0; i < 2; ++i)
		{
			str += "[";
			for (size_t j = 0; j < 2 - 1; ++j)
				str += std::to_string(data[i][j]) + ", ";

			str += std::to_string(data[i][2 - 1]) + "]\n";
		}
		str.resize(str.size() - 1);
		return str;
	}

	bool Mat_t<2, 2>::operator==(const Mat_t<2, 2>& other) const
	{
		return	data[0][0] == other.data[0][0] &&
			data[1][0] == other.data[1][0] &&
			data[0][1] == other.data[0][1] &&
			data[1][1] == other.data[1][1];
	}

	bool Mat_t<2, 2>::operator!=(const Mat_t<2, 2>& other) const
	{
		return !(*this == other);
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator=(const Mat_t<2, 2>& other)
	{
		data[0][0] = other.data[0][0];
		data[1][0] = other.data[1][0];
		data[0][1] = other.data[0][1];
		data[1][1] = other.data[1][1];
		return *this;
	}

	float Mat_t<2, 2>::determinant() const
	{
		return (data[0][0] * data[1][1]) - (data[1][0] * data[0][1]);
	}

	Mat_t<2, 2> Mat_t<2, 2>::inverse() const
	{
		Mat_t<2, 2> mat =
		{
			data[1][1], -data[0][1],
			-data[1][0],  data[0][0]
		};

		float det = determinant();
		dk_assert(det != 0.0f);
		mat /= det;

		return mat;
	}


	Mat_t<3, 3>::Mat_t()
	{
		data[0][0] = 1;
		data[0][1] = 0;
		data[0][2] = 0;

		data[1][0] = 0;
		data[1][1] = 1;
		data[1][2] = 0;

		data[2][0] = 0;
		data[2][1] = 0;
		data[2][2] = 1;
	}

	Mat_t<3, 3>::Mat_t(float val)
	{
		data[0][0] = val;
		data[0][1] = val;
		data[0][2] = val;

		data[1][0] = val;
		data[1][1] = val;
		data[1][2] = val;

		data[2][0] = val;
		data[2][1] = val;
		data[2][2] = val;
	}

	Mat_t<3, 3>::Mat_t(float a, float b, float c, float d, float e, float f, float g, float h, float i)
	{
		data[0][0] = a;
		data[0][1] = b;
		data[0][2] = c;

		data[1][0] = d;
		data[1][1] = e;
		data[1][2] = f;

		data[2][0] = g;
		data[2][1] = h;
		data[2][2] = i;
	}

	Mat_t<3, 3>::Mat_t(const Mat_t<3, 3>& other)
	{
		data[0][0] = other.data[0][0];
		data[0][1] = other.data[0][1];
		data[0][2] = other.data[0][2];

		data[1][0] = other.data[1][0];
		data[1][1] = other.data[1][1];
		data[1][2] = other.data[1][2];

		data[2][0] = other.data[2][0];
		data[2][1] = other.data[2][1];
		data[2][2] = other.data[2][2];
	}

	Row<float, 3> Mat_t<3, 3>::operator[](size_t index)
	{
		dk_assert(index < 3);
		return Row<float, 3>(data[index]);
	}

	Row<const float, 3> Mat_t<3, 3>::operator[](size_t index) const
	{
		dk_assert(index < 3);
		return Row<const float, 3>(data[index]);
	}

	std::string Mat_t<3, 3>::to_string() const
	{
		std::string str = "";

		for (size_t i = 0; i < 3; ++i)
		{
			str += "[";
			for (size_t j = 0; j < 3 - 1; ++j)
				str += std::to_string(data[i][j]) + ", ";

			str += std::to_string(data[i][3 - 1]) + "]\n";
		}
		str.resize(str.size() - 1);
		return str;
	}

	bool Mat_t<3, 3>::operator==(const Mat_t<3, 3>& other) const
	{
		return	data[0][0] == other.data[0][0] &&
			data[0][1] == other.data[0][1] &&
			data[0][2] == other.data[0][2] &&
			data[1][0] == other.data[1][0] &&
			data[1][1] == other.data[1][1] &&
			data[1][2] == other.data[1][2] &&
			data[2][0] == other.data[2][0] &&
			data[2][1] == other.data[2][1] &&
			data[2][2] == other.data[2][2];
	}

	bool Mat_t<3, 3>::operator!=(const Mat_t<3, 3>& other) const
	{
		return !(*this == other);
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator=(const Mat_t<3, 3>& other)
	{
		data[0][0] = other.data[0][0];
		data[0][1] = other.data[0][1];
		data[0][2] = other.data[0][2];
		data[1][0] = other.data[1][0];
		data[1][1] = other.data[1][1];
		data[1][2] = other.data[1][2];
		data[2][0] = other.data[2][0];
		data[2][1] = other.data[2][1];
		data[2][2] = other.data[2][2];
		return *this;
	}

	float Mat_t<3, 3>::determinant() const
	{
		return	(data[0][0] * data[1][1] * data[2][2]) + (data[0][1] * data[1][2] * data[2][0]) +
			(data[0][2] * data[1][0] * data[2][1]) - (data[0][2] * data[1][1] * data[2][0]) -
			(data[0][1] * data[1][0] * data[2][2]) - (data[0][0] * data[1][2] * data[2][1]);
	}

	Mat_t<3, 3> Mat_t<3, 3>::inverse() const
	{
		Mat_t<3, 3> mat =
		{
			(data[1][1] * data[2][2]) - (data[1][2] * data[2][1]), (data[0][2] * data[2][1]) - (data[0][1] * data[2][2]), (data[0][1] * data[1][2]) - (data[0][2] * data[1][1]),
			(data[1][2] * data[2][0]) - (data[1][0] * data[2][2]), (data[0][0] * data[2][2]) - (data[0][2] * data[2][0]), (data[0][2] * data[1][0]) - (data[0][0] * data[1][2]),
			(data[1][0] * data[2][1]) - (data[1][1] * data[2][0]), (data[0][1] * data[2][0]) - (data[0][0] * data[2][1]), (data[0][0] * data[1][1]) - (data[0][1] * data[1][0])
		};

		float det = determinant();
		dk_assert(det != 0);
		mat /= det;

		return mat;
	}



	Mat_t<4, 4>::Mat_t()
	{
		data[0][0] = 1;
		data[0][1] = 0;
		data[0][2] = 0;
		data[0][3] = 0;

		data[1][0] = 0;
		data[1][1] = 1;
		data[1][2] = 0;
		data[1][3] = 0;

		data[2][0] = 0;
		data[2][1] = 0;
		data[2][2] = 1;
		data[2][3] = 0;

		data[3][0] = 0;
		data[3][1] = 0;
		data[3][2] = 0;
		data[3][3] = 1;
	}

	Mat_t<4, 4>::Mat_t(float val)
	{
		data[0][0] = val;
		data[0][1] = val;
		data[0][2] = val;
		data[0][3] = val;

		data[1][0] = val;
		data[1][1] = val;
		data[1][2] = val;
		data[1][3] = val;

		data[2][0] = val;
		data[2][1] = val;
		data[2][2] = val;
		data[2][3] = val;

		data[3][0] = val;
		data[3][1] = val;
		data[3][2] = val;
		data[3][3] = val;
	}

	Mat_t<4, 4>::Mat_t(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p)
	{
		data[0][0] = a;
		data[0][1] = b;
		data[0][2] = c;
		data[0][3] = d;

		data[1][0] = e;
		data[1][1] = f;
		data[1][2] = g;
		data[1][3] = h;

		data[2][0] = i;
		data[2][1] = j;
		data[2][2] = k;
		data[2][3] = l;

		data[3][0] = m;
		data[3][1] = n;
		data[3][2] = o;
		data[3][3] = p;
	}

	Mat_t<4, 4>::Mat_t(const Mat_t<4, 4>& other)
	{
		data[0][0] = other.data[0][0];
		data[0][1] = other.data[0][1];
		data[0][2] = other.data[0][2];
		data[0][3] = other.data[0][3];

		data[1][0] = other.data[1][0];
		data[1][1] = other.data[1][1];
		data[1][2] = other.data[1][2];
		data[1][3] = other.data[1][3];

		data[2][0] = other.data[2][0];
		data[2][1] = other.data[2][1];
		data[2][2] = other.data[2][2];
		data[2][3] = other.data[2][3];

		data[3][0] = other.data[3][0];
		data[3][1] = other.data[3][1];
		data[3][2] = other.data[3][2];
		data[3][3] = other.data[3][3];
	}

	Row<float, 4> Mat_t<4, 4>::operator[](size_t index)
	{
		dk_assert(index < 4);
		return Row<float, 4>(data[index]);
	}

	Row<const float, 4> Mat_t<4, 4>::operator[](size_t index) const
	{
		dk_assert(index < 4);
		return Row<const float, 4>(data[index]);
	}

	std::string Mat_t<4, 4>::to_string() const
	{
		std::string str = "";

		for (size_t i = 0; i < 4; ++i)
		{
			str += "[";
			for (size_t j = 0; j < 4 - 1; ++j)
				str += std::to_string(data[i][j]) + ", ";

			str += std::to_string(data[i][4 - 1]) + "]\n";
		}
		str.resize(str.size() - 1);
		return str;
	}

	bool Mat_t<4, 4>::operator==(const Mat_t<4, 4>& other) const
	{
		return	data[0][0] == other.data[0][0] &&
			data[0][1] == other.data[0][1] &&
			data[0][2] == other.data[0][2] &&
			data[0][3] == other.data[0][3] &&
			data[1][0] == other.data[1][0] &&
			data[1][1] == other.data[1][1] &&
			data[1][2] == other.data[1][2] &&
			data[1][3] == other.data[1][3] &&
			data[2][0] == other.data[2][0] &&
			data[2][1] == other.data[2][1] &&
			data[2][2] == other.data[2][2] &&
			data[2][3] == other.data[2][3] &&
			data[3][0] == other.data[3][0] &&
			data[3][1] == other.data[3][1] &&
			data[3][2] == other.data[3][2] &&
			data[3][3] == other.data[3][3];
	}

	bool Mat_t<4, 4>::operator!=(const Mat_t<4, 4>& other) const
	{
		return !(*this == other);
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator=(const Mat_t<4, 4>& other)
	{
		data[0][0] = other.data[0][0];
		data[0][1] = other.data[0][1];
		data[0][2] = other.data[0][2];
		data[0][3] = other.data[0][3];
		data[1][0] = other.data[1][0];
		data[1][1] = other.data[1][1];
		data[1][2] = other.data[1][2];
		data[1][3] = other.data[1][3];
		data[2][0] = other.data[2][0];
		data[2][1] = other.data[2][1];
		data[2][2] = other.data[2][2];
		data[2][3] = other.data[2][3];
		data[3][0] = other.data[3][0];
		data[3][1] = other.data[3][1];
		data[3][2] = other.data[3][2];
		data[3][3] = other.data[3][3];
		return *this;
	}

	float Mat_t<4, 4>::determinant() const
	{
		return	(data[0][0] * data[1][1] * data[2][2] * data[3][3]) - (data[0][0] * data[1][1] * data[2][3] * data[3][2]) -
			(data[0][0] * data[1][2] * data[2][1] * data[3][3]) + (data[0][0] * data[1][2] * data[2][3] * data[3][1]) +
			(data[0][0] * data[1][3] * data[2][1] * data[3][2]) - (data[0][0] * data[1][3] * data[2][2] * data[3][1]) -
			(data[0][1] * data[1][0] * data[2][2] * data[3][3]) + (data[0][1] * data[1][0] * data[2][3] * data[3][2]) +
			(data[0][1] * data[1][2] * data[2][0] * data[3][3]) - (data[0][1] * data[1][2] * data[2][3] * data[3][1]) -
			(data[0][1] * data[1][3] * data[2][0] * data[3][2]) + (data[0][1] * data[1][3] * data[2][2] * data[3][1]) +
			(data[0][2] * data[1][0] * data[2][1] * data[3][3]) - (data[0][2] * data[1][0] * data[2][3] * data[3][1]) -
			(data[0][2] * data[1][1] * data[2][0] * data[3][3]) + (data[0][2] * data[1][1] * data[2][3] * data[3][0]) +
			(data[0][2] * data[1][3] * data[2][0] * data[3][1]) - (data[0][2] * data[1][3] * data[2][1] * data[3][0]) -
			(data[0][3] * data[1][0] * data[2][1] * data[3][2]) + (data[0][3] * data[1][0] * data[2][2] * data[3][1]) +
			(data[0][3] * data[1][1] * data[2][0] * data[3][2]) - (data[0][3] * data[1][1] * data[2][2] * data[3][0]) -
			(data[0][3] * data[1][2] * data[2][0] * data[3][1]) + (data[0][3] * data[1][2] * data[2][1] * data[3][0]);
	}

	Mat_t<4, 4> Mat_t<4, 4>::inverse() const
	{
		// 2x2 sub-determinants required to calculate 4x4 determinant
		const float det2_01_01 = data[0][0] * data[1][1] - data[0][1] * data[1][0];
		const float det2_01_02 = data[0][0] * data[1][2] - data[0][2] * data[1][0];
		const float det2_01_03 = data[0][0] * data[1][3] - data[0][3] * data[1][0];
		const float det2_01_12 = data[0][1] * data[1][2] - data[0][2] * data[1][1];
		const float det2_01_13 = data[0][1] * data[1][3] - data[0][3] * data[1][1];
		const float det2_01_23 = data[0][2] * data[1][3] - data[0][3] * data[1][2];

		// 3x3 sub-determinants required to calculate 4x4 determinant
		const float det3_201_012 = data[2][0] * det2_01_12 - data[2][1] * det2_01_02 + data[2][2] * det2_01_01;
		const float det3_201_013 = data[2][0] * det2_01_13 - data[2][1] * det2_01_03 + data[2][3] * det2_01_01;
		const float det3_201_023 = data[2][0] * det2_01_23 - data[2][2] * det2_01_03 + data[2][3] * det2_01_02;
		const float det3_201_123 = data[2][1] * det2_01_23 - data[2][2] * det2_01_13 + data[2][3] * det2_01_12;

		const float det = (-det3_201_123 * data[3][0] + det3_201_023 * data[3][1] - det3_201_013 * data[3][2] + det3_201_012 * data[3][3]);
		dk_assert(det != 0.0f);

		const float rcpDet = 1.0f / det;

		// remaining 2x2 sub-determinants
		const float det2_03_01 = data[0][0] * data[3][1] - data[0][1] * data[3][0];
		const float det2_03_02 = data[0][0] * data[3][2] - data[0][2] * data[3][0];
		const float det2_03_03 = data[0][0] * data[3][3] - data[0][3] * data[3][0];

		const float det2_03_12 = data[0][1] * data[3][2] - data[0][2] * data[3][1];
		const float det2_03_13 = data[0][1] * data[3][3] - data[0][3] * data[3][1];
		const float det2_03_23 = data[0][2] * data[3][3] - data[0][3] * data[3][2];

		const float det2_13_01 = data[1][0] * data[3][1] - data[1][1] * data[3][0];
		const float det2_13_02 = data[1][0] * data[3][2] - data[1][2] * data[3][0];
		const float det2_13_03 = data[1][0] * data[3][3] - data[1][3] * data[3][0];
		const float det2_13_12 = data[1][1] * data[3][2] - data[1][2] * data[3][1];
		const float det2_13_13 = data[1][1] * data[3][3] - data[1][3] * data[3][1];
		const float det2_13_23 = data[1][2] * data[3][3] - data[1][3] * data[3][2];

		// remaining 3x3 sub-determinants
		const float det3_203_012 = data[2][0] * det2_03_12 - data[2][1] * det2_03_02 + data[2][2] * det2_03_01;
		const float det3_203_013 = data[2][0] * det2_03_13 - data[2][1] * det2_03_03 + data[2][3] * det2_03_01;
		const float det3_203_023 = data[2][0] * det2_03_23 - data[2][2] * det2_03_03 + data[2][3] * det2_03_02;
		const float det3_203_123 = data[2][1] * det2_03_23 - data[2][2] * det2_03_13 + data[2][3] * det2_03_12;

		const float det3_213_012 = data[2][0] * det2_13_12 - data[2][1] * det2_13_02 + data[2][2] * det2_13_01;
		const float det3_213_013 = data[2][0] * det2_13_13 - data[2][1] * det2_13_03 + data[2][3] * det2_13_01;
		const float det3_213_023 = data[2][0] * det2_13_23 - data[2][2] * det2_13_03 + data[2][3] * det2_13_02;
		const float det3_213_123 = data[2][1] * det2_13_23 - data[2][2] * det2_13_13 + data[2][3] * det2_13_12;

		const float det3_301_012 = data[3][0] * det2_01_12 - data[3][1] * det2_01_02 + data[3][2] * det2_01_01;
		const float det3_301_013 = data[3][0] * det2_01_13 - data[3][1] * det2_01_03 + data[3][3] * det2_01_01;
		const float det3_301_023 = data[3][0] * det2_01_23 - data[3][2] * det2_01_03 + data[3][3] * det2_01_02;
		const float det3_301_123 = data[3][1] * det2_01_23 - data[3][2] * det2_01_13 + data[3][3] * det2_01_12;

		Mat_t<4, 4> m(0);

		m.data[0][0] = -det3_213_123 * rcpDet;
		m.data[1][0] = +det3_213_023 * rcpDet;
		m.data[2][0] = -det3_213_013 * rcpDet;
		m.data[3][0] = +det3_213_012 * rcpDet;

		m.data[0][1] = +det3_203_123 * rcpDet;
		m.data[1][1] = -det3_203_023 * rcpDet;
		m.data[2][1] = +det3_203_013 * rcpDet;
		m.data[3][1] = -det3_203_012 * rcpDet;

		m.data[0][2] = +det3_301_123 * rcpDet;
		m.data[1][2] = -det3_301_023 * rcpDet;
		m.data[2][2] = +det3_301_013 * rcpDet;
		m.data[3][2] = -det3_301_012 * rcpDet;

		m.data[0][3] = -det3_201_123 * rcpDet;
		m.data[1][3] = +det3_201_023 * rcpDet;
		m.data[2][3] = -det3_201_013 * rcpDet;
		m.data[3][3] = +det3_201_012 * rcpDet;

		return m;
	}
}

#if DUCK_USE_SIMD
namespace dk
{
	Mat_t<2, 2> Mat_t<2, 2>::operator+(const Mat_t<2, 2>& other) const
	{
		Mat_t<2, 2> mat = *this;
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_add_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator-(const Mat_t<2, 2>& other) const
	{
		Mat_t<2, 2> mat = *this;
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_sub_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	template<>
	Mat_t<2, 2> Mat_t<2, 2>::operator*(const Mat_t<2, 2>& other) const
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][0], data[0][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_mul_ps(m1, m2);

		Mat_t<2, 2> mat(0);
		mat.data[0][0] = m1.m128_f32[0] + m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2] + m1.m128_f32[3];

		m1 = { data[1][0], data[1][1], data[1][0], data[1][1] };
		m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_mul_ps(m1, m2);

		mat.data[1][0] = m1.m128_f32[0] + m1.m128_f32[1];
		mat.data[1][1] = m1.m128_f32[2] + m1.m128_f32[3];

		return mat;
	}

	Vec_t<2> Mat_t<2, 2>::operator*(const Vec_t<2>& vec) const
	{
		__m128 m1 = { data[0][0], data[0][1], data[1][0], data[1][1] };
		__m128 m2 = { vec.data[0], vec.data[1], vec.data[0], vec.data[1] };
		m1 = _mm_mul_ps(m1, m2);
		Vec_t<2> new_vec(0);
		new_vec.data[0] = m1.m128_f32[0] + m1.m128_f32[1];
		new_vec.data[1] = m1.m128_f32[2] + m1.m128_f32[3];
		return new_vec;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator*(float scalar) const
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		Mat_t<2, 2> mat = *this;
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator/(float scalar) const
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		Mat_t<2, 2> mat = *this;
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[1][0] = m1.m128_f32[1];
		mat.data[0][1] = m1.m128_f32[2];
		mat.data[1][1] = m1.m128_f32[3];
		return mat;
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator+=(const Mat_t<2, 2>& other)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_add_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator-=(const Mat_t<2, 2>& other)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { other.data[0][0], other.data[1][0], other.data[0][1], other.data[1][1] };
		m1 = _mm_sub_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator*=(const Mat_t<2, 2>& other)
	{
		*this = *this * other;
		return *this;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator*=(float scalar)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator/=(float scalar)
	{
		__m128 m1 = { data[0][0], data[1][0], data[0][1], data[1][1] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[1][0] = m1.m128_f32[1];
		data[0][1] = m1.m128_f32[2];
		data[1][1] = m1.m128_f32[3];
		return *this;
	}



	Mat_t<3, 3> Mat_t<3, 3>::operator+(const Mat_t<3, 3>& other) const
	{
		Mat_t<3, 3> mat = *this;
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { other.data[0][0], other.data[0][1], other.data[0][2], other.data[1][0] };
		m1 = _mm_add_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[0][1] = m1.m128_f32[1];
		mat.data[0][2] = m1.m128_f32[2];
		mat.data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { other.data[1][1], other.data[1][2], other.data[2][0], other.data[2][1] };
		m1 = _mm_add_ps(m1, m2);
		mat.data[1][1] = m1.m128_f32[0];
		mat.data[1][2] = m1.m128_f32[1];
		mat.data[2][0] = m1.m128_f32[2];
		mat.data[2][1] = m1.m128_f32[3];
		mat.data[2][2] += other.data[2][2];

		return mat;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator-(const Mat_t<3, 3>& other) const
	{
		Mat_t<3, 3> mat = *this;
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { other.data[0][0], other.data[0][1], other.data[0][2], other.data[1][0] };
		m1 = _mm_sub_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[0][1] = m1.m128_f32[1];
		mat.data[0][2] = m1.m128_f32[2];
		mat.data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { other.data[1][1], other.data[1][2], other.data[2][0], other.data[2][1] };
		m1 = _mm_sub_ps(m1, m2);
		mat.data[1][1] = m1.m128_f32[0];
		mat.data[1][2] = m1.m128_f32[1];
		mat.data[2][0] = m1.m128_f32[2];
		mat.data[2][1] = m1.m128_f32[3];
		mat.data[2][2] -= other.data[2][2];

		return mat;
	}

	template<>
	Mat_t<3, 3> Mat_t<3, 3>::operator*(const Mat_t<3, 3>& other) const
	{
		Mat_t<3, 3> mat(0);

		for (size_t i = 0; i < 3; ++i)
			for(size_t j = 0; j < 3; ++j)
			{
				__m128 m1 = { data[i][0], data[i][1], data[i][2], 0 };
				__m128 m2 = { other.data[0][j], other.data[1][j], other.data[2][j], 0 };
				m1 = _mm_mul_ps(m1, m2);
				mat.data[i][j] = m1.m128_f32[0] + m1.m128_f32[1] + m1.m128_f32[2];
			}

		return mat;
	}

	Vec_t<3> Mat_t<3, 3>::operator*(const Vec_t<3>& vec) const
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { vec.data[0], vec.data[1], vec.data[2], vec.data[0] };
		m1 = _mm_mul_ps(m1, m2);
		
		Vec_t<3> new_vec(0);
		new_vec.data[0] = m1.m128_f32[0] + m1.m128_f32[1] + m1.m128_f32[2];
		new_vec.data[1] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { vec.data[1], vec.data[2], vec.data[0], vec.data[1] };
		m1 = _mm_mul_ps(m1, m2);

		new_vec.data[1] += m1.m128_f32[0] + m1.m128_f32[1];
		new_vec.data[2] = m1.m128_f32[2] + m1.m128_f32[3] + (data[2][2] * vec.data[2]);

		return new_vec;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator*(float scalar) const
	{
		Mat_t<3, 3> mat = *this;
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[0][1] = m1.m128_f32[1];
		mat.data[0][2] = m1.m128_f32[2];
		mat.data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		mat.data[1][1] = m1.m128_f32[0];
		mat.data[1][2] = m1.m128_f32[1];
		mat.data[2][0] = m1.m128_f32[2];
		mat.data[2][1] = m1.m128_f32[3];
		mat.data[2][2] *= scalar;

		return mat;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator/(float scalar) const
	{
		Mat_t<3, 3> mat = *this;
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		mat.data[0][0] = m1.m128_f32[0];
		mat.data[0][1] = m1.m128_f32[1];
		mat.data[0][2] = m1.m128_f32[2];
		mat.data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		mat.data[1][1] = m1.m128_f32[0];
		mat.data[1][2] = m1.m128_f32[1];
		mat.data[2][0] = m1.m128_f32[2];
		mat.data[2][1] = m1.m128_f32[3];
		mat.data[2][2] /= scalar;

		return mat;
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator+=(const Mat_t<3, 3>& other)
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { other.data[0][0], other.data[0][1], other.data[0][2], other.data[1][0] };
		m1 = _mm_add_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[0][1] = m1.m128_f32[1];
		data[0][2] = m1.m128_f32[2];
		data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { other.data[1][1], other.data[1][2], other.data[2][0], other.data[2][1] };
		m1 = _mm_add_ps(m1, m2);
		data[1][1] = m1.m128_f32[0];
		data[1][2] = m1.m128_f32[1];
		data[2][0] = m1.m128_f32[2];
		data[2][1] = m1.m128_f32[3];
		data[2][2] += other.data[2][2];

		return *this;
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator-=(const Mat_t<3, 3>& other)
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { other.data[0][0], other.data[0][1], other.data[0][2], other.data[1][0] };
		m1 = _mm_sub_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[0][1] = m1.m128_f32[1];
		data[0][2] = m1.m128_f32[2];
		data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { other.data[1][1], other.data[1][2], other.data[2][0], other.data[2][1] };
		m1 = _mm_sub_ps(m1, m2);
		data[1][1] = m1.m128_f32[0];
		data[1][2] = m1.m128_f32[1];
		data[2][0] = m1.m128_f32[2];
		data[2][1] = m1.m128_f32[3];
		data[2][2] -= other.data[2][2];

		return *this;
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator*=(const Mat_t<3, 3>& other)
	{
		*this = *this * other;
		return *this;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator*=(float scalar)
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_mul_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[0][1] = m1.m128_f32[1];
		data[0][2] = m1.m128_f32[2];
		data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_add_ps(m1, m2);
		data[1][1] = m1.m128_f32[0];
		data[1][2] = m1.m128_f32[1];
		data[2][0] = m1.m128_f32[2];
		data[2][1] = m1.m128_f32[3];
		data[2][2] *= scalar;

		return *this;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator/=(float scalar)
	{
		__m128 m1 = { data[0][0], data[0][1], data[0][2], data[1][0] };
		__m128 m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		data[0][0] = m1.m128_f32[0];
		data[0][1] = m1.m128_f32[1];
		data[0][2] = m1.m128_f32[2];
		data[1][0] = m1.m128_f32[3];

		m1 = { data[1][1], data[1][2], data[2][0], data[2][1] };
		m2 = { scalar, scalar, scalar, scalar };
		m1 = _mm_div_ps(m1, m2);
		data[1][1] = m1.m128_f32[0];
		data[1][2] = m1.m128_f32[1];
		data[2][0] = m1.m128_f32[2];
		data[2][1] = m1.m128_f32[3];
		data[2][2] /= scalar;

		return *this;
	}



	Mat_t<4, 4> Mat_t<4, 4>::operator+(const Mat_t<4, 4>& other) const
	{
		Mat_t<4, 4> mat = *this;

		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { other.data[i][0], other.data[i][1], other.data[i][2], other.data[i][3] };
			m1 = _mm_add_ps(m1, m2);
			mat.data[i][0] = m1.m128_f32[0];
			mat.data[i][1] = m1.m128_f32[1];
			mat.data[i][2] = m1.m128_f32[2];
			mat.data[i][3] = m1.m128_f32[3];
		}

		return mat;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator-(const Mat_t<4, 4>& other) const
	{
		Mat_t<4, 4> mat = *this;

		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { other.data[i][0], other.data[i][1], other.data[i][2], other.data[i][3] };
			m1 = _mm_sub_ps(m1, m2);
			mat.data[i][0] = m1.m128_f32[0];
			mat.data[i][1] = m1.m128_f32[1];
			mat.data[i][2] = m1.m128_f32[2];
			mat.data[i][3] = m1.m128_f32[3];
		}

		return mat;
	}

	template<>
	Mat_t<4, 4> Mat_t<4, 4>::operator*(const Mat_t<4, 4>& other) const
	{
		Mat_t<4, 4> mat(0);

		for (size_t i = 0; i < 4; ++i)
			for(size_t j = 0; j < 4; ++j)
			{
				__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
				__m128 m2 = { other.data[0][j], other.data[1][j], other.data[2][j], other.data[3][j] };
				m1 = _mm_mul_ps(m1, m2);
				mat.data[i][j] = m1.m128_f32[0] + m1.m128_f32[1] + m1.m128_f32[2] + m1.m128_f32[3];
			}

		return mat;
	}

	Vec_t<4> Mat_t<4, 4>::operator*(const Vec_t<4>& vec) const
	{
		Vec_t<4> new_vec(0);

		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { vec.data[0], vec.data[1], vec.data[2], vec.data[3] };
			m1 = _mm_mul_ps(m1, m2);
			new_vec.data[i] = m1.m128_f32[0] + m1.m128_f32[1] + m1.m128_f32[2] + m1.m128_f32[3];
		}

		return new_vec;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator*(float scalar) const
	{
		Mat_t<4, 4> mat = *this;

		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { scalar, scalar, scalar, scalar };
			m1 = _mm_mul_ps(m1, m2);
			mat.data[i][0] = m1.m128_f32[0];
			mat.data[i][1] = m1.m128_f32[1];
			mat.data[i][2] = m1.m128_f32[2];
			mat.data[i][3] = m1.m128_f32[3];
		}

		return mat;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator/(float scalar) const
	{
		Mat_t<4, 4> mat = *this;

		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { scalar, scalar, scalar, scalar };
			m1 = _mm_div_ps(m1, m2);
			mat.data[i][0] = m1.m128_f32[0];
			mat.data[i][1] = m1.m128_f32[1];
			mat.data[i][2] = m1.m128_f32[2];
			mat.data[i][3] = m1.m128_f32[3];
		}

		return mat;
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator+=(const Mat_t<4, 4>& other)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { other.data[i][0], other.data[i][1], other.data[i][2], other.data[i][3] };
			m1 = _mm_add_ps(m1, m2);
			data[i][0] = m1.m128_f32[0];
			data[i][1] = m1.m128_f32[1];
			data[i][2] = m1.m128_f32[2];
			data[i][3] = m1.m128_f32[3];
		}

		return *this;
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator-=(const Mat_t<4, 4>& other)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { other.data[i][0], other.data[i][1], other.data[i][2], other.data[i][3] };
			m1 = _mm_sub_ps(m1, m2);
			data[i][0] = m1.m128_f32[0];
			data[i][1] = m1.m128_f32[1];
			data[i][2] = m1.m128_f32[2];
			data[i][3] = m1.m128_f32[3];
		}

		return *this;
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator*=(const Mat_t<4, 4>& other)
	{
		*this = *this * other;
		return *this;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator*=(float scalar)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { scalar, scalar, scalar, scalar };
			m1 = _mm_mul_ps(m1, m2);
			data[i][0] = m1.m128_f32[0];
			data[i][1] = m1.m128_f32[1];
			data[i][2] = m1.m128_f32[2];
			data[i][3] = m1.m128_f32[3];
		}

		return *this;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator/=(float scalar)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			__m128 m1 = { data[i][0], data[i][1], data[i][2], data[i][3] };
			__m128 m2 = { scalar, scalar, scalar, scalar };
			m1 = _mm_div_ps(m1, m2);
			data[i][0] = m1.m128_f32[0];
			data[i][1] = m1.m128_f32[1];
			data[i][2] = m1.m128_f32[2];
			data[i][3] = m1.m128_f32[3];
		}

		return *this;
	}
}
#else
namespace dk
{
	Mat_t<2, 2> Mat_t<2, 2>::operator+(const Mat_t<2, 2>& other) const
	{
		Mat_t<2, 2> mat = *this;
		mat.data[0][0] += other.data[0][0];
		mat.data[1][0] += other.data[1][0];
		mat.data[0][1] += other.data[0][1];
		mat.data[1][1] += other.data[1][1];
		return mat;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator-(const Mat_t<2, 2>& other) const
	{
		Mat_t<2, 2> mat = *this;
		mat.data[0][0] -= other.data[0][0];
		mat.data[1][0] -= other.data[1][0];
		mat.data[0][1] -= other.data[0][1];
		mat.data[1][1] -= other.data[1][1];
		return mat;
	}

	Vec_t<2> Mat_t<2, 2>::operator*(const Vec_t<2>& vec) const
	{
		Vec_t<2> new_vec(0);
		new_vec.data[0] = (data[0][0] * vec.data[0]) + (data[0][1] * vec.data[1]);
		new_vec.data[1] = (data[1][0] * vec.data[0]) + (data[1][1] * vec.data[1]);
		return new_vec;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator*(float scalar) const
	{
		Mat_t<2, 2> mat = *this;
		mat.data[0][0] *= scalar;
		mat.data[1][0] *= scalar;
		mat.data[0][1] *= scalar;
		mat.data[1][1] *= scalar;
		return mat;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator/(float scalar) const
	{
		Mat_t<2, 2> mat = *this;
		mat.data[0][0] /= scalar;
		mat.data[1][0] /= scalar;
		mat.data[0][1] /= scalar;
		mat.data[1][1] /= scalar;
		return mat;
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator+=(const Mat_t<2, 2>& other)
	{
		data[0][0] += other.data[0][0];
		data[1][0] += other.data[1][0];
		data[0][1] += other.data[0][1];
		data[1][1] += other.data[1][1];
		return *this;
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator-=(const Mat_t<2, 2>& other)
	{
		data[0][0] -= other.data[0][0];
		data[1][0] -= other.data[1][0];
		data[0][1] -= other.data[0][1];
		data[1][1] -= other.data[1][1];
		return *this;
	}

	Mat_t<2, 2>& Mat_t<2, 2>::operator*=(const Mat_t<2, 2>& other)
	{
		Mat_t<2, 2> mat(0);

		for (size_t i = 0; i < 2; ++i)
			for (size_t j = 0; j < 2; ++j)
				for (size_t k = 0; k < 2; ++k)
					mat.data[i][j] += data[i][k] * other.data[k][j];

		*this = mat;
		return *this;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator*=(float scalar)
	{
		data[0][0] *= scalar;
		data[1][0] *= scalar;
		data[0][1] *= scalar;
		data[1][1] *= scalar;
		return *this;
	}

	Mat_t<2, 2> Mat_t<2, 2>::operator/=(float scalar)
	{
		data[0][0] /= scalar;
		data[1][0] /= scalar;
		data[0][1] /= scalar;
		data[1][1] /= scalar;
		return *this;
	}



	Mat_t<3, 3> Mat_t<3, 3>::operator+(const Mat_t<3, 3>& other) const
	{
		Mat_t<3, 3> mat = *this;
		mat.data[0][0] += other.data[0][0];
		mat.data[0][1] += other.data[0][1];
		mat.data[0][2] += other.data[0][2];
		mat.data[1][0] += other.data[1][0];
		mat.data[1][1] += other.data[1][1];
		mat.data[1][2] += other.data[1][2];
		mat.data[2][0] += other.data[2][0];
		mat.data[2][1] += other.data[2][1];
		mat.data[2][2] += other.data[2][2];
		return mat;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator-(const Mat_t<3, 3>& other) const
	{
		Mat_t<3, 3> mat = *this;
		mat.data[0][0] -= other.data[0][0];
		mat.data[0][1] -= other.data[0][1];
		mat.data[0][2] -= other.data[0][2];
		mat.data[1][0] -= other.data[1][0];
		mat.data[1][1] -= other.data[1][1];
		mat.data[1][2] -= other.data[1][2];
		mat.data[2][0] -= other.data[2][0];
		mat.data[2][1] -= other.data[2][1];
		mat.data[2][2] -= other.data[2][2];
		return mat;
	}

	Vec_t<3> Mat_t<3, 3>::operator*(const Vec_t<3>& vec) const
	{
		Vec_t<3> new_vec(0);
		new_vec.data[0] = (data[0][0] * vec.data[0]) + (data[0][1] * vec.data[1]) + (data[0][2] * vec.data[2]);
		new_vec.data[1] = (data[1][0] * vec.data[0]) + (data[1][1] * vec.data[1]) + (data[1][2] * vec.data[2]);
		new_vec.data[2] = (data[2][0] * vec.data[0]) + (data[2][1] * vec.data[1]) + (data[2][2] * vec.data[2]);
		return new_vec;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator*(float scalar) const
	{
		Mat_t<3, 3> mat = *this;
		mat.data[0][0] *= scalar;
		mat.data[0][1] *= scalar;
		mat.data[0][2] *= scalar;
		mat.data[1][0] *= scalar;
		mat.data[1][1] *= scalar;
		mat.data[1][2] *= scalar;
		mat.data[2][0] *= scalar;
		mat.data[2][1] *= scalar;
		mat.data[2][2] *= scalar;
		return mat;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator/(float scalar) const
	{
		Mat_t<3, 3> mat = *this;
		mat.data[0][0] /= scalar;
		mat.data[0][1] /= scalar;
		mat.data[0][2] /= scalar;
		mat.data[1][0] /= scalar;
		mat.data[1][1] /= scalar;
		mat.data[1][2] /= scalar;
		mat.data[2][0] /= scalar;
		mat.data[2][1] /= scalar;
		mat.data[2][2] /= scalar;
		return mat;
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator+=(const Mat_t<3, 3>& other)
	{
		data[0][0] += other.data[0][0];
		data[0][1] += other.data[0][1];
		data[0][2] += other.data[0][2];
		data[1][0] += other.data[1][0];
		data[1][1] += other.data[1][1];
		data[1][2] += other.data[1][2];
		data[2][0] += other.data[2][0];
		data[2][1] += other.data[2][1];
		data[2][2] += other.data[2][2];
		return *this;
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator-=(const Mat_t<3, 3>& other)
	{
		data[0][0] -= other.data[0][0];
		data[0][1] -= other.data[0][1];
		data[0][2] -= other.data[0][2];
		data[1][0] -= other.data[1][0];
		data[1][1] -= other.data[1][1];
		data[1][2] -= other.data[1][2];
		data[2][0] -= other.data[2][0];
		data[2][1] -= other.data[2][1];
		data[2][2] -= other.data[2][2];
		return *this;
	}

	Mat_t<3, 3>& Mat_t<3, 3>::operator*=(const Mat_t<3, 3>& other)
	{
		Mat_t<3, 3> mat(0);

		for (size_t i = 0; i < 3; ++i)
			for (size_t j = 0; j < 3; ++j)
				for (size_t k = 0; k < 3; ++k)
					mat.data[i][j] += data[i][k] * other.data[k][j];

		*this = mat;
		return *this;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator*=(float scalar)
	{
		data[0][0] *= scalar;
		data[0][1] *= scalar;
		data[0][2] *= scalar;
		data[1][0] *= scalar;
		data[1][1] *= scalar;
		data[1][2] *= scalar;
		data[2][0] *= scalar;
		data[2][1] *= scalar;
		data[2][2] *= scalar;
		return *this;
	}

	Mat_t<3, 3> Mat_t<3, 3>::operator/=(float scalar)
	{
		data[0][0] /= scalar;
		data[0][1] /= scalar;
		data[0][2] /= scalar;
		data[1][0] /= scalar;
		data[1][1] /= scalar;
		data[1][2] /= scalar;
		data[2][0] /= scalar;
		data[2][1] /= scalar;
		data[2][2] /= scalar;
		return *this;
	}



	Mat_t<4, 4> Mat_t<4, 4>::operator+(const Mat_t<4, 4>& other) const
	{
		Mat_t<4, 4> mat = *this;
		mat.data[0][0] += other.data[0][0];
		mat.data[0][1] += other.data[0][1];
		mat.data[0][2] += other.data[0][2];
		mat.data[0][3] += other.data[0][3];
		mat.data[1][0] += other.data[1][0];
		mat.data[1][1] += other.data[1][1];
		mat.data[1][2] += other.data[1][2];
		mat.data[1][3] += other.data[1][3];
		mat.data[2][0] += other.data[2][0];
		mat.data[2][1] += other.data[2][1];
		mat.data[2][2] += other.data[2][2];
		mat.data[2][3] += other.data[2][3];
		mat.data[3][0] += other.data[3][0];
		mat.data[3][1] += other.data[3][1];
		mat.data[3][2] += other.data[3][2];
		mat.data[3][3] += other.data[3][3];
		return mat;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator-(const Mat_t<4, 4>& other) const
	{
		Mat_t<4, 4> mat = *this;
		mat.data[0][0] -= other.data[0][0];
		mat.data[0][1] -= other.data[0][1];
		mat.data[0][2] -= other.data[0][2];
		mat.data[0][3] -= other.data[0][3];
		mat.data[1][0] -= other.data[1][0];
		mat.data[1][1] -= other.data[1][1];
		mat.data[1][2] -= other.data[1][2];
		mat.data[1][3] -= other.data[1][3];
		mat.data[2][0] -= other.data[2][0];
		mat.data[2][1] -= other.data[2][1];
		mat.data[2][2] -= other.data[2][2];
		mat.data[2][3] -= other.data[2][3];
		mat.data[3][0] -= other.data[3][0];
		mat.data[3][1] -= other.data[3][1];
		mat.data[3][2] -= other.data[3][2];
		mat.data[3][3] -= other.data[3][3];
		return mat;
	}

	Vec_t<4> Mat_t<4, 4>::operator*(const Vec_t<4>& vec) const
	{
		Vec_t<4> new_vec(0);
		new_vec.data[0] = (data[0][0] * vec.data[0]) + (data[0][1] * vec.data[1]) + (data[0][2] * vec.data[2]) + (data[0][3] * vec.data[3]);
		new_vec.data[1] = (data[1][0] * vec.data[0]) + (data[1][1] * vec.data[1]) + (data[1][2] * vec.data[2]) + (data[1][3] * vec.data[3]);
		new_vec.data[2] = (data[2][0] * vec.data[0]) + (data[2][1] * vec.data[1]) + (data[2][2] * vec.data[2]) + (data[2][3] * vec.data[3]);
		new_vec.data[3] = (data[3][0] * vec.data[0]) + (data[3][1] * vec.data[1]) + (data[3][2] * vec.data[2]) + (data[3][3] * vec.data[3]);
		return new_vec;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator*(float scalar) const
	{
		Mat_t<4, 4> mat = *this;
		mat.data[0][0] *= scalar;
		mat.data[0][1] *= scalar;
		mat.data[0][2] *= scalar;
		mat.data[0][3] *= scalar;
		mat.data[1][0] *= scalar;
		mat.data[1][1] *= scalar;
		mat.data[1][2] *= scalar;
		mat.data[1][3] *= scalar;
		mat.data[2][0] *= scalar;
		mat.data[2][1] *= scalar;
		mat.data[2][2] *= scalar;
		mat.data[2][3] *= scalar;
		mat.data[3][0] *= scalar;
		mat.data[3][1] *= scalar;
		mat.data[3][2] *= scalar;
		mat.data[3][3] *= scalar;
		return mat;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator/(float scalar) const
	{
		Mat_t<4, 4> mat = *this;
		mat.data[0][0] /= scalar;
		mat.data[0][1] /= scalar;
		mat.data[0][2] /= scalar;
		mat.data[0][3] /= scalar;
		mat.data[1][0] /= scalar;
		mat.data[1][1] /= scalar;
		mat.data[1][2] /= scalar;
		mat.data[1][3] /= scalar;
		mat.data[2][0] /= scalar;
		mat.data[2][1] /= scalar;
		mat.data[2][2] /= scalar;
		mat.data[2][3] /= scalar;
		mat.data[3][0] /= scalar;
		mat.data[3][1] /= scalar;
		mat.data[3][2] /= scalar;
		mat.data[3][3] /= scalar;
		return mat;
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator+=(const Mat_t<4, 4>& other)
	{
		data[0][0] += other.data[0][0];
		data[0][1] += other.data[0][1];
		data[0][2] += other.data[0][2];
		data[0][3] += other.data[0][3];
		data[1][0] += other.data[1][0];
		data[1][1] += other.data[1][1];
		data[1][2] += other.data[1][2];
		data[1][3] += other.data[1][3];
		data[2][0] += other.data[2][0];
		data[2][1] += other.data[2][1];
		data[2][2] += other.data[2][2];
		data[2][3] += other.data[2][3];
		data[3][0] += other.data[3][0];
		data[3][1] += other.data[3][1];
		data[3][2] += other.data[3][2];
		data[3][3] += other.data[3][3];
		return *this;
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator-=(const Mat_t<4, 4>& other)
	{
		data[0][0] -= other.data[0][0];
		data[0][1] -= other.data[0][1];
		data[0][2] -= other.data[0][2];
		data[0][3] -= other.data[0][3];
		data[1][0] -= other.data[1][0];
		data[1][1] -= other.data[1][1];
		data[1][2] -= other.data[1][2];
		data[1][3] -= other.data[1][3];
		data[2][0] -= other.data[2][0];
		data[2][1] -= other.data[2][1];
		data[2][2] -= other.data[2][2];
		data[2][3] -= other.data[2][3];
		data[3][0] -= other.data[3][0];
		data[3][1] -= other.data[3][1];
		data[3][2] -= other.data[3][2];
		data[3][3] -= other.data[3][3];
		return *this;
	}

	Mat_t<4, 4>& Mat_t<4, 4>::operator*=(const Mat_t<4, 4>& other)
	{
		Mat_t<4, 4> mat(0);

		for (size_t i = 0; i < 4; ++i)
			for (size_t j = 0; j < 4; ++j)
				for (size_t k = 0; k < 4; ++k)
					mat.data[i][j] += data[i][k] * other.data[k][j];

		*this = mat;
		return *this;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator*=(float scalar)
	{
		data[0][0] *= scalar;
		data[0][1] *= scalar;
		data[0][2] *= scalar;
		data[0][3] *= scalar;
		data[1][0] *= scalar;
		data[1][1] *= scalar;
		data[1][2] *= scalar;
		data[1][3] *= scalar;
		data[2][0] *= scalar;
		data[2][1] *= scalar;
		data[2][2] *= scalar;
		data[2][3] *= scalar;
		data[3][0] *= scalar;
		data[3][1] *= scalar;
		data[3][2] *= scalar;
		data[3][3] *= scalar;
		return *this;
	}

	Mat_t<4, 4> Mat_t<4, 4>::operator/=(float scalar)
	{
		data[0][0] /= scalar;
		data[0][1] /= scalar;
		data[0][2] /= scalar;
		data[0][3] /= scalar;
		data[1][0] /= scalar;
		data[1][1] /= scalar;
		data[1][2] /= scalar;
		data[1][3] /= scalar;
		data[2][0] /= scalar;
		data[2][1] /= scalar;
		data[2][2] /= scalar;
		data[2][3] /= scalar;
		data[3][0] /= scalar;
		data[3][1] /= scalar;
		data[3][2] /= scalar;
		data[3][3] /= scalar;
		return *this;
	}
}
#endif