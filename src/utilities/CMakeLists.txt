# Headers
set(DUCK_UTILS_HDRS
	common.hpp
	debugging.hpp
	macros.hpp
	resource_allocator.hpp
	file_io.hpp
)

# Sources
set(DUCK_UTILS_SRCS
	resource_allocator.cpp
	file_io.cpp
)

# Utilities lib
add_library(Duck-Utilities
	${DUCK_UTILS_HDRS}
	${DUCK_UTILS_SRCS}
)