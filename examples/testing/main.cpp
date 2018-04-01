#include "math\common.hpp"
#include "graphics\graphics.hpp"

int main()
{
	{
		dk::Graphics graphics("Test window", 1280, 720);
		bool running = true;

		while (running)
		{
			// Loop through events
			SDL_Event evt;
			while (SDL_PollEvent(&evt))
			{
				if (evt.type == SDL_QUIT)
					running = false;
			}
		}
	}

	std::cin.get();
	return 0;
}