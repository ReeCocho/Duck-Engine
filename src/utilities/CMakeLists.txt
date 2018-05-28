# Headers
set(DUCK_UTILS_HDRS
	common.hpp
	debugging.hpp
	macros.hpp
	resource_allocator.hpp
	file_io.hpp
	threading.hpp
	clock.hpp
	frustum.hpp
	reflection.hpp
	archive.hpp
)

# Sources
set(DUCK_UTILS_SRCS
	resource_allocator.cpp
	file_io.cpp
	threading.cpp
	clock.cpp
	frustum.cpp
	reflection.cpp
	archive.cpp
)

# Utilities lib
add_library(Duck-Utilities
	${DUCK_UTILS_HDRS}
	${DUCK_UTILS_SRCS}
)