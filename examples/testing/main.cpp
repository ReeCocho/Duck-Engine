#include <engine.hpp>
#include <graphics\shader.hpp>
#include <graphics\mesh.hpp>
#include <graphics\material.hpp>

int main()
{
	dk::engine::initialize(1, "Test Window", 1280, 720);
	dk::engine::simulate();
	dk::engine::shutdown();

	std::cin.get();
	return 0;
}