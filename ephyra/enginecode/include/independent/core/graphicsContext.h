/** \file graphicsContext.h */
#pragma once

namespace Engine
{
	class GraphicsContext
	{
	public:
		virtual void init() = 0; //!< Init the graphics context for window api
		virtual void swapBuffers() = 0; //!< Swap front and back buffers (Double Buffering)
	};
}
