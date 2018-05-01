/**
 * @file engine.cpp
 * @brief Engine source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include <utilities\clock.hpp>
#include <graphics\material.hpp>
#include <json.hpp>
#include <fstream>
#include "engine.hpp"

/** For convenience */
using json = nlohmann::json;

namespace
{
	/** Rendering thread. */
	std::unique_ptr<dk::SimulationThread> rendering_thread;

	/** Game time clock. */
	dk::Clock game_clock;
}

namespace dk
{
	namespace engine
	{
		Graphics graphics = {};

		Renderer renderer = {};

		ResourceManager resource_manager = {};

		Input input;

		Scene scene = {};



		void initialize(const std::string& path)
		{
			// Read config file
			std::ifstream stream(path);
			json j;
			stream >> j;

			// Init graphics
			::new(&graphics)(Graphics)(j["thread_count"], j["title"], j["width"], j["height"]);

			// Init resource manager
			::new(&resource_manager)(ResourceManager)
			(
				static_cast<Renderer*>(&renderer),
				j["meshes"],
				j["textures"],
				j["shaders"],
				j["materials"]
			);

			// Init renderer
			::new(&renderer)(Renderer)(&graphics, &resource_manager.get_texture_allocator(), &resource_manager.get_mesh_allocator());

			// Init input manager
			input = Input();

			// Init scene
			scene = {};

			rendering_thread = std::make_unique<SimulationThread>([]() { renderer.render(); });
		}

		void simulate()
		{
			// Reset delta time
			game_clock.get_delta_time();
			
			// FPS timer data
			uint64_t frames_per_sec = 0;
			float fps_timer = 0;

			while (!input.is_closing())
			{
				// Gather input
				input.poll_events();

				// Wait for threads to finish
				rendering_thread->wait();

				// Get delta time
				float dt = game_clock.get_delta_time();
				fps_timer += dt;

				// Check if we need to print and reset FPS
				if (fps_timer >= 1.0f)
				{
					dk_log("FPS : " << frames_per_sec);
					fps_timer = 0;
					frames_per_sec = 0;
				}
				++frames_per_sec;

				// Perform a game tick
				scene.tick(dt);

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
			resource_manager.shutdown();
			graphics.shutdown();
		}
	}
}