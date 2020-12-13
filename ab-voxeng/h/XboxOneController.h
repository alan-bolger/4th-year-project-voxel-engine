#ifndef XBOXONECONTROLLER_H
#define XBOXONECONTROLLER_H

#include <iostream>
#include <SDL.h>

struct ControllerState
{
	bool X;
	bool A;
	bool Y;
	bool B;
	bool LB;
	bool RB;
	bool LeftThumbStickClick;
	bool RightThumbStickClick;
	bool DpadUp;
	bool DpadDown;
	bool DpadLeft;
	bool DpadRight;
	bool Menu;
	bool View;
	bool Xbox;
	int RTrigger;
	int LTrigger;
	SDL_Point RightThumbStick;
	SDL_Point LeftThumbStick;
};

namespace ab
{
	class XboxOneController
	{
	public:
		ControllerState m_currentState;

		XboxOneController(int t_controllerIndex);
		~XboxOneController();
		void processEvents(SDL_Event& t_event);

	private:
		SDL_GameController *m_joystick;
		const int JOYSTICK_DEAD_ZONE = 8000;
		int m_controllerIndex;
	};
}

#endif // !XBOXONECONTROLLER_H

