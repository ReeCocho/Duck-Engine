/**
 * @file input.cpp
 * @brief Input source file.
 * @author Connor J. Bramham (ReeCocho)
 */

/** Includes */
#include "config.hpp"
#include "input.hpp"

namespace dk
{
	Input::Input(char dummy)
	{
		// Initialize SDL events
		SDL_Init(SDL_INIT_EVENTS);
	}

	void Input::poll_events()
	{
		// Reset text input
		m_text_input = "";

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

			// Text input
			else if(evt.type == SDL_TEXTINPUT)
			{
				m_text_input = evt.text.text;
			}

			// Mouse wheel
			else if (evt.type == SDL_MOUSEWHEEL)
			{
				m_mouse_wheel = { static_cast<float>(evt.wheel.x), static_cast<float>(evt.wheel.y) };
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