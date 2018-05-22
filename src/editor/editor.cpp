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

		OffScreenForwardRenderer scene_renderer;

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
			ImGuiIO& io = ImGui::GetIO(); (void)io;

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
			::new(&resource_manager)(ResourceManager)(&scene_renderer);

			// Create physics engine
			::new(&physics)(Physics)(glm::vec3(j["gravity"][0], j["gravity"][1], j["gravity"][2]));

			// Init renderers
			::new(&scene_renderer)(OffScreenForwardRenderer)(&graphics, &resource_manager.get_texture_allocator(), &resource_manager.get_mesh_allocator());
			::new(&editor_renderer)(EditorRenderer)(&graphics);

			// Create font textures
			{
				ImGuiIO& io = ImGui::GetIO();

				// Get texture data
				unsigned char* pixels;
				int width, height;
				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
				size_t upload_size = width * height * 4 * sizeof(char);

				// Create image
				vk::ImageCreateInfo info = {};
				info.imageType = vk::ImageType::e2D;
				info.format = vk::Format::eR8G8B8A8Unorm;
				info.extent.width = static_cast<uint32_t>(width);
				info.extent.height = static_cast<uint32_t>(height);
				info.extent.depth = 1;
				info.mipLevels = 1;
				info.arrayLayers = 1;
				info.samples = vk::SampleCountFlagBits::e1;
				info.tiling = vk::ImageTiling::eOptimal;
				info.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
				info.sharingMode = vk::SharingMode::eExclusive;
				info.initialLayout = vk::ImageLayout::eUndefined;

				// Create image
				vk::Image image;
				vk::DeviceMemory memory;
				graphics.create_image
				(
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height),
					vk::Format::eR8G8B8A8Unorm,
					vk::ImageTiling::eOptimal,
					vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
					vk::MemoryPropertyFlagBits::eDeviceLocal,
					image,
					memory
				);
				dk_assert(image);
				dk_assert(memory);

				// Create image view
				vk::ImageView view = graphics.create_image_view(image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
				dk_assert(view);

				// Create staging buffer
				VkMemBuffer staging_buffer = graphics.create_buffer
				(
					static_cast<vk::DeviceSize>(upload_size),
					vk::BufferUsageFlagBits::eTransferSrc,
					vk::MemoryPropertyFlagBits::eHostVisible
				);

				// Transition to transfer mode
				graphics.transition_image_layout(image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

				// Upload to buffer
				void* map = graphics.get_logical_device().mapMemory(staging_buffer.memory, 0, upload_size);
				memcpy(map, pixels, upload_size);
				vk::MappedMemoryRange range[1] = {};
				range[0].memory = staging_buffer.memory;
				range[0].size = upload_size;
				graphics.get_logical_device().flushMappedMemoryRanges(1, range);
				graphics.get_logical_device().unmapMemory(staging_buffer.memory);

				// Copy image data
				graphics.copy_buffer_to_image(staging_buffer.buffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
				
				// Transition back to sampling
				graphics.transition_image_layout(image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

				// Destroy staging buffer
				staging_buffer.free(graphics.get_logical_device());

				// Sampler creation info
				vk::SamplerCreateInfo sampler_info = {};
				sampler_info.setMagFilter(vk::Filter::eLinear);
				sampler_info.setMinFilter(vk::Filter::eLinear);
				sampler_info.setAddressModeU(vk::SamplerAddressMode::eRepeat);
				sampler_info.setAddressModeV(vk::SamplerAddressMode::eRepeat);
				sampler_info.setAddressModeW(vk::SamplerAddressMode::eRepeat);
				sampler_info.setAnisotropyEnable(false);
				sampler_info.setMaxAnisotropy(1);
				sampler_info.setBorderColor(vk::BorderColor::eFloatOpaqueBlack);
				sampler_info.setUnnormalizedCoordinates(false);
				sampler_info.setCompareEnable(false);
				sampler_info.setCompareOp(vk::CompareOp::eAlways);
				sampler_info.setMipmapMode(vk::SamplerMipmapMode::eLinear);
				sampler_info.setMipLodBias(0.0f);
				sampler_info.setMinLod(0.0f);
				sampler_info.setMaxLod(1);

				// Create sampler
				vk::Sampler sampler = graphics.get_logical_device().createSampler(sampler_info);

				// Create texture
				font_texture = resource_manager.create_texture
				(
					"font_texture",
					image,
					view,
					sampler,
					memory,
					vk::Filter::eLinear,
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				);

				// Store our identifier
				io.Fonts->TexID = (void*)(intptr_t)(VkImage)image;
			}

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
				scene_renderer.render(); 
				editor_renderer.render();
			});

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

			ImGuiIO& io = ImGui::GetIO();

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

				// Update delta time fo imgui
				io.DeltaTime = dt;

				// Start rendering thread
				rendering_thread->start();

				// Increment physics timer
				physics_timer += physics_clock.get_delta_time();

				// Run physics
				if (physics_timer >= DK_PHYSICS_STEP_RATE)
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
			editor_renderer.shutdown();
			scene_renderer.shutdown();
			physics.shutdown();
			resource_manager.shutdown();
			graphics.shutdown();
		}
	}
}