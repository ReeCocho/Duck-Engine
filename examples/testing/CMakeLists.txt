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
	${Bullet_LIBRARIES}
	Duck-Utilities
	Duck-Graphics
	Duck-ECS
	Duck-Physics
	Duck-Engine
	Duck-Editor
	Duck-Standard-Components
)

set_target_properties(Testing PROPERTIES 
	VS_DEBUGGER_WORKING_DIRECTORY 
	"${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_command(TARGET Testing POST_BUILD 
	COMMAND "${CMAKE_COMMAND}" -E copy_directory 
	"${PROJECT_BINARY_DIR}/shaders"
    "${CMAKE_CURRENT_BINARY_DIR}/shaders" 
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_SOURCE_DIR}/src/shaders/resources.json" "${CMAKE_CURRENT_BINARY_DIR}/shaders/resources.json"
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_SOURCE_DIR}/src/shaders/standard.shd" "${CMAKE_CURRENT_BINARY_DIR}/shaders/standard.shd"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/textures" "${CMAKE_CURRENT_BINARY_DIR}/textures"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/meshes" "${CMAKE_CURRENT_BINARY_DIR}/meshes"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/materials" "${CMAKE_CURRENT_BINARY_DIR}/materials"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/cubemaps" "${CMAKE_CURRENT_BINARY_DIR}/cubemaps"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_SOURCE_DIR}/examples/testing/skys" "${CMAKE_CURRENT_BINARY_DIR}/skys"
)

add_custom_command(TARGET Testing POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	"${CMAKE_SOURCE_DIR}/examples/testing/config.json" "${CMAKE_CURRENT_BINARY_DIR}/config.json"
)