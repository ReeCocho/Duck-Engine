# Headers
set(DUCK_ECS_HDRS
	entity.hpp
	component.hpp
	system.hpp
	scene.hpp
	component_archive.hpp
)

# Sources
set(DUCK_ECS_SRCS
	entity.cpp
	component.cpp
	system.cpp
	scene.cpp
	component_archive.cpp
)

# ECS lib
add_library(Duck-ECS
	${DUCK_ECS_HDRS}
	${DUCK_ECS_SRCS}
)