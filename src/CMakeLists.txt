add_subdirectory(utilities)
add_subdirectory(graphics)
add_subdirectory(physics)
add_subdirectory(ecs)
add_subdirectory(editor)

# Headers
set(DUCK_ENGINE_HDRS
	engine.hpp
	input.hpp
	resource_manager.hpp
	archive.hpp
	components/mesh_renderer.hpp
	components/camera.hpp
	components/lights.hpp
	components/rigidbody.hpp
	components/character_controller.hpp
	components/transform.hpp
)

# Sources
set(DUCK_ENGINE_SRCS
	engine.cpp
	input.cpp
	resource_manager.cpp
	archive.cpp
	components/mesh_renderer.cpp
	components/camera.cpp
	components/lights.cpp
	components/rigidbody.cpp
	components/character_controller.cpp
	components/transform.cpp
)

# Engine lib
add_library(Duck-Engine
	${DUCK_ENGINE_HDRS}
	${DUCK_ENGINE_SRCS}
)