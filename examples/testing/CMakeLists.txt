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
	Duck-Math
	Duck-Graphics
)