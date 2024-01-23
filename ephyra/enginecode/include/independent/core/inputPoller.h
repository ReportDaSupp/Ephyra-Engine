/**
 * \file inputPoller.h
 * \brief Contains the declaration of the inputPoller class.
 */
#pragma once

#include "globalProperties.h"

namespace Ephyra
{
	/* \class InputPoller
	* \brief API agnostic input poller for getting current keyboard / mouse state
	*/
	class InputPoller
	{
        public:
            // Process methods
            void processKeyPress(int key, int scancode, int actions, int mods);
            void processMouseScroll(float xOffset, float yOffset);
            void processMouseMove(float xPos, float yPos);
            void processMouseClick(int button, int action, int mods);
            void endFrame();

            // Mouse getters   
            inline float& getMousePosX() { return m_lastX; }
            inline float& getMousePosY() { return m_lastY; }
            inline float& getMouseDeltaX() { return m_mouseDeltaX; }
            inline float& getMouseDeltaY() { return m_mouseDeltaY; }
            inline float& getMouseScrollX() { return m_Xscroll; }
            inline float& getMouseScrollY() { return m_Yscroll; }
            inline bool& isRightClicked() { return m_mouseButtonPressed[1]; }
            inline bool& isLeftClicked() { return m_mouseButtonPressed[0]; }
            inline bool  isDragging() { return m_isDragging && m_mouseButtonPressed[1]; }
            inline bool  mouseHasMoved() { return (m_mouseDeltaX != 0.0 && m_mouseDeltaY != 0.0); }

            // Keyboard getters
            bool keyHasBeenPressed();
            inline bool& isKeyPressed(int keyCode) { return m_keysPressed[keyCode]; }
            inline void processedKey(int keyCode) { m_keysPressed[keyCode] = false; }

        private:
            // mouse vars
            float m_Xscroll = 0.0;
            float m_Yscroll = 0.0;
            float m_Xpos = SCR_WIDTH / 2;  //centre of the screen 
            float m_Ypos = SCR_HEIGHT / 2;
            float m_mouseDeltaX = 0.0f;
            float m_mouseDeltaY = 0.0f;
            float m_lastX = 0.0f;
            float m_lastY = 0.0f;
            bool m_mouseButtonPressed[2] = { 0 };  //!< mouse buttons 0 is left, 1 is right
            bool m_isDragging = false;
            bool m_firstPress = true;
            // key vars
            bool m_keysPressed[350] = { 0 };

	};
}
