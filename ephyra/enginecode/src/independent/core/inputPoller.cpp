/** \file inputPoller.cpp */
#include "ephyra_pch.h"
#include "core/inputPoller.h"
#include <numeric>

namespace Ephyra
{

#ifdef NG_PLATFORM_WINDOWS

	void InputPoller::processMouseScroll(float xOffset, float yOffset)
	{
		m_Xscroll = xOffset;
		m_Yscroll = yOffset;

	}

	void InputPoller::processMouseMove(float xPos, float yPos)
	{

		if (m_firstPress) {  // initialise lastPos to currentPos on first frame

			m_lastX = m_Xpos;
			m_lastY = m_Ypos;
			m_firstPress = false;
		}
		// difference between current position and pre position
		m_mouseDeltaX = xPos - m_lastX;
		m_mouseDeltaY = m_lastY - yPos;

		m_lastX = xPos;
		m_lastY = yPos;

	}

	void InputPoller::processMouseClick(int button, int action, int mods)
	{
		if (button < 350) {  // left or right mouse, update if need additional mouse buttons
			if (action == 1) {
				m_mouseButtonPressed[button] = true;  // button is pressed
				m_isDragging = false;                 // mouse is dragging
			}
			else if ((action == 0)) {
				m_mouseButtonPressed[button] = false;  // button release
				m_isDragging = false;                  // no longer dragging
			}
		}

	}

	//reset
	void InputPoller::endFrame()
	{
		m_mouseDeltaX = 0.0f;
		m_mouseDeltaY = 0.0f;
	}

	// has ANY key been pressed?
	bool InputPoller::keyHasBeenPressed()
	{
		int sum = 0;
		for (auto a : m_keysPressed)
		{
			if (a = true) sum++;
		}
		return (sum > 0);  // if at least one is pressed then this value will be > 0
	}

	void InputPoller::processKeyPress(int key, int scancode, int action, int mods)
	{

		if (key < 350) {  // only supporting standard keyboards keys
			if (action == 1)  // if a key has been pressed
			{
				m_keysPressed[key] = true;  // this key is pressed
				auto test = 0;
			}
			else if ((action == 0))
				m_keysPressed[key] = false;
		}
	}

#endif
}