#pragma once

#include "Core/Systems/Events/EventHandler.h"
#include "Core/Initialization/GraphicsContext.h"

namespace Engine 
{

	struct WindowProperties
	{
		char* m_title;
		uint32_t m_width, m_height;
		bool m_isFullscreen;
		bool m_isVSync;
		glm::vec2 m_position;
		WindowProperties(char * title = "Ephyra Animation Engine", uint32_t width = 1024, uint32_t height = 800, bool fullscreen = false) : m_title(title), m_width(width), m_height(height), m_isFullscreen(fullscreen) {}
	};

	class Window
	{
	public:

		virtual void init(const WindowProperties& properties) = 0; //!< initialise the window
		virtual void close() = 0; //!< Close the window

		virtual ~Window() {}; //!< Destructor

		virtual void onUpdate(float timestep) = 0; //!< Call on each frame to update sim
		virtual void setVSync(bool VSync) = 0; //!< Enable Vsync or not

		virtual unsigned int getWidth() const = 0; //!< Return width
		virtual unsigned int getHeight() const = 0; //!< Return height

		virtual unsigned int getWidthf() const = 0; //!< Return width as float
		virtual unsigned int getHeightf() const = 0; //!< Return height as float

		virtual void* getNativeWindow() const = 0; //!< Return Native Resolution / Window Size

		virtual bool isFullScreenMode() const = 0; //!< Return if Fullscreen Mode Enabled
		virtual bool isVSync() const = 0; //!< Return if VSync enabled

		virtual EventHandler& getEventHandler() = 0; //!< Interpolate Inputs

		static Window* create(const WindowProperties& properties = WindowProperties());

	protected:

		EventHandler m_eventHandler; //!< Event handler init
		std::shared_ptr<GraphicsContext> m_graphicsContext;

	};
}
