/** \file GLFWWindowImp.h */
#pragma once

#include "core/window.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	/** \class GlfWWindowImp 
	*	Implementation of GLFW Window
	*/
	class GLFWWindowImp : public Window
	{
	public:

		GLFWWindowImp(const WindowProperties& properties); //!< Constructor
		virtual void init(const WindowProperties& properties) override; //!< initialise the window
		virtual void close() override; //!< Close the window

		//virtual ~Window() {}; //!< Destructor

		virtual void onUpdate(float timestep) override; //!< Call on each frame to update sim
		virtual void setVSync(bool VSync) override; //!< Enable Vsync or not

		virtual inline unsigned int getWidth() const override { return m_props.m_width; }; //!< Return width
		virtual inline unsigned int getHeight() const override { return m_props.m_height; }; //!< Return height

		virtual inline unsigned int getWidthf() const override { return (float)m_props.m_width; }; //!< Return width
		virtual inline unsigned int getHeightf() const override { return (float)m_props.m_height; }; //!< Return height

		virtual inline void* getNativeWindow() const override { return m_native; } //!< Return Native Resolution / Window Size

		virtual inline bool isFullScreenMode() const override { return m_props.m_isFullscreen; }; //!< Return if Fullscreen Mode Enabled
		virtual inline bool isVSync() const override { return m_props.m_isVSync; }; //!< Return if VSync enabled

		inline EventHandler& getEventHandler() override { return m_eventHandler; }; //!< Interpolate Inputs

	private:
		WindowProperties m_props; //!< Properties
		GLFWwindow* m_native; //!< Native GLFW window
		float m_aspectRatio; //!< Store aspect ratio


	};

}