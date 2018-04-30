# SPIRV Cross files
set(SPIRV_CROSS_FILES
	spirv_cross/GLSL.std.450.h
	spirv_cross/spirv.hpp
	spirv_cross/spirv_cfg.hpp
	spirv_cross/spirv_common.hpp
	spirv_cross/spirv_cross.hpp
	spirv_cross/spirv_cross_util.hpp
	spirv_cross/spirv_glsl.hpp
	
	spirv_cross/spirv_cfg.cpp
	spirv_cross/spirv_cross.cpp
	spirv_cross/spirv_cross_util.cpp
	spirv_cross/spirv_glsl.cpp
)

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
	command_manager.hpp
	mesh.hpp
	material.hpp
	lighting.hpp
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
	command_manager.cpp
	mesh.cpp
	material.cpp
	lighting.cpp
)

# Graphics lib
add_library(Duck-Graphics
	${DUCK_GRAPHICS_HDRS}
	${DUCK_GRAPHICS_SRCS}
	${SPIRV_CROSS_FILES}
)
source_group("spirv_cross" FILES ${SPIRV_CROSS_FILES})