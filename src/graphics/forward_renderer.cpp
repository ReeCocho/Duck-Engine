/**
 * @file forward_renderer.cpp
 * @brief Duck forward+ renderer source.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes. */
#include <array>
#include <utilities\file_io.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "forward_renderer.hpp"

namespace dk
{
	ForwardRendererBase::ForwardRendererBase()
	{

	}

	ForwardRendererBase::ForwardRendererBase(Graphics* graphics, ResourceAllocator<Texture>* texture_allocator, ResourceAllocator<Mesh>* mesh_allocator) :
		Renderer(graphics),
		m_texture_allocator(texture_allocator),
		m_mesh_allocator(mesh_allocator)
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
		alloc_info.commandBufferCount = 3;
		auto command_buffers = get_graphics().get_logical_device().allocateCommandBuffers(alloc_info);

		m_vk_primary_command_buffer = command_buffers[0];
		m_vk_depth_prepass_command_buffer = command_buffers[1];
		m_vk_rendering_command_buffer = command_buffers[2];

		dk_assert(m_vk_primary_command_buffer);
		dk_assert(m_vk_depth_prepass_command_buffer);
		dk_assert(m_vk_rendering_command_buffer);

		// Create semaphores
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_semaphores.on_screen_rendering_finished = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		m_semaphores.depth_prepass_finished = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_semaphores.on_screen_rendering_finished);
		dk_assert(m_semaphores.depth_prepass_finished);

		// Create lighting manager
		m_lighting_manager = std::make_unique<LightingManager>(&get_graphics(), 128, 8);

		// Create depth prepass
		{
			// Describes depth attachment usage
			vk::AttachmentDescription attachment = {};
			attachment.format = find_best_depth_format(get_graphics().get_physical_device());
			attachment.samples = vk::SampleCountFlagBits::e1;
			attachment.loadOp = vk::AttachmentLoadOp::eClear;
			attachment.storeOp = vk::AttachmentStoreOp::eStore;
			attachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
			attachment.stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment.initialLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
			attachment.finalLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;

			vk::AttachmentReference attachment_ref = {};
			attachment_ref.attachment = 0;
			attachment_ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 0;
			subpass.pColorAttachments = nullptr;
			subpass.pDepthStencilAttachment = &attachment_ref;

			vk::SubpassDependency dependency = {};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
			dependency.srcAccessMask = vk::AccessFlagBits::eMemoryRead;
			dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
			dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

			vk::RenderPassCreateInfo render_pass_info = {};
			render_pass_info.attachmentCount = 1;
			render_pass_info.pAttachments = &attachment;
			render_pass_info.subpassCount = 1;
			render_pass_info.pSubpasses = &subpass;
			render_pass_info.dependencyCount = 1;
			render_pass_info.pDependencies = &dependency;

			m_render_passes.depth_prepass = get_graphics().get_logical_device().createRenderPass(render_pass_info);
		}

		// Create depth image
		{
			auto depth_format = find_best_depth_format(get_graphics().get_physical_device());

			// Depth attachment
			FrameBufferAttachment depth = get_graphics().create_attachment
			(
				depth_format,
				vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled
			);

			// Create sampler to sample from the attachments
			vk::SamplerCreateInfo sampler = {};
			sampler.setMagFilter(vk::Filter::eNearest);
			sampler.setMinFilter(vk::Filter::eNearest);
			sampler.setMipmapMode(vk::SamplerMipmapMode::eLinear);
			sampler.setAddressModeU(vk::SamplerAddressMode::eClampToEdge);
			sampler.setAddressModeV(sampler.addressModeU);
			sampler.setAddressModeW(sampler.addressModeU);
			sampler.setMipLodBias(0.0f);
			sampler.setMaxAnisotropy(1.0f);
			sampler.setMinLod(0.0f);
			sampler.setMaxLod(1.0f);
			sampler.setBorderColor(vk::BorderColor::eFloatOpaqueWhite);

			// Create sampler
			vk::Sampler depth_sampler = get_graphics().get_logical_device().createSampler(sampler);

			// Resize allocator if needed
			if (m_texture_allocator->num_allocated() + 1 > m_texture_allocator->max_allocated())
				m_texture_allocator->resize(m_texture_allocator->max_allocated() + 1);

			// Create attachments
			m_depth_prepass_image.depth_texture = Handle<Texture>(m_texture_allocator->allocate(), m_texture_allocator);

			::new(m_texture_allocator->get_resource_by_handle(m_depth_prepass_image.depth_texture.id))(Texture)
			(
				&get_graphics(),
				depth.image,
				depth.view,
				depth_sampler,
				depth.memory,
				vk::Filter::eNearest,
				get_graphics().get_width(),
				get_graphics().get_height()
			);

			vk::FramebufferCreateInfo fbuf_create_info = {};
			fbuf_create_info.pNext = nullptr;
			fbuf_create_info.renderPass = m_render_passes.depth_prepass;
			fbuf_create_info.pAttachments = &m_depth_prepass_image.depth_texture->get_image_view();
			fbuf_create_info.attachmentCount = 1;
			fbuf_create_info.width = get_graphics().get_width();
			fbuf_create_info.height = get_graphics().get_height();
			fbuf_create_info.layers = 1;

			// Create framebuffer
			m_depth_prepass_image.framebuffer = get_graphics().get_logical_device().createFramebuffer(fbuf_create_info);

			get_graphics().transition_image_layout
			(
				m_depth_prepass_image.depth_texture->get_image(), 
				depth_format, 
				vk::ImageLayout::eUndefined, 
				vk::ImageLayout::eDepthStencilAttachmentOptimal
			);
		}

		// Create descriptor set
		{
			std::array<vk::DescriptorSetLayoutBinding, 3> bindings = {};
			bindings[0].binding = 0;
			bindings[0].descriptorType = vk::DescriptorType::eStorageBuffer;
			bindings[0].descriptorCount = 1;
			bindings[0].stageFlags = vk::ShaderStageFlagBits::eFragment;

			bindings[1].binding = 1;
			bindings[1].descriptorType = vk::DescriptorType::eStorageBuffer;
			bindings[1].descriptorCount = 1;
			bindings[1].stageFlags = vk::ShaderStageFlagBits::eFragment;

			bindings[2].binding = 2;
			bindings[2].descriptorType = vk::DescriptorType::eUniformBuffer;
			bindings[2].descriptorCount = 1;
			bindings[2].stageFlags = vk::ShaderStageFlagBits::eFragment;

			vk::DescriptorSetLayoutCreateInfo layout_info = {};
			layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_info.pBindings = bindings.data();

			m_descriptor.layout = get_graphics().get_logical_device().createDescriptorSetLayout(layout_info);
			dk_assert(m_descriptor.layout);

			std::array<vk::DescriptorPoolSize, 3> pool_sizes = {};
			pool_sizes[0].type = vk::DescriptorType::eStorageBuffer;
			pool_sizes[0].descriptorCount = 1;

			pool_sizes[1].type = vk::DescriptorType::eStorageBuffer;
			pool_sizes[1].descriptorCount = 1;

			pool_sizes[2].type = vk::DescriptorType::eUniformBuffer;
			pool_sizes[2].descriptorCount = 1;

			// Create descriptor pool
			vk::DescriptorPoolCreateInfo pool_info = {};
			pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
			pool_info.pPoolSizes = pool_sizes.data();
			pool_info.maxSets = 1;

			m_descriptor.pool = get_graphics().get_logical_device().createDescriptorPool(pool_info);
			dk_assert(m_descriptor.pool);

			// Allocate descriptor set
			vk::DescriptorSetAllocateInfo alloc_info = {};
			alloc_info.descriptorPool = m_descriptor.pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &m_descriptor.layout;

			m_descriptor.set = get_graphics().get_logical_device().allocateDescriptorSets(alloc_info)[0];
			dk_assert(m_descriptor.set);

			// Update descriptor set
			vk::DescriptorBufferInfo buffer_info = {};
			buffer_info.buffer = m_lighting_manager->get_lighting_data_ubo().buffer;
			buffer_info.offset = 0;
			buffer_info.range = static_cast<uint32_t>(m_lighting_manager->get_lighting_data_size());

			vk::WriteDescriptorSet write = {};
			write.dstSet = m_descriptor.set;
			write.dstBinding = static_cast<uint32_t>(2);
			write.dstArrayElement = 0;
			write.descriptorType = vk::DescriptorType::eUniformBuffer;
			write.descriptorCount = 1;
			write.pBufferInfo = &buffer_info;

			get_graphics().get_logical_device().updateDescriptorSets(1, &write, 0, nullptr);
		}

		// Create thread pool
		m_thread_pool = std::make_unique<ThreadPool>(get_graphics().get_command_manager().get_pool_count());
	}

	void ForwardRendererBase::shutdown()
	{
		// Wait for logical device and graphics queue
		get_graphics().get_device_manager().get_graphics_queue().waitIdle();
		get_graphics().get_logical_device().waitIdle();

		// Destroy thread pool
		m_thread_pool->wait();
		m_thread_pool.reset();

		// Destroy descriptor set and pool
		get_graphics().get_logical_device().destroyDescriptorPool(m_descriptor.pool);
		get_graphics().get_logical_device().destroyDescriptorSetLayout(m_descriptor.layout);

		// Destroy lighting manager
		m_lighting_manager.reset();

		// Destroy semaphores
		get_graphics().get_logical_device().destroySemaphore(m_semaphores.on_screen_rendering_finished);
		get_graphics().get_logical_device().destroySemaphore(m_semaphores.depth_prepass_finished);

		// Depth depth image
		get_graphics().get_logical_device().destroyFramebuffer(m_depth_prepass_image.framebuffer);
		m_depth_prepass_image.depth_texture->free();
		m_texture_allocator->deallocate(m_depth_prepass_image.depth_texture.id);

		// Destroy command pool
		get_graphics().get_logical_device().destroyCommandPool(m_vk_command_pool);

		// Destroy render passes
		get_graphics().get_logical_device().destroyRenderPass(m_render_passes.shader_pass);
		get_graphics().get_logical_device().destroyRenderPass(m_render_passes.depth_prepass);
	}

	void ForwardRendererBase::render()
	{

	}

	void ForwardRendererBase::draw(const RenderableObject& obj) { m_renderable_objects.push_back(obj); }

	void ForwardRendererBase::flush_queues()
	{
		m_lighting_manager->flush_queues();
		m_renderable_objects.clear();
	}

	void ForwardRendererBase::upate_lighting_data()
	{
		// Submit lighting data
		m_lighting_manager->upload();

		// Update lighting descriptor sets
		std::array<vk::DescriptorBufferInfo, 2> buffer_infos = {};
		buffer_infos[0].buffer = m_lighting_manager->get_point_light_ssbo().buffer;
		buffer_infos[0].offset = 0;
		buffer_infos[0].range = static_cast<uint32_t>(m_lighting_manager->get_point_light_data_size());

		buffer_infos[1].buffer = m_lighting_manager->get_directional_light_ssbo().buffer;
		buffer_infos[1].offset = 0;
		buffer_infos[1].range = static_cast<uint32_t>(m_lighting_manager->get_directional_light_data_size());

		std::array<vk::WriteDescriptorSet, 2> writes = {};
		writes[0].dstSet = m_descriptor.set;
		writes[0].dstBinding = 0;
		writes[0].dstArrayElement = 0;
		writes[0].descriptorType = vk::DescriptorType::eStorageBuffer;
		writes[0].descriptorCount = 1;
		writes[0].pBufferInfo = &buffer_infos[0];

		writes[1].dstSet = m_descriptor.set;
		writes[1].dstBinding = 1;
		writes[1].dstArrayElement = 0;
		writes[1].descriptorType = vk::DescriptorType::eStorageBuffer;
		writes[1].descriptorCount = 1;
		writes[1].pBufferInfo = &buffer_infos[1];

		get_graphics().get_logical_device().updateDescriptorSets(static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
	}

	void ForwardRendererBase::generate_depth_prepass_command_buffer(vk::Extent2D extent)
	{
		// Begin command buffer
		vk::CommandBufferBeginInfo cmd_buf_info = {};
		cmd_buf_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;
		cmd_buf_info.pInheritanceInfo = nullptr;

		m_vk_depth_prepass_command_buffer.begin(cmd_buf_info);

		// Clear values for all attachments written in the fragment shader
		vk::ClearValue clear_value = {};
		clear_value.setDepthStencil({ 1.0f, 0 });

		// Begin render pass
		vk::RenderPassBeginInfo render_pass_begin_info = {};
		render_pass_begin_info.renderPass = m_render_passes.depth_prepass;
		render_pass_begin_info.framebuffer = m_depth_prepass_image.framebuffer;
		render_pass_begin_info.renderArea.extent = extent;
		render_pass_begin_info.renderArea.offset = vk::Offset2D(0, 0);
		render_pass_begin_info.clearValueCount = 1;
		render_pass_begin_info.pClearValues = &clear_value;

		m_vk_depth_prepass_command_buffer.beginRenderPass(render_pass_begin_info, vk::SubpassContents::eSecondaryCommandBuffers);

		// Inheritance info for the meshes command buffers
		vk::CommandBufferInheritanceInfo inheritance_info = {};
		inheritance_info.renderPass = m_render_passes.depth_prepass;
		inheritance_info.framebuffer = m_depth_prepass_image.framebuffer;

		// List of command buffers to execute
		std::vector<vk::CommandBuffer> command_buffers = {};

		// Draw skybox if allowed
		if (m_main_camera.sky_box.allocator && 
			m_main_camera.sky_box->get_material() != Handle<Material>() && 
			m_main_camera.sky_box->get_mesh() != Handle<Mesh>())
		{
			command_buffers.push_back(m_main_camera.command_buffers[1].get_command_buffer());
			draw_sky_box(m_main_camera.command_buffers[1], extent, inheritance_info, true);
		}

		// List of jobs.
		// First dimension is the queue.
		// Second dimension is the job list.
		std::vector<std::vector<std::function<void(void)>>> jobs(get_graphics().get_command_manager().get_pool_count());

		// Loop over every mesh
		for (size_t i = 0; i < m_renderable_objects.size(); ++i)
		{
			// Easy to reference
			auto& obj = m_renderable_objects[i];

			// Frustum culling
			AABB new_aabb = obj.mesh->get_aabb();
			new_aabb.transform(obj.model);

			if (!m_main_camera.frustum.check_inside(new_aabb)) continue;

			// Add command buffer to list
			auto& command_buffer = obj.command_buffers[1].get_command_buffer();
			command_buffers.push_back(command_buffer);

			// Create job
			jobs[obj.command_buffers[1].get_thread_index()].push_back(([this, &obj, command_buffer, inheritance_info, extent]()
			{
				// Begin command buffer
				vk::CommandBufferBeginInfo begin_info = {};
				begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue;
				begin_info.pInheritanceInfo = &inheritance_info;

				command_buffer.begin(begin_info);

				// Set viewport
				vk::Viewport viewport = {};
				viewport.setHeight(static_cast<float>(extent.height));
				viewport.setWidth(static_cast<float>(extent.width));
				viewport.setMinDepth(0);
				viewport.setMaxDepth(1);
				command_buffer.setViewport(0, 1, &viewport);

				// Set scissor
				vk::Rect2D scissor = {};
				scissor.extent = extent;
				scissor.setOffset({ 0, 0 });
				command_buffer.setScissor(0, 1, &scissor);

				// Bind shader
				command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, obj.shader->get_depth_pipeline());

				// Bind descriptor sets
				command_buffer.bindDescriptorSets
				(
					vk::PipelineBindPoint::eGraphics,
					obj.shader->get_depth_pipeline_layout(),
					0,
					static_cast<uint32_t>(obj.descriptor_sets.size()),
					obj.descriptor_sets.data(),
					0,
					nullptr
				);

				// Draw mesh
				const auto& mem_buffer = obj.mesh->get_vertex_buffer();
				vk::DeviceSize offsets[] = { 0 };

				command_buffer.bindVertexBuffers(0, 1, &mem_buffer.buffer, offsets);
				command_buffer.bindIndexBuffer(obj.mesh->get_index_buffer().buffer, 0, vk::IndexType::eUint16);
				command_buffer.drawIndexed(static_cast<uint32_t>(obj.mesh->get_index_count()), 1, 0, 0, 0);

				// End command buffer
				command_buffer.end();
			}));
		}

		// Add jobs to thread pool
		for (size_t i = 0; i < jobs.size(); ++i)
			m_thread_pool->workers[i]->add_jobs(jobs[i]);

		// Wait for threads to finish
		m_thread_pool->wait();

		// Execute command buffers
		if (command_buffers.size() > 0)
			m_vk_depth_prepass_command_buffer.executeCommands(command_buffers);

		// End render pas
		m_vk_depth_prepass_command_buffer.endRenderPass();

		// End command buffer
		m_vk_depth_prepass_command_buffer.end();
	}

	void ForwardRendererBase::generate_rendering_command_buffer(const vk::Framebuffer& framebuffer, vk::Extent2D extent)
	{
		// Begin recording to primary command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		m_vk_rendering_command_buffer.begin(begin_info);

		// Inheritance info for the meshes command buffers
		vk::CommandBufferInheritanceInfo inheritance_info = {};
		inheritance_info.renderPass = m_render_passes.shader_pass;
		inheritance_info.framebuffer = framebuffer;

		vk::ClearValue clear_color = {};
		clear_color.color = std::array<float, 4> { 0.0f, 0.0f, 0.0f, 1.0f };

		// Begin render pass
		vk::RenderPassBeginInfo render_pass_info = {};
		render_pass_info.renderPass = m_render_passes.shader_pass;
		render_pass_info.framebuffer = framebuffer;
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = extent;
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_color;

		m_vk_rendering_command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eSecondaryCommandBuffers);

		// Command buffers to execute
		std::vector<vk::CommandBuffer> command_buffers = {};

		// Draw skybox if allowed
		if (m_main_camera.sky_box != Handle<SkyBox>() &&
			m_main_camera.sky_box->get_material() != Handle<Material>() &&
			m_main_camera.sky_box->get_mesh() != Handle<Mesh>())
		{
			command_buffers.push_back(m_main_camera.command_buffers[0].get_command_buffer());
			draw_sky_box(m_main_camera.command_buffers[0], extent, inheritance_info, false);
		}

		// List of jobs.
		// First dimension is the queue.
		// Second dimension is the job list.
		std::vector<std::vector<std::function<void(void)>>> jobs(get_graphics().get_command_manager().get_pool_count());

		// Loop over every mesh
		for (size_t i = 0; i < m_renderable_objects.size(); ++i)
		{
			// Easy reference
			auto& obj = m_renderable_objects[i];

			// Frustum culling
			AABB new_aabb = obj.mesh->get_aabb();
			new_aabb.transform(obj.model);

			if (!m_main_camera.frustum.check_inside(new_aabb)) continue;

			// Add command buffer to list
			auto& command_buffer = obj.command_buffers[0].get_command_buffer();
			command_buffers.push_back(command_buffer);

			// Create job
			jobs[obj.command_buffers[0].get_thread_index()].push_back(([this, &obj, command_buffer, inheritance_info, extent]()
			{
				// Begin command buffer
				vk::CommandBufferBeginInfo begin_info = {};
				begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue;
				begin_info.pInheritanceInfo = &inheritance_info;

				command_buffer.begin(begin_info);

				// Set viewport
				vk::Viewport viewport = {};
				viewport.setHeight(static_cast<float>(extent.height));
				viewport.setWidth(static_cast<float>(extent.width));
				viewport.setMinDepth(0);
				viewport.setMaxDepth(1);
				command_buffer.setViewport(0, 1, &viewport);

				// Set scissor
				vk::Rect2D scissor = {};
				scissor.setExtent(extent);
				scissor.setOffset({ 0, 0 });
				command_buffer.setScissor(0, 1, &scissor);

				// Bind shader
				command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, obj.shader->get_graphics_pipeline());

				// Bind descriptor sets
				command_buffer.bindDescriptorSets
				(
					vk::PipelineBindPoint::eGraphics,
					obj.shader->get_graphics_pipeline_layout(),
					0,
					static_cast<uint32_t>(obj.descriptor_sets.size()),
					obj.descriptor_sets.data(),
					0,
					nullptr
				);

				// Draw mesh
				const auto& mem_buffer = obj.mesh->get_vertex_buffer();
				vk::DeviceSize offsets[] = { 0 };

				command_buffer.bindVertexBuffers(0, 1, &mem_buffer.buffer, offsets);
				command_buffer.bindIndexBuffer(obj.mesh->get_index_buffer().buffer, 0, vk::IndexType::eUint16);
				command_buffer.drawIndexed(static_cast<uint32_t>(obj.mesh->get_index_count()), 1, 0, 0, 0);

				// End command buffer
				command_buffer.end();
			}));
		}

		// Add jobs to worker threads
		for (size_t i = 0; i < jobs.size(); ++i)
			m_thread_pool->workers[i]->add_jobs(jobs[i]);

		// Wait for threads to finish
		m_thread_pool->wait();

		// Execute command buffers
		if (command_buffers.size() > 0)
			m_vk_rendering_command_buffer.executeCommands(command_buffers);

		// End render pass and command buffer
		m_vk_rendering_command_buffer.endRenderPass();
		m_vk_rendering_command_buffer.end();
	}

	void ForwardRendererBase::draw_sky_box(VkManagedCommandBuffer& managed_command_buffer, vk::Extent2D extent, vk::CommandBufferInheritanceInfo inheritance_info, bool depth_prepass)
	{
		// Update sky box data
		VertexShaderData data = {};
		data.model = glm::translate({}, m_main_camera.position);
		data.mvp = m_main_camera.vp_mat * data.model;
		m_main_camera.sky_box->set_vertex_data(data);

		auto& command_buffer = managed_command_buffer.get_command_buffer();

		// Descriptor sets
		std::vector<vk::DescriptorSet> descriptor_sets = 
		{ 
			m_main_camera.sky_box->get_descriptor_set(), 
			m_descriptor.set,
			m_main_camera.sky_box->get_material()->get_texture_descriptor_set() 
		};

		// Begin command buffer
		vk::CommandBufferBeginInfo begin_info = {};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse | vk::CommandBufferUsageFlagBits::eRenderPassContinue;
		begin_info.pInheritanceInfo = &inheritance_info;

		command_buffer.begin(begin_info);

		// Set viewport
		vk::Viewport viewport = {};
		viewport.setHeight(static_cast<float>(extent.height));
		viewport.setWidth(static_cast<float>(extent.width));
		viewport.setMinDepth(0);
		viewport.setMaxDepth(1);
		command_buffer.setViewport(0, 1, &viewport);

		// Set scissor
		vk::Rect2D scissor = {};
		scissor.setExtent(extent);
		scissor.setOffset({ 0, 0 });
		command_buffer.setScissor(0, 1, &scissor);

		// Bind shader and descriptor sets
		if (depth_prepass)
		{
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_main_camera.sky_box->get_material()->get_shader()->get_depth_pipeline());
			command_buffer.bindDescriptorSets
			(
				vk::PipelineBindPoint::eGraphics,
				m_main_camera.sky_box->get_material()->get_shader()->get_depth_pipeline_layout(),
				0,
				static_cast<uint32_t>(descriptor_sets.size()),
				descriptor_sets.data(),
				0,
				nullptr
			);
		}
		else
		{
			command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_main_camera.sky_box->get_material()->get_shader()->get_graphics_pipeline());
			command_buffer.bindDescriptorSets
			(
				vk::PipelineBindPoint::eGraphics,
				m_main_camera.sky_box->get_material()->get_shader()->get_graphics_pipeline_layout(),
				0,
				static_cast<uint32_t>(descriptor_sets.size()),
				descriptor_sets.data(),
				0,
				nullptr
			);
		}

		// Draw mesh
		const auto& mem_buffer = m_main_camera.sky_box->get_mesh()->get_vertex_buffer();
		vk::DeviceSize offsets[] = { 0 };

		command_buffer.bindVertexBuffers(0, 1, &mem_buffer.buffer, offsets);
		command_buffer.bindIndexBuffer(m_main_camera.sky_box->get_mesh()->get_index_buffer().buffer, 0, vk::IndexType::eUint16);
		command_buffer.drawIndexed(static_cast<uint32_t>(m_main_camera.sky_box->get_mesh()->get_index_count()), 1, 0, 0, 0);

		// End command buffer
		command_buffer.end();
	}



	ForwardRenderer::ForwardRenderer()
	{

	}

	ForwardRenderer::ForwardRenderer(Graphics* graphics, ResourceAllocator<Texture>* texture_allocator, ResourceAllocator<Mesh>* mesh_allocator) :
		ForwardRendererBase(graphics, texture_allocator, mesh_allocator),
		m_vk_framebuffers({})
	{
		m_swapchain_manager = std::make_unique<VkSwapchainManager>
		(
			get_graphics().get_physical_device(),
			get_graphics().get_logical_device(),
			get_graphics().get_surface(),
			get_graphics().get_width(),
			get_graphics().get_height()
		);

		// Create semaphore
		vk::SemaphoreCreateInfo semaphore_info = {};
		m_vk_image_available = get_graphics().get_logical_device().createSemaphore(semaphore_info);
		dk_assert(m_vk_image_available);

		// Resize framebuffers
		m_vk_framebuffers.resize(m_swapchain_manager->get_image_count());

		// Create shader pass
		{
			std::array<vk::AttachmentDescription, 2> attachment_descs = {};

			attachment_descs[0].format = get_swapchain_manager().get_image_format();
			attachment_descs[0].samples = vk::SampleCountFlagBits::e1;
			attachment_descs[0].loadOp = vk::AttachmentLoadOp::eClear;
			attachment_descs[0].storeOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[0].stencilLoadOp = vk::AttachmentLoadOp::eClear;
			attachment_descs[0].stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[0].initialLayout = vk::ImageLayout::eUndefined;
			attachment_descs[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;

			attachment_descs[1].format = find_best_depth_format(get_graphics().get_physical_device());
			attachment_descs[1].samples = vk::SampleCountFlagBits::e1;
			attachment_descs[1].loadOp = vk::AttachmentLoadOp::eLoad;
			attachment_descs[1].storeOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[1].stencilLoadOp = vk::AttachmentLoadOp::eLoad;
			attachment_descs[1].stencilStoreOp = vk::AttachmentStoreOp::eStore;
			attachment_descs[1].initialLayout = vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal;
			attachment_descs[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			std::array<vk::AttachmentReference, 2> attachment_refs = {};
			attachment_refs[0].attachment = 0;
			attachment_refs[0].layout = vk::ImageLayout::eColorAttachmentOptimal;

			attachment_refs[1].attachment = 1;
			attachment_refs[1].layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			vk::SubpassDescription subpass = {};
			subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &attachment_refs[0];
			subpass.pDepthStencilAttachment = &attachment_refs[1];

			// Use subpass dependencies for attachment layput transitions
			std::array<vk::SubpassDependency, 2> dependencies =
			{
				vk::SubpassDependency
				(
					VK_SUBPASS_EXTERNAL,
					0,
					vk::PipelineStageFlagBits::eBottomOfPipe,
					vk::PipelineStageFlagBits::eColorAttachmentOutput,
					vk::AccessFlagBits::eMemoryRead,
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
					vk::DependencyFlagBits::eByRegion
				),
				vk::SubpassDependency
				(
					0,
					VK_SUBPASS_EXTERNAL,
					vk::PipelineStageFlagBits::eColorAttachmentOutput,
					vk::PipelineStageFlagBits::eBottomOfPipe,
					vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
					vk::AccessFlagBits::eMemoryRead,
					vk::DependencyFlagBits::eByRegion
				)
			};

			// Create render pass
			vk::RenderPassCreateInfo render_pass_info = {};
			render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_descs.size());
			render_pass_info.pAttachments = attachment_descs.data();
			render_pass_info.subpassCount = 1;
			render_pass_info.pSubpasses = &subpass;
			render_pass_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
			render_pass_info.pDependencies = dependencies.data();

			m_render_passes.shader_pass = get_graphics().get_logical_device().createRenderPass(render_pass_info);
		}

		// Create framebuffers
		for (size_t i = 0; i < m_vk_framebuffers.size(); i++)
		{
			std::array<vk::ImageView, 2> attachments =
			{
				get_swapchain_manager().get_image_view(i),
				m_depth_prepass_image.depth_texture->get_image_view()
			};

			vk::FramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.renderPass = m_render_passes.shader_pass;
			framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = get_swapchain_manager().get_image_extent().width;
			framebuffer_info.height = get_swapchain_manager().get_image_extent().height;
			framebuffer_info.layers = 1;

			m_vk_framebuffers[i] = get_graphics().get_logical_device().createFramebuffer(framebuffer_info);
			dk_assert(m_vk_framebuffers[i]);
		}
	}

	void ForwardRenderer::shutdown()
	{
		// Wait for presentation to finish
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		ForwardRendererBase::shutdown();

		// Destroy semaphore
		get_graphics().get_logical_device().destroySemaphore(m_vk_image_available);

		// Destroy framebuffers
		for (auto& framebuffer : m_vk_framebuffers)
			get_graphics().get_logical_device().destroyFramebuffer(framebuffer);

		// Destroy swapchain
		m_swapchain_manager.reset();
	}

	void ForwardRenderer::render()
	{
		// Wait for present queue to finish if needed
		get_graphics().get_device_manager().get_present_queue().waitIdle();

		// Get image index to render too
		uint32_t image_index = get_graphics().get_logical_device().acquireNextImageKHR
		(
			get_swapchain_manager().get_swapchain(),
			std::numeric_limits<uint64_t>::max(),
			m_vk_image_available,
			vk::Fence()
		).value;

		// Update lights
		upate_lighting_data();

		// Window extent
		vk::Extent2D extent = {};
		extent.width = static_cast<uint32_t>(get_graphics().get_width());
		extent.height = static_cast<uint32_t>(get_graphics().get_height());

		// Perform depth prepass
		generate_depth_prepass_command_buffer(extent);
		{
			vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eAllGraphics;

			vk::SubmitInfo submit_info = {};
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &m_vk_depth_prepass_command_buffer;
			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores = &m_semaphores.depth_prepass_finished;
			submit_info.pWaitDstStageMask = &wait_stage;
			submit_info.pWaitSemaphores = &m_vk_image_available;
			submit_info.waitSemaphoreCount = 1;

			// Submit draw command
			get_graphics().get_device_manager().get_graphics_queue().submit(1, &submit_info, { nullptr });
		}

		// Prepare rendering command buffer
		generate_rendering_command_buffer(m_vk_framebuffers[image_index], extent);
		{
			vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

			vk::SubmitInfo submit_info = {};
			submit_info.waitSemaphoreCount = 1;
			submit_info.pWaitSemaphores = &m_semaphores.depth_prepass_finished;
			submit_info.pWaitDstStageMask = &wait_stage;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &m_vk_rendering_command_buffer;
			submit_info.signalSemaphoreCount = 1;
			submit_info.pSignalSemaphores = &m_semaphores.on_screen_rendering_finished;

			// Submit to queue
			get_graphics().get_device_manager().get_graphics_queue().submit(submit_info, vk::Fence());
		}

		// Wait for graphics queue to finish rendering
		get_graphics().get_device_manager().get_graphics_queue().waitIdle();

		// Present to screen
		{
			vk::PresentInfoKHR present_info = {};
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &m_semaphores.on_screen_rendering_finished;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &get_swapchain_manager().get_swapchain();
			present_info.pImageIndices = &image_index;

			get_graphics().get_device_manager().get_present_queue().presentKHR(present_info);
		}

		// Clear rendering queues
		flush_queues();
	}
}