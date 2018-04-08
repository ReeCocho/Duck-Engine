/**
 * @file engine.cpp
 * @brief Engine source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "engine.hpp"

namespace dk
{
	namespace engine
	{
		Graphics graphics;

		RendererType renderer;

		Input input;



		void initialize(size_t thread_count, const std::string& name, int width, int height)
		{
			::new(&graphics)(Graphics)(thread_count, name, width, height);
			::new(&renderer)(RendererType)(&graphics);
			input = Input();
		}

		void simulate()
		{
			while (!input.is_closing())
			{
				input.poll_events();
				renderer.render();
			}
		}

		void shutdown()
		{
			// Wait for presentation to finish
			graphics.get_device_manager().get_present_queue().waitIdle();

			renderer.shutdown();
			graphics.shutdown();
		}
	}
}