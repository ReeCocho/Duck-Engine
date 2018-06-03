# Headers
set(DUCK_ENGINE_HDRS
	engine.hpp
	input.hpp
	resource_manager.hpp
	archive.hpp
	common.hpp
	config.hpp
)

# Sources
set(DUCK_ENGINE_SRCS
	engine.cpp
	input.cpp
	resource_manager.cpp
	archive.cpp
)

# Engine lib
add_library(Duck-Engine
	${DUCK_ENGINE_HDRS}
	${DUCK_ENGINE_SRCS}
)