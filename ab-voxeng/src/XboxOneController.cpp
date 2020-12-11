#include "XboxOneController.h"

/// <summary>
/// Constructor for the XboxOneController class.
/// </summary>
ab::XboxOneController::XboxOneController(int t_controllerIndex) : m_controllerIndex(t_controllerIndex)
{
	std::cout << "XboxOneController constructor called" << std::endl;

	// Check if a controller is connected
	m_joystick = NULL;
	m_joystick = SDL_GameControllerOpen(m_controllerIndex);

	if (m_joystick == NULL)
	{
		std::cout << "Could not create XboxOneController->m_joystick at index " << m_controllerIndex << std::endl;
	}
	else
	{
		std::cout << "Creating XboxOneController->m_joystick" << std::endl;
	}
}

/// <summary>
/// Destructor for the XboxOneController class.
/// </summary>
ab::XboxOneController::~XboxOneController()
{
	std::cout << "XboxOneController destructor called" << std::endl;

	std::cout << "Deleting XboxOneController->m_joystick" << std::endl;
	SDL_GameControllerClose(m_joystick);
	m_joystick = NULL;
}

/// <summary>
/// Update controller logic.
/// </summary>
/// <param name="t_event">A reference to an SDL_Event.</param>
void ab::XboxOneController::processEvents(SDL_Event &t_event)
{
	if (t_event.type == SDL_CONTROLLERAXISMOTION)
	{
		if (t_event.caxis.which == m_controllerIndex)
		{
			///////////////////////
			// LEFT ANALOG STICK //
			///////////////////////
			if (t_event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
			{
				if (t_event.caxis.value > JOYSTICK_DEAD_ZONE || t_event.caxis.value < -JOYSTICK_DEAD_ZONE)
				{
					m_currentState.LeftThumbStick.x = t_event.caxis.value;
				}
				else
				{
					m_currentState.LeftThumbStick.x = 0;
				}				
			}

			if (t_event.jaxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
			{
				if (t_event.caxis.value > JOYSTICK_DEAD_ZONE || t_event.caxis.value < -JOYSTICK_DEAD_ZONE)
				{
					m_currentState.LeftThumbStick.y = t_event.caxis.value;
				}
				else
				{
					m_currentState.LeftThumbStick.y = 0;
				}
			}

			////////////////////////
			// RIGHT ANALOG STICK //
			////////////////////////
			if (t_event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
			{
				if (t_event.caxis.value > JOYSTICK_DEAD_ZONE || t_event.caxis.value < -JOYSTICK_DEAD_ZONE)
				{
					m_currentState.RightThumbStick.x = t_event.caxis.value;
				}
				else
				{
					m_currentState.RightThumbStick.x = 0;
				}				
			}

			if (t_event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
			{
				if (t_event.caxis.value > JOYSTICK_DEAD_ZONE || t_event.caxis.value < -JOYSTICK_DEAD_ZONE)
				{
					m_currentState.RightThumbStick.y = t_event.caxis.value;
				}
				else
				{
					m_currentState.RightThumbStick.y = 0;
				}
			}

			////////////
			// LT, RT //
			////////////
			if (t_event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
			{
				m_currentState.LTrigger = t_event.caxis.value;
			}

			if (t_event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
			{
				m_currentState.RTrigger = t_event.caxis.value;
			}

			///////////
			// D-PAD //
			///////////
			if (t_event.caxis.axis == SDL_CONTROLLER_BUTTON_DPAD_UP)
			{
				m_currentState.DpadUp = true;
			}
			else
			{
				m_currentState.DpadUp = false;
			}

			if (t_event.caxis.axis == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
			{
				m_currentState.DpadDown = true;
			}
			else
			{
				m_currentState.DpadDown = false;
			}

			if (t_event.caxis.axis == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
			{
				m_currentState.DpadLeft = true;
			}
			else
			{
				m_currentState.DpadLeft = false;
			}

			if (t_event.caxis.axis == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
			{
				m_currentState.DpadRight = true;
			}
			else
			{
				m_currentState.DpadRight = false;
			}
		}
	}

	if (t_event.type == SDL_CONTROLLERBUTTONDOWN)
	{
		if (t_event.cbutton.which == m_controllerIndex)
		{
			////////////////
			// A, B, X, Y //
			////////////////
			if (t_event.cbutton.button == SDL_CONTROLLER_BUTTON_A)
			{
				m_currentState.A = true;
			}
			else
			{
				m_currentState.A = false;
			}

			if (t_event.cbutton.button == SDL_CONTROLLER_BUTTON_B)
			{
				m_currentState.B = true;
			}
			else
			{
				m_currentState.B = false;
			}

			if (t_event.cbutton.button == SDL_CONTROLLER_BUTTON_X)
			{
				m_currentState.X = true;
			}
			else
			{
				m_currentState.X = false;
			}

			if (t_event.cbutton.button == SDL_CONTROLLER_BUTTON_Y)
			{
				m_currentState.Y = true;
			}
			else
			{
				m_currentState.Y = false;
			}

			////////////
			// LB, RB //
			////////////
			if (t_event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
			{
				m_currentState.LB = true;
			}
			else
			{
				m_currentState.LB = false;
			}

			if (t_event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
			{
				m_currentState.RB = true;
			}
			else
			{
				m_currentState.RB = false;
			}
		}
	}
}
