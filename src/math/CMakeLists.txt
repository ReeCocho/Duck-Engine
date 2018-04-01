# Headers
set(DUCK_MATH_HDRS
	common.hpp
	matrices.hpp
	quaternions.hpp
	vectors.hpp
	transformations.hpp
)

# Sources
set(DUCK_MATH_SRCS
	matrices.cpp
	quaternions.cpp
	vectors.cpp
	transformations.cpp
)

# Math lib
add_library(Duck-Math 
	${DUCK_MATH_HDRS}
	${DUCK_MATH_SRCS}
)