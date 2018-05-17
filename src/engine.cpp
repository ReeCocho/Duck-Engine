/**
 * @file engine.cpp
 * @brief Engine source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include <utilities\clock.hpp>
#include <graphics\material.hpp>
#include <config.hpp>
#include <json.hpp>
#include <fstream>
#include "engine.hpp"

/** For convenience */
using json = nlohmann::json;

namespace
{
	/** Rendering thread. */
	std::unique_ptr<dk::SimulationThread> rendering_thread;

	/* Physics thread. */
	std::unique_ptr<dk::SimulationThread> physics_thread;

	/** Game time clock. */
	dk::Clock game_clock = {};

	/** Physics clock. */
	dk::Clock physics_clock = {};

	/** Physics timer. */
	float physics_timer = 0.0f;
}

namespace dk
{
	namespace engine
	{
		Graphics graphics;

		ForwardRenderer renderer;

		ResourceManager resource_manager;

		Physics physics;

		Input input;

		Scene scene;



		void initialize(const std::string& path)
		{
			// Read config file
			std::ifstream stream(path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;

			// Init graphics
			::new(&graphics)(Graphics)(j["thread_count"], j["title"], j["width"], j["height"]);

			// Init resource manager
			::new(&resource_manager)(ResourceManager)(&renderer);

			// Create physics engine
			::new(&physics)(Physics)(glm::vec3(j["gravity"][0], j["gravity"][1], j["gravity"][2]));

			// Init renderer
			::new(&renderer)(ForwardRenderer)(&graphics, &resource_manager.get_texture_allocator(), &resource_manager.get_mesh_allocator());

			// Load resources
			resource_manager.load_resources
			(
				j["meshes"],
				j["textures"],
				j["shaders"],
				j["materials"],
				j["cubemaps"],
				j["skys"]
			);

			// Init input manager
			input = Input();

			// Init scene
			scene = {};

			// Create threads
			rendering_thread = std::make_unique<SimulationThread>([]() { renderer.render(); });
			physics_thread = std::make_unique<SimulationThread>([]() { physics.step(physics_timer); physics_timer = 0.0f; });
		}

		void simulate()
		{
			// Reset clocks
			game_clock.get_delta_time();
			physics_clock.get_delta_time();

			// FPS timer data
			uint64_t frames_per_sec = 0;
			float fps_timer = 0.0f;

			while (!input.is_closing())
			{
				// Gather input
				input.poll_events();

				// Wait for threads to finish
				rendering_thread->wait();
				physics_thread->wait();

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

				// Increment physics timer
				physics_timer += physics_clock.get_delta_time();

				// Run physics
				if(physics_timer >= DK_PHYSICS_STEP_RATE)
					physics_thread->start();
			}

			// Wait for threads to finish
			rendering_thread->wait();
			physics_thread->wait();
			graphics.get_device_manager().get_present_queue().waitIdle();
		}

		void shutdown()
		{
			// Stop threads
			rendering_thread.reset();
			physics_thread.reset();

			// Shutdown systems
			scene.shutdown();
			renderer.shutdown();
			physics.shutdown();
			resource_manager.shutdown();
			graphics.shutdown();
		}
	}
}