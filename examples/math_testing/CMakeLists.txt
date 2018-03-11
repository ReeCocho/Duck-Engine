# Executable
add_executable (
	MathTesting 
	main.cpp
)

# Libraries
target_link_libraries(
	MathTesting
	Duck-Utilities
	Duck-Math
)