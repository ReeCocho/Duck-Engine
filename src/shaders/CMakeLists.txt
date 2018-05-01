if (${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "AMD64")
  set(GLSL_LC "$ENV{VULKAN_SDK}/Bin/glslc.exe")
else()
  set(GLSL_LC "$ENV{VULKAN_SDK}/Bin32/glslc.exe")
endif()

set(GLSL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/shaders/standard.frag
    ${CMAKE_SOURCE_DIR}/src/shaders/standard.vert
)

set(GLSL_INCLUDE_FILES
	"${CMAKE_SOURCE_DIR}/src/shaders/Duck-Standard.frag"
	"${CMAKE_SOURCE_DIR}/src/shaders/Duck-Standard.vert"
)

set(GLSL_INCLUDE_FILES_FIXED)
foreach(INCLUDE_FILE ${GLSL_INCLUDE_FILES})
	set(GLSL_INCLUDE_FILES_FIXED ${GLSL_INCLUDE_FILES_FIXED} -I INCLUDE_FILE)
endforeach(INCLUDE_FILE)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_LC} ${GLSL} -o ${SPIRV} ${GLSL_INCLUDE_FILES_FIXED}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS 
	${SPIRV_BINARY_FILES}
)

add_dependencies(Testing Shaders)

add_custom_command(OUTPUT Testing
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Testing>/../shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${PROJECT_BINARY_DIR}/shaders"
	"$<TARGET_FILE_DIR:Testing>/../shaders"
)