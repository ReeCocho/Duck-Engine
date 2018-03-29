# Executable
add_executable (
	Testing 
	main.cpp
)

# Libraries
target_link_libraries(
	Testing
	Duck-Utilities
	Duck-Math
	Duck-Graphics
)