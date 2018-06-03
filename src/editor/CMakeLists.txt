# IMGUI files
set(IMGUI_FILES
	imgui/imconfig.h
	imgui/imgui.h
	imgui/imgui_internal.h
	imgui/stb_rect_pack.h
	imgui/stb_textedit.h
	imgui/stb_truetype.h
	imgui/imguidock.h
	
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imguidock.cpp
)

# Headers
set(DUCK_EDITOR_HDRS
	editor.hpp
	editor_renderer.hpp
	hierarchy.hpp
	inspector.hpp
	scene_view.hpp
	file_explorer.hpp
	toolbar.hpp
	editor_window.hpp
	component_inspector.hpp
)

# Sources
set(DUCK_EDITOR_SRCS
	editor.cpp
	editor_renderer.cpp
	hierarchy.cpp
	inspector.cpp
	scene_view.cpp
	file_explorer.cpp
	toolbar.cpp
	editor_window.cpp
	component_inspector.cpp
)

# ECS lib
add_library(Duck-Editor
	${IMGUI_FILES}
	${DUCK_EDITOR_HDRS}
	${DUCK_EDITOR_SRCS}
)

source_group("imgui" FILES ${IMGUI_FILES})