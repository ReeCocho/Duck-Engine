# Headers
set(DUCK_GRAPHICS_HDRS
	graphics.hpp
	vulkan_utilities.hpp
	debugging.hpp
	device_manager.hpp
	renderer.hpp
	swapchain_manager.hpp
	texture.hpp
	shader.hpp
	forward_renderer.hpp
	command_manager.hpp
)

# Sources
set(DUCK_GRAPHICS_SRCS
	graphics.cpp
	vulkan_utilities.cpp
	debugging.cpp
	device_manager.cpp
	renderer.cpp
	swapchain_manager.cpp
	texture.cpp
	shader.cpp
	forward_renderer.cpp
	command_manager.cpp
)

# Graphics lib
add_library(Duck-Graphics
	${DUCK_GRAPHICS_HDRS}
	${DUCK_GRAPHICS_SRCS}
)