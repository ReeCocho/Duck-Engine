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
#include "editor.hpp"
#include "hierarchy.hpp"
#include "inspector.hpp"
#include "scene_view.hpp"
#include "file_explorer.hpp"

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



	/** Font texture. */
	dk::Handle<dk::Texture> font_texture;
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
			// Setup Dear ImGui binding
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();

			// Read config file
			std::ifstream stream(path);
			dk_assert(stream.is_open());
			json j;
			stream >> j;

			// Init graphics
			::new(&graphics)(Graphics)(j["thread_count"], j["title"], j["width"], j["height"]);

			// Tell IMGUI about the window
#ifdef _WIN32
			SDL_SysWMinfo wmInfo;
			SDL_VERSION(&wmInfo.version);
			SDL_GetWindowWMInfo(graphics.get_window(), &wmInfo);
			io.ImeWindowHandle = wmInfo.info.win.window;
#endif

			// Setup style
			ImGui::StyleColorsDark();

			// Setup display size (every frame to accommodate for window resizing)
			int w = graphics.get_width();
			int h = graphics.get_height();
			io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
			io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

			// Init resource manager
			::new(&resource_manager)(ResourceManager)(&renderer);

			// Create physics engine
			::new(&physics)(Physics)(glm::vec3(j["gravity"][0], j["gravity"][1], j["gravity"][2]));

			// Init renderers
			::new(&renderer)(OffScreenForwardRenderer)(&graphics, &resource_manager.get_texture_allocator(), &resource_manager.get_mesh_allocator());
			::new(&editor_renderer)(EditorRenderer)(&graphics, &resource_manager.get_texture_allocator());

			// Set font descriptor set
			io.Fonts->TexID = editor_renderer.get_font_descriptor_set();

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

			// Key mappings
			io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
			io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
			io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
			io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
			io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
			io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
			io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
			io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
			io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
			io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
			io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
			io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
			io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
			io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
			io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
			io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
			io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
			io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
			io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
			io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
			io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

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

			// Get IMGUI IO 
			ImGuiIO& io = ImGui::GetIO();

			// Widgets
			Inspector inspector(&graphics, &scene);
			EditorHierarchy hierarchy(&graphics, &scene, &inspector);
			SceneView scene_view(&renderer, &editor_renderer, &input);

			while (!input.is_closing())
			{
				// Gather input
				input.poll_events();

				// Update mouse events
				io.MouseDown[0] = input.get_mouse_button_held(dk::MouseButton::Left);
				io.MouseDown[1] = input.get_mouse_button_held(dk::MouseButton::Right);
				io.MouseDown[2] = input.get_mouse_button_held(dk::MouseButton::Middle);

				glm::vec2 mouse_pos = input.get_mouse_position();
				io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);

				glm::vec2 mouse_wheel = input.get_mouse_wheel();
				if (mouse_wheel.x > 0) io.MouseWheelH += 1;
				if (mouse_wheel.x < 0) io.MouseWheelH -= 1;
				if (mouse_wheel.y > 0) io.MouseWheel += 1;
				if (mouse_wheel.y < 0) io.MouseWheel -= 1;

				// Update text input
				io.AddInputCharactersUTF8(input.get_text_input().data());

				// Update keyboard
				io.KeysDown[SDL_SCANCODE_TAB] = input.get_key_held(dk::KeyCode::Tab);
				io.KeysDown[SDL_SCANCODE_LEFT] = input.get_key_held(dk::KeyCode::Left);
				io.KeysDown[SDL_SCANCODE_RIGHT] = input.get_key_held(dk::KeyCode::Right);
				io.KeysDown[SDL_SCANCODE_UP] = input.get_key_held(dk::KeyCode::Up);
				io.KeysDown[SDL_SCANCODE_DOWN] = input.get_key_held(dk::KeyCode::Down);
				io.KeysDown[SDL_SCANCODE_PAGEUP] = input.get_key_held(dk::KeyCode::PageUp);
				io.KeysDown[SDL_SCANCODE_PAGEDOWN] = input.get_key_held(dk::KeyCode::PageDown);
				io.KeysDown[SDL_SCANCODE_HOME] = input.get_key_held(dk::KeyCode::Home);
				io.KeysDown[SDL_SCANCODE_END] = input.get_key_held(dk::KeyCode::End);
				io.KeysDown[SDL_SCANCODE_INSERT] = input.get_key_held(dk::KeyCode::Insert);
				io.KeysDown[SDL_SCANCODE_DELETE] = input.get_key_held(dk::KeyCode::Delete);
				io.KeysDown[SDL_SCANCODE_BACKSPACE] = input.get_key_held(dk::KeyCode::Backspace);
				io.KeysDown[SDL_SCANCODE_SPACE] = input.get_key_held(dk::KeyCode::Space);
				io.KeysDown[SDL_SCANCODE_RETURN] = input.get_key_held(dk::KeyCode::Return);
				io.KeysDown[SDL_SCANCODE_ESCAPE] = input.get_key_held(dk::KeyCode::Escape);
				io.KeysDown[SDL_SCANCODE_A] = input.get_key_held(dk::KeyCode::A);
				io.KeysDown[SDL_SCANCODE_C] = input.get_key_held(dk::KeyCode::C);
				io.KeysDown[SDL_SCANCODE_V] = input.get_key_held(dk::KeyCode::V);
				io.KeysDown[SDL_SCANCODE_X] = input.get_key_held(dk::KeyCode::X);
				io.KeysDown[SDL_SCANCODE_Y] = input.get_key_held(dk::KeyCode::Y);
				io.KeysDown[SDL_SCANCODE_Z] = input.get_key_held(dk::KeyCode::Z);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

				// Get delta time
				float dt = game_clock.get_delta_time();

				// Perform a game tick
				scene.tick(dt);

				// Update delta time for imgui
				io.DeltaTime = dt;

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

				// Master frame
				ImGui::NewFrame();

				// Draw widgets
				hierarchy.draw();
				inspector.draw();
				scene_view.draw(dt);

				// End master frame
				ImGui::EndFrame();

				// Tell IMGUI to render
				ImGui::Render();

				// Draw to window
				editor_renderer.render();
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
			editor_renderer.shutdown();
			renderer.shutdown();
			physics.shutdown();
			resource_manager.shutdown();
			graphics.shutdown();
		}
	}
}