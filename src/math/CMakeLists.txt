# Headers
set(DUCK_MATH_HDRS
	${CMAKE_SOURCE_DIR}/src/math/common.hpp
	${CMAKE_SOURCE_DIR}/src/math/config.hpp
	${CMAKE_SOURCE_DIR}/src/math/matrices.hpp
	${CMAKE_SOURCE_DIR}/src/math/quaternions.hpp
	${CMAKE_SOURCE_DIR}/src/math/vectors.hpp
)

# Math lib
add_library(Duck-Math INTERFACE)

if(MSVC)
    add_custom_target(Duck-Math.headers SOURCES ${DUCK_MATH_HDRS})
endif()

target_sources(Duck-Math INTERFACE ${DUCK_MATH_HDRS})