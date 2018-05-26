/**
 * @file renderer.cpp
 * @brief Duck renderer base source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "renderer.hpp"

namespace dk
{
	Renderer::Renderer(Graphics* graphics, int width, int height) : 
		m_graphics(graphics),
		m_width(width),
		m_height(height)
	{}

	void Renderer::resize(int width, int height)
	{
		m_width = width;
		m_height = height;
	}
}