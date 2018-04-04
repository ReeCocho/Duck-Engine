#include "math\common.hpp"
#include "graphics\graphics.hpp"
#include "graphics\forward_renderer.hpp"
#include "graphics\shader.hpp"

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

		while (running)
		{
			// Loop through events
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				if (evt.type == SDL_QUIT)
					running = false;
			}

			renderer.render();
		}

		shader.free();
	}

	std::cin.get();
	return 0;
}