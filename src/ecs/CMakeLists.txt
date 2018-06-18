# Headers
set(DUCK_ECS_HDRS
	entity.hpp
	component.hpp
	system.hpp
	scene.hpp
	entity.imp.hpp
	component.imp.hpp
	system.imp.hpp
	scene.imp.hpp
)

# Sources
set(DUCK_ECS_SRCS
	entity.cpp
	system.cpp
	scene.cpp
)

# ECS lib
add_library(Duck-ECS
	${DUCK_ECS_HDRS}
	${DUCK_ECS_SRCS}
)