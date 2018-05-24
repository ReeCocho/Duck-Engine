/**
 * @file editor.cpp
 * @brief Editor source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\threading.hpp>
#include <utilities\clock.hpp>
#include <graphics\material.hpp>
#include <config.hpp>
#include <json.hpp>
#include <fstream>
#include "imgui\imgui.h"
#include "imgui\imguidock.h"
#include "editor.hpp"
#include "editor_window.hpp"

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

	/** Delta time. (Time since last frame. */
	float delta_time;

	std::unique_ptr<dk::EditorWindow> editor_window;
}

namespace dk
{
	namespace editor
	{
		Graphics graphics;

		OffScreenForwardRenderer renderer;

		EditorRenderer editor_renderer;

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

			// Init renderers
			::new(&renderer)(OffScreenForwardRenderer)(&graphics, &resource_manager.get_texture_allocator(), &resource_manager.get_mesh_allocator());
			::new(&editor_renderer)(EditorRenderer)(&graphics);

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
			::new(&input)(Input)(0);

			// Init scene
			scene = {};

			// Create threads
			rendering_thread = std::make_unique<SimulationThread>([]() 
			{ 
				renderer.render(); 
				editor_window->draw(delta_time);
				editor_renderer.render();
			});

			physics_thread = std::make_unique<SimulationThread>([]() 
			{ 
				physics.step(physics_timer); 
				physics_timer = 0.0f; 
			});
		}

		void simulate()
		{
			// Reset clocks
			game_clock.get_delta_time();
			physics_clock.get_delta_time();

			// Create editor window (We do this here so the transform system can be added to the scene)
			editor_window = std::make_unique<EditorWindow>(&graphics, &editor_renderer, &renderer, &input, &scene);

			while (!input.is_closing())
			{
				// Gather input
				input.poll_events();

				// Get delta time
				delta_time = game_clock.get_delta_time();

				// Perform a game tick
				scene.tick(delta_time);

				// Start rendering thread
				rendering_thread->start();

				// Increment physics timer
				physics_timer += physics_clock.get_delta_time();

				// Run physics
				if (physics_timer >= DK_PHYSICS_STEP_RATE)
					physics_thread->start();

				// Wait for threads to finish
				rendering_thread->wait();
				physics_thread->wait();
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
			editor_window.reset();
			scene.shutdown();
			editor_renderer.shutdown();
			renderer.shutdown();
			physics.shutdown();
			resource_manager.shutdown();
			graphics.shutdown();
		}
	}
}