/**
 * @file renderer.cpp
 * @brief Duck renderer base source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <array>
#include "renderer.hpp"

namespace dk
{
	Renderer::Renderer(Graphics& graphics) :	
		m_graphics(graphics), 
		m_swapchain_manager
		(
			m_graphics.get_physical_device(), 
			m_graphics.get_logical_device(), 
			m_graphics.get_surface(), 
			m_graphics.get_width(), 
			m_graphics.get_height()
		)
	{

	}

	Renderer::~Renderer()
	{

	}
}