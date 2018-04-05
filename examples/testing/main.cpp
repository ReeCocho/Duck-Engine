#include "math\common.hpp"
#include "graphics\graphics.hpp"
#include "graphics\forward_renderer.hpp"
#include "graphics\shader.hpp"
#include "graphics\mesh.hpp"

int main()
{
	{
		dk::Graphics graphics(1, "Test window", 1280, 720);
		dk::ForwardRenderer renderer(graphics);
		bool running = true;

		dk::Shader shader(	graphics, 
							renderer.get_shader_render_pass(), 
							dk::read_binary_file("shaders/standard.vert.spv"),
							dk::read_binary_file("shaders/standard.frag.spv"));

		dk::Mesh mesh(graphics,
		{},
		{
			{ dk::Vec3( 1,  1,  0),	dk::Vec2(0, 0) },
			{ dk::Vec3(-1,  1,  0),	dk::Vec2(0, 0) },
			{ dk::Vec3( 0, -1,  0),	dk::Vec2(0, 0) }
		});

		auto command_buffer = graphics.get_command_manager().allocate_command_buffer(vk::CommandBufferLevel::eSecondary);
		dk::RenderableObject renderable =
		{
			command_buffer,
			&shader,
			&mesh,
			{}
		};

		while (running)
		{
			// Loop through events
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				if (evt.type == SDL_QUIT)
					running = false;
			}

			renderer.draw(renderable);
			renderer.render();
		}

		// Wait for presentation to finish
		graphics.get_device_manager().get_present_queue().waitIdle();

		command_buffer.free();
		shader.free();
		mesh.free();
	}

	std::cin.get();
	return 0;
}