/**
 * @file engine.cpp
 * @brief Engine source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include <graphics\material.hpp>
#include "engine.hpp"

namespace
{
	/** Rendering thread. */
	std::unique_ptr<dk::SimulationThread> rendering_thread;
}

namespace dk
{
	namespace engine
	{
		Graphics graphics = {};

		RendererType renderer = {};

		Input input;

		Scene scene = {};



		void initialize(size_t thread_count, const std::string& name, int width, int height)
		{
			::new(&graphics)(Graphics)(thread_count, name, width, height);
			::new(&renderer)(RendererType)(&graphics);
			input = Input();
			scene = {};

			rendering_thread = std::make_unique<SimulationThread>([]() { renderer.render(); });
		}

		void simulate()
		{
			while (!input.is_closing())
			{
				// Gather input
				input.poll_events();

				// Wait for threads to finish
				rendering_thread->wait();
				
				// Perform a game tick
				scene.tick(0);

				// Start rendering thread
				rendering_thread->start();
			}

			// Wait for presentation to finish
			rendering_thread->wait();
			graphics.get_device_manager().get_present_queue().waitIdle();
		}

		void shutdown()
		{
			// Stop threads
			rendering_thread.reset();

			scene.shutdown();
			renderer.shutdown();
			graphics.shutdown();
		}
	}
}