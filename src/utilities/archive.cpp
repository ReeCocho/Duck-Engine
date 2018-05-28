/**
 * @file archive.cpp
 * @brief Archive source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include "archive.hpp"

namespace dk
{
	JSONArchive::JSONArchive() : m_json({}), m_writing(true) {}

	JSONArchive::JSONArchive(const json& archive) : m_json(archive), m_writing(false) {}

	template<>
	void JSONArchive::write<glm::vec2>(const std::string& name, glm::vec2 data)
	{
		m_json[name][0] = data.x;
		m_json[name][1] = data.y;
	}

	template<>
	void JSONArchive::write<glm::vec3>(const std::string& name, glm::vec3 data)
	{
		m_json[name][0] = data.x;
		m_json[name][1] = data.y;
		m_json[name][2] = data.z;
	}

	template<>
	void JSONArchive::write<glm::vec4>(const std::string& name, glm::vec4 data)
	{
		m_json[name][0] = data.x;
		m_json[name][1] = data.y;
		m_json[name][2] = data.z;
		m_json[name][3] = data.w;
	}

	template<>
	void JSONArchive::write<glm::quat>(const std::string& name, glm::quat data)
	{
		m_json[name][0] = data.x;
		m_json[name][1] = data.y;
		m_json[name][2] = data.z;
		m_json[name][3] = data.w;
	}

	template<>
	void JSONArchive::write<glm::mat2>(const std::string& name, glm::mat2 data)
	{
		m_json[name][0] = data[0][0];
		m_json[name][1] = data[0][1];
		m_json[name][2] = data[1][0];
		m_json[name][3] = data[1][1];
	}

	template<>
	void JSONArchive::write<glm::mat3>(const std::string& name, glm::mat3 data)
	{
		m_json[name][0] = data[0][0];
		m_json[name][1] = data[0][1];
		m_json[name][2] = data[0][2];
		m_json[name][3] = data[1][0];
		m_json[name][4] = data[1][1];
		m_json[name][5] = data[1][2];
		m_json[name][6] = data[2][0];
		m_json[name][7] = data[2][1];
		m_json[name][8] = data[2][2];
	}

	template<>
	void JSONArchive::write<glm::mat4>(const std::string& name, glm::mat4 data)
	{
		m_json[name][0 ] = data[0][0];
		m_json[name][1 ] = data[0][1];
		m_json[name][2 ] = data[0][2];
		m_json[name][3 ] = data[0][3];
		m_json[name][4 ] = data[1][0];
		m_json[name][5 ] = data[1][1];
		m_json[name][6 ] = data[1][2];
		m_json[name][7 ] = data[1][3];
		m_json[name][8 ] = data[2][0];
		m_json[name][9 ] = data[2][1];
		m_json[name][10] = data[2][2];
		m_json[name][11] = data[2][3];
		m_json[name][12] = data[3][0];
		m_json[name][13] = data[3][1];
		m_json[name][14] = data[3][2];
		m_json[name][15] = data[3][3];
	}

	template<>
	glm::vec2 JSONArchive::read<glm::vec2>(const std::string& name)
	{
		return { m_json[name][0], m_json[name][1] };
	}

	template<>
	glm::vec3 JSONArchive::read<glm::vec3>(const std::string& name)
	{
		return { m_json[name][0], m_json[name][1], m_json[name][2] };
	}

	template<>
	glm::vec4 JSONArchive::read<glm::vec4>(const std::string& name)
	{
		return { m_json[name][0], m_json[name][1], m_json[name][2], m_json[name][3] };
	}

	template<>
	glm::quat JSONArchive::read<glm::quat>(const std::string& name)
	{
		return { m_json[name][0], m_json[name][1], m_json[name][2], m_json[name][3] };
	}

	template<>
	glm::mat2 JSONArchive::read<glm::mat2>(const std::string& name)
	{
		glm::mat2 mat = {};
		mat[0][0] = m_json[name][0];
		mat[0][1] = m_json[name][1];
		mat[1][0] = m_json[name][2];
		mat[1][1] = m_json[name][3];
		return mat;
	}

	template<>
	glm::mat3 JSONArchive::read<glm::mat3>(const std::string& name)
	{
		glm::mat3 mat = {};
		mat[0][0] = m_json[name][0];
		mat[0][1] = m_json[name][1];
		mat[0][2] = m_json[name][2];
		mat[1][0] = m_json[name][3];
		mat[1][1] = m_json[name][4];
		mat[1][2] = m_json[name][5];
		mat[2][0] = m_json[name][6];
		mat[2][1] = m_json[name][7];
		mat[2][2] = m_json[name][8];
		return mat;
	}

	template<>
	glm::mat4 JSONArchive::read<glm::mat4>(const std::string& name)
	{
		glm::mat4 mat = {};
		mat[0][0] = m_json[name][0];
		mat[0][1] = m_json[name][1];
		mat[0][2] = m_json[name][2];
		mat[0][3] = m_json[name][3];
		mat[1][0] = m_json[name][4];
		mat[1][1] = m_json[name][5];
		mat[1][2] = m_json[name][6];
		mat[1][3] = m_json[name][7];
		mat[2][0] = m_json[name][8];
		mat[2][1] = m_json[name][9];
		mat[2][2] = m_json[name][10];
		mat[2][3] = m_json[name][11];
		mat[3][0] = m_json[name][12];
		mat[3][1] = m_json[name][13];
		mat[3][2] = m_json[name][14];
		mat[3][3] = m_json[name][15];
		return mat;
	}
}