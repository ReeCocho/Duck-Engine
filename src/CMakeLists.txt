add_subdirectory(utilities)
add_subdirectory(graphics)
add_subdirectory(ecs)


# Headers
set(DUCK_ENGINE_HDRS
	engine.hpp
	input.hpp
	components/mesh_renderer.hpp
)

# Sources
set(DUCK_ENGINE_SRCS
	engine.cpp
	input.cpp
	components/mesh_renderer.cpp
)

# Engine lib
add_library(Duck-Engine
	${DUCK_ENGINE_HDRS}
	${DUCK_ENGINE_SRCS}
)