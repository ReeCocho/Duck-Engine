# Headers
set(DUCK_ECS_HDRS
	entity.hpp
	component.hpp
	system.hpp
	scene.hpp
)

# Sources
set(DUCK_ECS_SRCS
	entity.cpp
	component.cpp
	system.cpp
	scene.cpp
)

# ECS lib
add_library(Duck-ECS
	${DUCK_ECS_HDRS}
	${DUCK_ECS_SRCS}
)