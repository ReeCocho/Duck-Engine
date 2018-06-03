# Headers
set(DUCK_STANDARD_COMPONENTS_HDRS
	camera.hpp
	character_controller.hpp
	lights.hpp
	mesh_renderer.hpp
	rigidbody.hpp
	transform.hpp
)

# Sources
set(DUCK_STANDARD_COMPONENTS_SRCS
	camera.cpp
	character_controller.cpp
	lights.cpp
	mesh_renderer.cpp
	rigidbody.cpp
	transform.cpp
)

# Standard components lib
add_library(Duck-Standard-Components
	${DUCK_STANDARD_COMPONENTS_HDRS}
	${DUCK_STANDARD_COMPONENTS_SRCS}
)