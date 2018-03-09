# Sources
set(DUCK_MATH_SRCS
	matrices.cpp
	quaternions.cpp
	vectors.cpp
)

# Headers
set(DUCK_MATH_HDRS
	common.hpp
	config.hpp
	matrices.hpp
	quaternions.hpp
	vectors.hpp
)

# Math lib
add_library(
	Duck-Math
	${DUCK_MATH_SRCS}
	${DUCK_MATH_HDRS}
)