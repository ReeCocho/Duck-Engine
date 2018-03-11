# Headers
set(DUCK_UTILS_HDRS
	${CMAKE_SOURCE_DIR}/src/utilities/common.hpp
	${CMAKE_SOURCE_DIR}/src/utilities/debugging.hpp
	${CMAKE_SOURCE_DIR}/src/utilities/macros.hpp
)

# Utilities lib
add_library(Duck-Utilities INTERFACE)

if(MSVC)
    add_custom_target(Duck-Utilities.headers SOURCES ${DUCK_UTILS_HDRS})
endif()

target_sources(Duck-Utilities INTERFACE ${DUCK_UTILS_HDRS})