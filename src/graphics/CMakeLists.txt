# Headers
set(DUCK_GRAPHICS_HDRS
	graphics.hpp
	vulkan_utilities.hpp
	debugging.hpp
)

# Sources
set(DUCK_GRAPHICS_SRCS
	graphics.cpp
	vulkan_utilities.cpp
	debugging.cpp
)

# Graphics lib
add_library(Duck-Graphics
	${DUCK_GRAPHICS_HDRS}
	${DUCK_GRAPHICS_SRCS}
)