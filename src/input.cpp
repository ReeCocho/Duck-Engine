/**
 * @file input.cpp
 * @brief Input source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes */
#include "config.hpp"
#include <editor\imgui\imgui.h>
#include "input.hpp"

namespace dk
{
	Input::Input(char dummy)
	{
		// Initialize SDL events
		SDL_Init(SDL_INIT_EVENTS);

#if DK_EDITOR
		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
		io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
		io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
		io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
		io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
		io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
		io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
		io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
		io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
		io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
		io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
		io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
		io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
		io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
		io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
#endif
	}

	void Input::poll_events()
	{
		// Set last key presses
		m_last_keys = m_current_keys;

		// Set last mouse buttons
		m_left_mouse_button = std::make_tuple(std::get<0>(m_left_mouse_button), std::get<0>(m_left_mouse_button));
		m_right_mouse_button = std::make_tuple(std::get<0>(m_right_mouse_button), std::get<0>(m_right_mouse_button));
		m_middle_mouse_button = std::make_tuple(std::get<0>(m_middle_mouse_button), std::get<0>(m_middle_mouse_button));

		// Say we haven't found the last mouse delta
		m_found_mouse_delta = false;

		// Loop through events
		SDL_Event evt;

		while (SDL_PollEvent(&evt))
		{
			// Close window
			if (evt.type == SDL_QUIT)
				m_closing = true;

			// Get key down
			else if (evt.type == SDL_KEYDOWN)
				m_current_keys[static_cast<KeyCode>(evt.key.keysym.sym)] = true;

			// Get key up
			else if (evt.type == SDL_KEYUP)
				m_current_keys[static_cast<KeyCode>(evt.key.keysym.sym)] = false;

			// Get mouse button down
			else if (evt.type == SDL_MOUSEBUTTONDOWN)
			{
				if (evt.button.button == SDL_BUTTON_LEFT)
					m_left_mouse_button = std::make_tuple(true, std::get<1>(m_left_mouse_button));

				else if (evt.button.button == SDL_BUTTON_RIGHT)
					m_right_mouse_button = std::make_tuple(true, std::get<1>(m_right_mouse_button));

				else if (evt.button.button == SDL_BUTTON_MIDDLE)
					m_middle_mouse_button = std::make_tuple(true, std::get<1>(m_middle_mouse_button));
			}

			// Get mouse button up
			else if (evt.type == SDL_MOUSEBUTTONUP)
			{
				if (evt.button.button == SDL_BUTTON_LEFT)
					m_left_mouse_button = std::make_tuple(false, std::get<1>(m_left_mouse_button));

				else if (evt.button.button == SDL_BUTTON_RIGHT)
					m_right_mouse_button = std::make_tuple(false, std::get<1>(m_right_mouse_button));

				else if (evt.button.button == SDL_BUTTON_MIDDLE)
					m_middle_mouse_button = std::make_tuple(false, std::get<1>(m_middle_mouse_button));
			}
		}
	}

	void Input::register_button(const std::string& name, KeyCode key)
	{
		m_buttons.insert(std::pair<std::string, KeyCode>(name, key));
	}

	void Input::register_axis(const std::string& name, const KeyAxis& axis)
	{
		m_axis.insert(std::pair<std::string, KeyAxis>(name, axis));
	}
}