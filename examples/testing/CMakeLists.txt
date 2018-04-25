# Executable
add_executable (
	Testing 
	main.cpp
)

# Libraries
target_link_libraries(
	Testing
	${SDL2_LIBRARY} 
	${VULKAN_LIBRARY}
	Duck-Utilities
	Duck-Graphics
	Duck-ECS
	Duck-Engine
)

add_custom_command(TARGET Testing POST_BUILD 
	COMMAND "${CMAKE_COMMAND}" -E copy_directory 
	"${PROJECT_BINARY_DIR}/shaders"
    "$<TARGET_FILE_DIR:Testing>/../shaders" 
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/textures" "$<TARGET_FILE_DIR:Testing>/../textures"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/meshes" "$<TARGET_FILE_DIR:Testing>/../meshes"
)