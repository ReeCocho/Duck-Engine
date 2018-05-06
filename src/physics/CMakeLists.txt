# Headers
set(DUCK_PHYSICS_HDRS
	physics.hpp
)

# Sources
set(DUCK_PHYSICS_SRCS
	physics.cpp
)

# Physics lib
add_library(Duck-Physics
	${DUCK_PHYSICS_HDRS}
	${DUCK_PHYSICS_SRCS}
)