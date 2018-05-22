/**
 * @file editor_renderer.cpp
 * @brief Duck editor renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <utilities\file_io.hpp>
#include <config.hpp>
#include "imgui\imgui.h"
#include "editor_renderer.hpp"

namespace dk
{
	EditorRenderer::EditorRenderer()
	{

	}

	EditorRenderer::EditorRenderer(Graphics* graphics, ResourceAllocator<Texture>* texture_alloc) :
		Renderer(graphics),
		m_texture_allocator(texture_alloc),
		m_vk_framebuffers({})
	{
		// Create command pool
		vk::CommandPoolCreateInfo pool_info = {};
		pool_info.queueFamilyIndex = get_graphics().get_device_manager().get_queue_family_indices().graphics_family;
		pool_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		m_vk_command_pool = get_graphics().get_logical_device().createCommandPool(pool_info);
		dk_assert(m_vk_command_pool);

		// Create command buffer
		vk::CommandBufferAllocateInfo alloc_info = {};
		alloc_info.commandPool = m_vk_command_pool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;
		auto command_buffers = get_graphics().get_logical_device().allocateCommandBuffers(alloc_info);

		m_vk_primary_command_buffer = command_buffers[0];

		m_swapchain_manager = std::make_unique<VkSwapchainManager>
		(
			get_graphics().get_physical_device(),
			get_graphics().get_logical_device(),
			get_graphics().get_surface(),
			get_graphics().get_width(),
			get_graphics().get_height()
		);

		// Create semaphores
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_vk_image_available = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		m_vk_rendering_finished = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);
		dk_assert(m_vk_rendering_finished);

		// Create the render pass
		{
			vk::AttachmentDescription attachment = {};
			attachment.format = m_swapchain_manager->get_image_format();
			attachment.samples = vk::SampleCountFlagBits::e1;
			attachment.loadOp = vk::AttachmentLoadOp::eClear;
			attachment.storeOp = vk::AttachmentStoreOp::eStore;
			attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
			attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
			attachment.initialLayout = vk::ImageLayout::eUndefined;
			attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

			vk::AttachmentReference color_attachment = {};
			color_attachment.attachment = 0;
			color_attachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &color_attachment;

			vk::SubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;

			vk::RenderPassCreateInfo info = {};
			info.attachmentCount = 1;
			info.pAttachments = &attachment;
			info.subpassCount = 1;
			info.pSubpasses = &subpass;
			info.dependencyCount = 1;
			info.pDependencies = &dependency;

			m_vk_render_pass = graphics->get_logical_device().createRenderPass(info);
			dk_assert(m_vk_render_pass);
		}

		// Create framebuffers
		m_vk_framebuffers.resize(m_swapchain_manager->get_image_count());
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++)
		{
			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_vk_render_pass;
			framebuffer_info.attachmentCount = 1;
			framebuffer_info.pAttachments = &get_swapchain_manager().get_image_view(i);
			framebuffer_info.width = get_swapchain_manager().get_image_extent().width;
			framebuffer_info.height = get_swapchain_manager().get_image_extent().height;
			framebuffer_info.layers = 1;

			m_vk_framebuffers[i] = get_graphics().get_logical_device().createFramebuffer(framebuffer_info);
			dk_assert(m_vk_framebuffers[i]);
		}

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
			get_graphics().create_image
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
			vk::ImageView view = get_graphics().create_image_view(image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
			dk_assert(view);

			// Create staging buffer
			VkMemBuffer staging_buffer = get_graphics().create_buffer
			(
				static_cast<vk::DeviceSize>(upload_size),
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostVisible
			);

			// Transition to transfer mode
			get_graphics().transition_image_layout(image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

			// Upload to buffer
			void* map = get_graphics().get_logical_device().mapMemory(staging_buffer.memory, 0, upload_size);
			memcpy(map, pixels, upload_size);
			vk::MappedMemoryRange range[1] = {};
			range[0].memory = staging_buffer.memory;
			range[0].size = upload_size;
			get_graphics().get_logical_device().flushMappedMemoryRanges(1, range);
			get_graphics().get_logical_device().unmapMemory(staging_buffer.memory);

			// Copy image data
			get_graphics().copy_buffer_to_image(staging_buffer.buffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

			// Transition back to sampling
			get_graphics().transition_image_layout(image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

			// Destroy staging buffer
			staging_buffer.free(get_graphics().get_logical_device());

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
			vk::Sampler sampler = get_graphics().get_logical_device().createSampler(sampler_info);

			// Create texture
			if (m_texture_allocator->num_allocated() + 1 > m_texture_allocator->max_allocated())
				m_texture_allocator->resize(m_texture_allocator->max_allocated() + 16);

			m_font_texture = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator);
			::new(m_texture_allocator->get_resource_by_handle(m_font_texture.id))(Texture)
			(
				&get_graphics(),
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

		// Create font descriptor
		{
			// Create descriptor pool
			vk::DescriptorPoolSize pool_size = { vk::DescriptorType::eCombinedImageSampler, 1 };

			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
			pool_info.maxSets = 1;
			pool_info.poolSizeCount = 1;
			pool_info.pPoolSizes = &pool_size;
			m_font_descriptor.pool = get_graphics().get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_font_descriptor.pool);

			// Create descriptor set layout
			vk::DescriptorSetLayoutBinding binding = {};
			binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			binding.descriptorCount = 1;
			binding.stageFlags = vk::ShaderStageFlagBits::eFragment;
			binding.pImmutableSamplers = &m_font_texture->get_sampler();

			vk::DescriptorSetLayoutCreateInfo info = {};
			info.bindingCount = 1;
			info.pBindings = &binding;
			m_font_descriptor.layout = get_graphics().get_logical_device().createDescriptorSetLayout(info);
			dk_assert(m_font_descriptor.layout);

			// Create descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_font_descriptor.pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_font_descriptor.layout;
			m_font_descriptor.set = get_graphics().get_logical_device().allocateDescriptorSets(alloc_info)[0];
			dk_assert(m_font_descriptor.set);
		}

		// Create UI shader
		{
			ShaderCreateInfo info = {};
			info.stage_flags = vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eVertex;
			
			// Render pass
			info.pipeline_create_info.render_pass = m_vk_render_pass;

			// Push constants
			info.pipeline_create_info.push_constant_ranges.push_back({});
			info.pipeline_create_info.push_constant_ranges[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
			info.pipeline_create_info.push_constant_ranges[0].offset = sizeof(float) * 0;
			info.pipeline_create_info.push_constant_ranges[0].size = sizeof(float) * 4;

			// Descriptor set layout
			info.pipeline_create_info.descriptor_set_layouts.push_back(m_font_descriptor.layout);

			// Vertex input
			vk::VertexInputBindingDescription binding_desc = {};
			binding_desc.stride = sizeof(ImDrawVert);
			binding_desc.inputRate = vk::VertexInputRate::eVertex;

			std::array<vk::VertexInputAttributeDescription, 3> attribute_desc = {};
			attribute_desc[0].location = 0;
			attribute_desc[0].binding = binding_desc.binding;
			attribute_desc[0].format = vk::Format::eR32G32Sfloat;
			attribute_desc[0].offset = (size_t)(&((ImDrawVert*)0)->pos);
			attribute_desc[1].location = 1;
			attribute_desc[1].binding = binding_desc.binding;
			attribute_desc[1].format = vk::Format::eR32G32Sfloat;
			attribute_desc[1].offset = (size_t)(&((ImDrawVert*)0)->uv);
			attribute_desc[2].location = 2;
			attribute_desc[2].binding = binding_desc.binding;
			attribute_desc[2].format = vk::Format::eR8G8B8A8Unorm;
			attribute_desc[2].offset = (size_t)(&((ImDrawVert*)0)->col);

			info.pipeline_create_info.vertex_input_info.vertexBindingDescriptionCount = 1;
			info.pipeline_create_info.vertex_input_info.pVertexBindingDescriptions = &binding_desc;
			info.pipeline_create_info.vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_desc.size());
			info.pipeline_create_info.vertex_input_info.pVertexAttributeDescriptions = attribute_desc.data();

			// Input assembly
			info.pipeline_create_info.input_assembly.topology = vk::PrimitiveTopology::eTriangleList;

			// Viewport
			info.pipeline_create_info.viewport_state.viewportCount = 1;
			info.pipeline_create_info.viewport_state.scissorCount = 1;

			// Rasterization
			info.pipeline_create_info.rasterizer.polygonMode = vk::PolygonMode::eFill;
			info.pipeline_create_info.rasterizer.cullMode = vk::CullModeFlagBits::eNone;
			info.pipeline_create_info.rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
			info.pipeline_create_info.rasterizer.lineWidth = 1.0f;

			// Multi sampling
			info.pipeline_create_info.multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

			// Color blending
			vk::PipelineColorBlendAttachmentState color_attachment = {};
			color_attachment.blendEnable = true;
			color_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
			color_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
			color_attachment.colorBlendOp = vk::BlendOp::eAdd;
			color_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
			color_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
			color_attachment.alphaBlendOp = vk::BlendOp::eAdd;
			color_attachment.colorWriteMask =	vk::ColorComponentFlagBits::eR | 
												vk::ColorComponentFlagBits::eG | 
												vk::ColorComponentFlagBits::eB | 
												vk::ColorComponentFlagBits::eA;

			VkPipelineColorBlendStateCreateInfo blend_info = {};
			info.pipeline_create_info.color_blending.attachmentCount = 1;
			info.pipeline_create_info.color_blending.pAttachments = &color_attachment;

			// Dynamic states
			std::array<vk::DynamicState, 2> dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
			info.pipeline_create_info.dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
			info.pipeline_create_info.dynamic_state.pDynamicStates = dynamic_states.data();

			m_ui_shader = std::make_unique<Shader>
			(
				&get_graphics(),
				std::vector<ShaderCreateInfo> { info },
				read_binary_file(DK_EDITOR_VERTEX_SHADER),
				read_binary_file(DK_EDITOR_FRAGMENT_SHADER)
			);
		}

		// Update the Descriptor Set:
		{
			vk::DescriptorImageInfo desc_image = {};
			desc_image.sampler = m_font_texture->get_sampler();
			desc_image.imageView = m_font_texture->get_image_view();
			desc_image.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

			vk::WriteDescriptorSet write_desc = {};
			write_desc.dstSet = m_font_descriptor.set;
			write_desc.descriptorCount = 1;
			write_desc.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			write_desc.pImageInfo = &desc_image;

			get_graphics().get_logical_device().updateDescriptorSets(write_desc, {});
		}
	}

	void EditorRenderer::shutdown()
	{
		// Wait for presentation to finish
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		// Destroy shader
		m_ui_shader->free();
		m_ui_shader.reset();

		// Destroy buffers
		m_draw_data.index_buffer.free(get_graphics().get_logical_device());
		m_draw_data.vertex_buffer.free(get_graphics().get_logical_device());

		// Destroy descriptor
		get_graphics().get_logical_device().destroyDescriptorSetLayout(m_font_descriptor.layout);
		get_graphics().get_logical_device().destroyDescriptorPool(m_font_descriptor.pool);

		// Destroy render pass
		get_graphics().get_logical_device().destroyRenderPass(m_vk_render_pass);

		// Destroy command pool
		get_graphics().get_logical_device().destroyCommandPool(m_vk_command_pool);

		// Destroy semaphores
		get_graphics().get_logical_device().destroySemaphore(m_vk_image_available);
		get_graphics().get_logical_device().destroySemaphore(m_vk_rendering_finished);

		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			get_graphics().get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy swapchain
		m_swapchain_manager.reset();
	}

	void EditorRenderer::render()
	{
		// Wait for present queue to finish if needed
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		// Get IMGUI
		ImGuiIO& io = ImGui::GetIO();

		// Get draw data
		ImDrawData* draw_data = ImGui::GetDrawData();

		if (!draw_data || draw_data->TotalVtxCount == 0)
			return;

		// Get image index to render too
		uint32_t image_index = get_graphics().get_logical_device().acquireNextImageKHR
		(
			get_swapchain_manager().get_swapchain(),
			std::numeric_limits<uint64_t>::max(),
			m_vk_image_available,
			vk::Fence()
		).value;

		// Update vertex and index buffers
		{
			const vk::DeviceSize allignment = 256;

			// Create the Vertex Buffer:
			size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
			if (!m_draw_data.vertex_buffer.buffer || m_draw_data.vertex_buffer_size < vertex_size)
			{
				// Free old buffer
				if (m_draw_data.vertex_buffer.buffer)
					get_graphics().get_logical_device().destroyBuffer(m_draw_data.vertex_buffer.buffer);

				if (m_draw_data.vertex_buffer.memory)
					get_graphics().get_logical_device().freeMemory(m_draw_data.vertex_buffer.memory);

				// Create new buffer
				VkDeviceSize vertex_buffer_size = ((vertex_size - 1) / allignment + 1) * allignment;

				m_draw_data.vertex_buffer = get_graphics().create_buffer
				(
					vertex_buffer_size, 
					vk::BufferUsageFlagBits::eVertexBuffer, 
					vk::MemoryPropertyFlagBits::eHostVisible
				);

				m_draw_data.vertex_buffer_size = static_cast<size_t>(vertex_buffer_size);
			}

			// Create the Index Buffer:
			size_t index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
			if (!m_draw_data.index_buffer.buffer || m_draw_data.index_buffer_size < index_size)
			{
				// Free old buffer
				if (m_draw_data.index_buffer.buffer)
					get_graphics().get_logical_device().destroyBuffer(m_draw_data.index_buffer.buffer);

				if (m_draw_data.index_buffer.memory)
					get_graphics().get_logical_device().freeMemory(m_draw_data.index_buffer.memory);

				// Create new buffer
				vk::DeviceSize index_buffer_size = ((index_size - 1) / allignment + 1) * allignment;

				m_draw_data.index_buffer = get_graphics().create_buffer
				(
					index_buffer_size,
					vk::BufferUsageFlagBits::eIndexBuffer,
					vk::MemoryPropertyFlagBits::eHostVisible
				);

				m_draw_data.index_buffer_size = static_cast<size_t>(index_buffer_size);
			}

			// Upload Vertex and index Data:
			{
				// Map memory
				ImDrawVert* vtx_dst = (ImDrawVert*)get_graphics().get_logical_device().mapMemory(m_draw_data.vertex_buffer.memory, 0, vertex_size);
				ImDrawIdx* idx_dst = (ImDrawIdx*)get_graphics().get_logical_device().mapMemory(m_draw_data.index_buffer.memory, 0, index_size);;

				// Copy data
				for (int n = 0; n < draw_data->CmdListsCount; ++n)
				{
					const ImDrawList* cmd_list = draw_data->CmdLists[n];
					memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
					memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
					vtx_dst += cmd_list->VtxBuffer.Size;
					idx_dst += cmd_list->IdxBuffer.Size;
				}

				// Unmap memory
				std::array<vk::MappedMemoryRange, 2> range = {};
				range[0].memory = m_draw_data.vertex_buffer.memory;
				range[0].size = VK_WHOLE_SIZE;
				range[1].memory = m_draw_data.index_buffer.memory;
				range[1].size = VK_WHOLE_SIZE;

				get_graphics().get_logical_device().flushMappedMemoryRanges(range);
				get_graphics().get_logical_device().unmapMemory(m_draw_data.vertex_buffer.memory);
				get_graphics().get_logical_device().unmapMemory(m_draw_data.index_buffer.memory);
			}
		}

		// Render everything
		{
			// Begin command buffer
			vk::CommandBufferBeginInfo cmd_buf_info = {};
			cmd_buf_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
			cmd_buf_info.pInheritanceInfo = nullptr;

			m_vk_primary_command_buffer.begin(cmd_buf_info);

			// Clear values for all attachments written in the fragment shader
			vk::ClearValue clear_value = {};
			clear_value.color = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 0.0f };

			// Window extent
			vk::Extent2D extent = {};
			extent.width = static_cast<uint32_t>(get_graphics().get_width());
			extent.height = static_cast<uint32_t>(get_graphics().get_height());

			// Set viewport
			vk::Viewport viewport = {};
			viewport.setHeight(static_cast<float>(extent.height));
			viewport.setWidth(static_cast<float>(extent.width));
			viewport.setMinDepth(0);
			viewport.setMaxDepth(1);
			m_vk_primary_command_buffer.setViewport(0, 1, &viewport);

			// Set scissor
			vk::Rect2D scissor = {};
			scissor.setExtent(extent);
			scissor.setOffset({ 0, 0 });
			m_vk_primary_command_buffer.setScissor(0, 1, &scissor);

			// Begin render pass
			vk::RenderPassBeginInfo render_pass_begin_info = {};
			render_pass_begin_info.renderPass = m_vk_render_pass;
			render_pass_begin_info.framebuffer = m_vk_framebuffers[image_index];
			render_pass_begin_info.renderArea.extent = extent;
			render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
			render_pass_begin_info.clearValueCount = 1;
			render_pass_begin_info.pClearValues = &clear_value;

			m_vk_primary_command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eInline);

			// Bind pipeline and descriptor sets:
			{
				m_vk_primary_command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_ui_shader->get_pipeline(0).pipeline);
				m_vk_primary_command_buffer.bindDescriptorSets
				(
					vk::PipelineBindPoint::eGraphics,
					m_ui_shader->get_pipeline(0).layout, 
					0,
					m_font_descriptor.set,
					{}
				);
			}

			// Bind Vertex And Index Buffer:
			{
				vk::DeviceSize vertex_offset = 0;
				m_vk_primary_command_buffer.bindVertexBuffers(0, m_draw_data.vertex_buffer.buffer, vertex_offset);
				m_vk_primary_command_buffer.bindIndexBuffer(m_draw_data.index_buffer.buffer, 0, vk::IndexType::eUint16);
			}

			// Setup scale and translation:
			{
				std::array<float, 2> scale = {};
				scale[0] = 2.0f / io.DisplaySize.x;
				scale[1] = 2.0f / io.DisplaySize.y;
				
				std::array<float, 2> translate = {};
				translate[0] = -1.0f;
				translate[1] = -1.0f;

				m_vk_primary_command_buffer.pushConstants<float>(m_ui_shader->get_pipeline(0).layout, vk::ShaderStageFlagBits::eVertex, 0, scale);
				m_vk_primary_command_buffer.pushConstants<float>(m_ui_shader->get_pipeline(0).layout, vk::ShaderStageFlagBits::eVertex, sizeof(float) * 2, translate);
			}

			// Render the command lists:
			int vtx_offset = 0;
			int idx_offset = 0;
			for (int n = 0; n < draw_data->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = draw_data->CmdLists[n];
				for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
					if (pcmd->UserCallback)
					{
						pcmd->UserCallback(cmd_list, pcmd);
					}
					else
					{
						vk::Rect2D scissor;
						scissor.offset.x = (int32_t)(pcmd->ClipRect.x) > 0 ? (int32_t)(pcmd->ClipRect.x) : 0;
						scissor.offset.y = (int32_t)(pcmd->ClipRect.y) > 0 ? (int32_t)(pcmd->ClipRect.y) : 0;
						scissor.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
						scissor.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y + 1); // FIXME: Why +1 here?
						m_vk_primary_command_buffer.setScissor(0, scissor);
						m_vk_primary_command_buffer.drawIndexed(pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
					}
					idx_offset += pcmd->ElemCount;
				}
				vtx_offset += cmd_list->VtxBuffer.Size;
			}

			m_vk_primary_command_buffer.endRenderPass();
			m_vk_primary_command_buffer.end();

			vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eAllGraphics;

			vk::SubmitInfo submit_info = {};
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &m_vk_primary_command_buffer;
			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores = &m_vk_rendering_finished;
			submit_info.pWaitDstStageMask = &wait_stage;
			submit_info.pWaitSemaphores = &m_vk_image_available;
			submit_info.waitSemaphoreCount = 1;

			// Submit draw command
			get_graphics().get_device_manager().get_graphics_queue().submit(1, &submit_info, { nullptr });
		}

		// Wait for graphics queue to finish rendering
		get_graphics().get_device_manager().get_graphics_queue().waitIdle();

		// Present to screen
		{
			vk::PresentInfoKHR present_info = {};
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &m_vk_rendering_finished;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &get_swapchain_manager().get_swapchain();
			present_info.pImageIndices = &image_index;

			get_graphics().get_device_manager().get_present_queue().presentKHR(present_info);
		}

		// ImGuiIO& io = ImGui::GetIO();
		// ImGui::NewFrame();
	}
}