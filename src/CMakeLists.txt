add_subdirectory(utilities)
add_subdirectory(graphics)
add_subdirectory(ecs)


# Headers
set(DUCK_ENGINE_HDRS
	engine.hpp
	input.hpp
	resource_manager.hpp
	components/mesh_renderer.hpp
	components/camera.hpp
	components/lights.hpp
)

# Sources
set(DUCK_ENGINE_SRCS
	engine.cpp
	input.cpp
	resource_manager.cpp
	components/mesh_renderer.cpp
	components/camera.cpp
	components/lights.cpp
)

# Engine lib
add_library(Duck-Engine
	${DUCK_ENGINE_HDRS}
	${DUCK_ENGINE_SRCS}
)