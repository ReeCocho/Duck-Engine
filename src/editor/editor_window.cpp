/**
 * @file editor_window.cpp
 * @brief Editor window source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include "editor_window.hpp"

namespace dk
{
	EditorWindow::EditorWindow
	(
		Graphics* graphics,
		EditorRenderer* editor_renderer,
		OffScreenForwardRenderer* scene_renderer,
		Input* input,
		Scene* scene
	) :
		m_graphics(graphics),
		m_editor_renderer(editor_renderer),
		m_scene_renderer(scene_renderer),
		m_input(input),
		m_scene(scene)
	{
		// Setup Dear ImGui binding
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// Tell IMGUI about the window
#ifdef _WIN32
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(m_graphics->get_window(), &wmInfo);
		io.ImeWindowHandle = wmInfo.info.win.window;
#endif

		// Setup style
		ImGui::StyleColorsDark();

		// Setup display size (every frame to accommodate for window resizing)
		int w = m_graphics->get_width();
		int h = m_graphics->get_height();
		io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		// Create font textures
		{
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
			m_graphics->create_image
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
			vk::ImageView view = m_graphics->create_image_view(image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
			dk_assert(view);

			// Create staging buffer
			VkMemBuffer staging_buffer = m_graphics->create_buffer
			(
				static_cast<vk::DeviceSize>(upload_size),
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible
			);

			// Transition to transfer mode
			m_graphics->transition_image_layout(image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

			// Upload to buffer
			void* map = m_graphics->get_logical_device().mapMemory(staging_buffer.memory, 0, upload_size);
			memcpy(map, pixels, upload_size);
			vk::MappedMemoryRange range[1] = {};
			range[0].memory = staging_buffer.memory;
			range[0].size = upload_size;
			m_graphics->get_logical_device().flushMappedMemoryRanges(1, range);
			m_graphics->get_logical_device().unmapMemory(staging_buffer.memory);

			// Copy image data
			m_graphics->copy_buffer_to_image(staging_buffer.buffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

			// Transition back to sampling
			m_graphics->transition_image_layout(image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

			// Destroy staging buffer
			staging_buffer.free(m_graphics->get_logical_device());

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
			sampler_info.setMinLod(-1000.0f);
			sampler_info.setMaxLod(1000.0f);

			// Create sampler
			vk::Sampler sampler = m_graphics->get_logical_device().createSampler(sampler_info);

			// Create texture
			m_font_texture = std::make_unique<Texture>
			(
				m_graphics,
				image,
				view,
				sampler,
				memory,
				vk::Filter::eLinear,
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			);
		}

		// Create font descriptor set
		{
			// Create descriptor pool
			vk::DescriptorPoolSize pool_size = { vk::DescriptorType::eCombinedImageSampler, 1 };

			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
			pool_info.maxSets = 1;
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			m_vk_font_descriptor_pool = m_graphics->get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_vk_font_descriptor_pool);

			// Create font descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_vk_font_descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_editor_renderer->get_descriptor_set_layout();
			m_vk_font_descriptor_set = m_graphics->get_logical_device().allocateDescriptorSets(alloc_info)[0];
			dk_assert(m_vk_font_descriptor_set);

			// Update font descriptor set
			vk::DescriptorImageInfo image = {};
			image.sampler = m_font_texture->get_sampler();
			image.imageView = m_font_texture->get_image_view();
			image.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			vk::WriteDescriptorSet write_desc = {};
			write_desc.dstSet = m_vk_font_descriptor_set;
			write_desc.descriptorCount = 1;
			write_desc.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			write_desc.pImageInfo = &image;
			write_desc.dstBinding = 0;

			m_graphics->get_logical_device().updateDescriptorSets(write_desc, {});
		}

		// Set font descriptor set
		io.Fonts->TexID = m_vk_font_descriptor_set;

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

		// Cursors
		m_cursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		m_cursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		m_cursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		m_cursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		m_cursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		m_cursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
		m_cursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);

		// Create docking context
		m_dock_context = ImGui::CreateDockContext();
		ImGui::SetCurrentDockContext(m_dock_context);

		// Create widgets
		m_inspector = std::make_unique<Inspector>(m_graphics, m_scene);
		m_hierarchy = std::make_unique<EditorHierarchy>(m_graphics, m_scene, m_inspector.get());
		m_scene_view = std::make_unique<SceneView>(m_scene_renderer, m_editor_renderer, m_input);
		m_toolbar = std::make_unique<Toolbar>(m_graphics, m_scene);
	}

	EditorWindow::~EditorWindow()
	{
		// Destroy descriptor pool
		m_graphics->get_logical_device().destroyDescriptorPool(m_vk_font_descriptor_pool);

		// Destroy font texture
		m_font_texture->free();
		m_font_texture.reset();

		// Release cursors
		for (ImGuiMouseCursor i = 0; i < ImGuiMouseCursor_COUNT; ++i)
			SDL_FreeCursor(m_cursors[i]);

		// Destroy dock context
		ImGui::DestroyDockContext(m_dock_context);
	}

	void EditorWindow::draw(float dt)
	{
		// Get IMGUI IO 
		ImGuiIO& io = ImGui::GetIO();

		// Update delta time for imgui
		io.DeltaTime = dt;

		// Update mouse events
		io.MouseDown[0] = m_input->get_mouse_button_held(dk::MouseButton::Left);
		io.MouseDown[1] = m_input->get_mouse_button_held(dk::MouseButton::Right);
		io.MouseDown[2] = m_input->get_mouse_button_held(dk::MouseButton::Middle);

		glm::vec2 mouse_pos = m_input->get_mouse_position();
		io.MousePos = ImVec2(mouse_pos.x, mouse_pos.y);

		glm::vec2 mouse_wheel = m_input->get_mouse_wheel();
		if (mouse_wheel.x > 0) io.MouseWheelH += 1;
		if (mouse_wheel.x < 0) io.MouseWheelH -= 1;
		if (mouse_wheel.y > 0) io.MouseWheel += 1;
		if (mouse_wheel.y < 0) io.MouseWheel -= 1;

		// Update text input
		io.AddInputCharactersUTF8(m_input->get_text_input().data());

		// Update keyboard
		io.KeysDown[SDL_SCANCODE_TAB] = m_input->get_key_held(dk::KeyCode::Tab);
		io.KeysDown[SDL_SCANCODE_LEFT] = m_input->get_key_held(dk::KeyCode::Left);
		io.KeysDown[SDL_SCANCODE_RIGHT] = m_input->get_key_held(dk::KeyCode::Right);
		io.KeysDown[SDL_SCANCODE_UP] = m_input->get_key_held(dk::KeyCode::Up);
		io.KeysDown[SDL_SCANCODE_DOWN] = m_input->get_key_held(dk::KeyCode::Down);
		io.KeysDown[SDL_SCANCODE_PAGEUP] = m_input->get_key_held(dk::KeyCode::PageUp);
		io.KeysDown[SDL_SCANCODE_PAGEDOWN] = m_input->get_key_held(dk::KeyCode::PageDown);
		io.KeysDown[SDL_SCANCODE_HOME] = m_input->get_key_held(dk::KeyCode::Home);
		io.KeysDown[SDL_SCANCODE_END] = m_input->get_key_held(dk::KeyCode::End);
		io.KeysDown[SDL_SCANCODE_INSERT] = m_input->get_key_held(dk::KeyCode::Insert);
		io.KeysDown[SDL_SCANCODE_DELETE] = m_input->get_key_held(dk::KeyCode::Delete);
		io.KeysDown[SDL_SCANCODE_BACKSPACE] = m_input->get_key_held(dk::KeyCode::Backspace);
		io.KeysDown[SDL_SCANCODE_SPACE] = m_input->get_key_held(dk::KeyCode::Space);
		io.KeysDown[SDL_SCANCODE_RETURN] = m_input->get_key_held(dk::KeyCode::Return);
		io.KeysDown[SDL_SCANCODE_ESCAPE] = m_input->get_key_held(dk::KeyCode::Escape);
		io.KeysDown[SDL_SCANCODE_A] = m_input->get_key_held(dk::KeyCode::A);
		io.KeysDown[SDL_SCANCODE_C] = m_input->get_key_held(dk::KeyCode::C);
		io.KeysDown[SDL_SCANCODE_V] = m_input->get_key_held(dk::KeyCode::V);
		io.KeysDown[SDL_SCANCODE_X] = m_input->get_key_held(dk::KeyCode::X);
		io.KeysDown[SDL_SCANCODE_Y] = m_input->get_key_held(dk::KeyCode::Y);
		io.KeysDown[SDL_SCANCODE_Z] = m_input->get_key_held(dk::KeyCode::Z);
		io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
		io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
		io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
		io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

		// Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
		{
			ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
			if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
				SDL_ShowCursor(0);
			else
			{
				SDL_SetCursor(m_cursors[cursor] ? m_cursors[cursor] : m_cursors[ImGuiMouseCursor_Arrow]);
				SDL_ShowCursor(1);
			}
		}

		// Master frame
		ImGui::NewFrame();

		// Title bar
		m_toolbar->draw();

		// Docking
		const ImVec2 display_size = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowPos(ImVec2(0, 16));
		ImGui::SetNextWindowSize(ImVec2(display_size.x, display_size.y - 16));
		const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		const float oldWindowRounding = ImGui::GetStyle().WindowRounding; ImGui::GetStyle().WindowRounding = 0;
		const bool visible = ImGui::Begin("imguidock window (= lumix engine's dock system)", NULL, ImVec2(0, 0), 1.0f, flags);
		ImGui::GetStyle().WindowRounding = oldWindowRounding;
		if (visible)
		{
			ImGui::BeginDockspace();

			ImGui::SetNextDock(ImGuiDockSlot_Left);
			if(ImGui::BeginDock("Inspector"))
				m_inspector->draw();
			ImGui::EndDock();

			ImGui::SetNextDock(ImGuiDockSlot_Left);
			if(ImGui::BeginDock("Scene View"))
				m_scene_view->draw(dt);
			ImGui::EndDock();

			ImGui::SetNextDock(ImGuiDockSlot_Left);
			if(ImGui::BeginDock("Hierarchy"))
				m_hierarchy->draw();
			ImGui::EndDock();

			ImGui::EndDockspace();
		}
		ImGui::End();

		// End master frame
		ImGui::EndFrame();

		// Tell IMGUI to render
		ImGui::Render();
	}
}