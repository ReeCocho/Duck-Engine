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
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_SOURCE_DIR}/src/shaders/resources.json" "$<TARGET_FILE_DIR:Testing>/../shaders/resources.json"
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_SOURCE_DIR}/src/shaders/standard.shd" "$<TARGET_FILE_DIR:Testing>/../shaders/standard.shd"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/textures" "$<TARGET_FILE_DIR:Testing>/../textures"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/meshes" "$<TARGET_FILE_DIR:Testing>/../meshes"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/materials" "$<TARGET_FILE_DIR:Testing>/../materials"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_SOURCE_DIR}/examples/testing/config.json" "$<TARGET_FILE_DIR:Testing>/../config.json"
)