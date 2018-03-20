#pragma once

/**
 * @file transformations.cpp
 * @brief Matrix transformation utilities source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "transformations.hpp"

namespace dk
{
	Mat_t<4, 4> perspective(float near, float far, float aspect, float fov)
	{
		dk_assert(near != 0.0f && far != 0.0f && aspect != 0.0f && fov > 0.0f && fov < (90.0f * DUCK_RAD_CONST));
		const float hfovt = std::tanf((fov * DUCK_RAD_CONST) / 2.0f);
		const float fmn = far - near;

		return
		{
			1.0f / (aspect * hfovt),	0.0f,			0.0f,				0.0f,
			0.0f,						1.0f / hfovt,	0.0f,				0.0f,
			0.0f,						0.0f,			far/fmn,			1.0f,		
			0.0f,						0.0f,			-(far*near)/fmn,	1.0f
		};
	}

	Mat_t<4, 4> ortho(float near, float far, float w, float h)
	{
		dk_assert(near != 0.0f && far != 0.0f && w != 0.0f && h != 0.0f);

		const float dw = w * w;
		const float dh = h * h;
		const float fmn = far - near;

		return 
		{
			2.0f / dw,	0.0f,		0.0f,		0.0f,
			0.0f,		2.0f / dh,	0.0f,		0.0f,
			0.0f,		0.0f,		1.0f / fmn,	0.0f,
			0.0f,		0.0f,		near / fmn,	1.0f
		};
	}

	Mat_t<4, 4> look_at(const Vec_t<3>& src, const Vec_t<3>& trg, const Vec_t<3>& up)
	{
		const Vec_t<3> f = (trg - src).normalized();
		const Vec_t<3> s = up.cross(f).normalized();
		const Vec_t<3> u = f.cross(s);

		return
		{
			s.x, s.y, s.z, 0.0f,
			u.x, u.y, u.z, 0.0f,
			f.x, f.y, f.z, 0.0f,
			-s.dot(src), -u.dot(src), -f.dot(src), 1.0f
		};
	}

	Mat_t<4, 4> translate(const Vec_t<4>& pos)
	{
		return 
		{
			1.0f, 0.0f, 0.0f, pos.x,
			0.0f, 1.0f, 0.0f, pos.y,
			0.0f, 0.0f, 1.0f, pos.z,
			0.0f, 0.0f, 0.0f, pos.w
		};
	}

	Mat_t<4, 4> rotate(const Quat& rot)
	{
		return rot.as_matrix();
	}

	Mat_t<4, 4> scale(const Vec_t<4>& scl)
	{
		return
		{
			scl.x, 0.0f, 0.0f, 0.0f,
			0.0f, scl.y, 0.0f, 0.0f,
			0.0f, 0.0f, scl.z, 0.0f,
			0.0f, 0.0f, 0.0f, scl.w
		};
	}
}