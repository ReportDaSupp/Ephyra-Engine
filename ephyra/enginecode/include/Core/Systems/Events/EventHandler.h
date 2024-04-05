/** \file eventHandler.h */
#pragma once

#include "events.h"
#include <functional>

namespace Engine
{
	class EventHandler
	{
	public:

		//Window Events

		void setOnCloseCallback(const std::function<bool(WindowCloseEvent&)>& fn) { m_onCloseCallback = fn; }
		void setOnResizeCallback(const std::function<bool(WindowResizeEvent&)>& fn) { m_onResizeCallback = fn; }
		void setOnFocusCallback(const std::function<bool(WindowFocusEvent&)>& fn) { m_onFocusCallback = fn; }
		void setOnLostFocusCallback(const std::function<bool(WindowLostFocusEvent&)>& fn) { m_onLostFocusCallback = fn; }
		void setOnMovedCallback(const std::function<bool(WindowMovedEvent&)>& fn) { m_onMovedCallback = fn; }

		std::function<bool(WindowCloseEvent&)>& getOnCloseCallback() { return m_onCloseCallback; }
		std::function<bool(WindowResizeEvent&)>& getOnResizeCallback() { return m_onResizeCallback; }
		std::function<bool(WindowFocusEvent&)>& getOnFocusCallback() { return m_onFocusCallback; }
		std::function<bool(WindowLostFocusEvent&)>& getOnLostFocusCallback() { return m_onLostFocusCallback; }
		std::function<bool(WindowMovedEvent&)>& getOnMovedCallback() { return m_onMovedCallback; }

		//Key Events

		void setOnKeyPressedCallback(const std::function<bool(KeyPressedEvent&)>& fn) { m_onKeyPressedCallback = fn; }
		void setOnKeyReleasedCallback(const std::function<bool(KeyReleasedEvent&)>& fn) { m_onKeyReleasedCallback = fn; }
		void setOnKeyTypedCallback(const std::function<bool(KeyTypedEvent&)>& fn) { m_onKeyTypedCallback = fn; }

		std::function<bool(KeyPressedEvent&)>& getOnKeyPressedCallback() { return m_onKeyPressedCallback; }
		std::function<bool(KeyReleasedEvent&)>& getOnKeyReleasedCallback() { return m_onKeyReleasedCallback; }
		std::function<bool(KeyTypedEvent&)>& getOnKeyTypedCallback() { return m_onKeyTypedCallback; }

		// Mouse Events

		void setOnMouseDownCallback(const std::function<bool(MouseButtonPressedEvent&)>& fn) { m_onMouseButtonDownCallback = fn; }
		void setOnMouseUpCallback(const std::function<bool(MouseButtonReleasedEvent&)>& fn) { m_onMouseButtonUpCallback = fn; }
		void setOnMouseMovedCallback(const std::function<bool(MouseMovedEvent&)>& fn) { m_onMouseMovedCallback = fn; }
		void setOnMouseWheelCallback(const std::function<bool(MouseScrolledEvent&)>& fn) { m_onMouseWheelCallback = fn; }

		std::function<bool(MouseButtonPressedEvent&)>& getOnMouseDownCallback() { return m_onMouseButtonDownCallback; }
		std::function<bool(MouseButtonReleasedEvent&)>& getOnMouseUpCallback() { return m_onMouseButtonUpCallback; }
		std::function<bool(MouseMovedEvent&)>& getOnMouseMovedCallback() { return m_onMouseMovedCallback; }
		std::function<bool(MouseScrolledEvent&)>& getOnMouseWheelCallback() { return m_onMouseWheelCallback; }

	private:

		//Window Events

		std::function<bool(WindowCloseEvent&)> m_onCloseCallback = std::bind(&EventHandler::defaultOnClose, this, std::placeholders::_1);
		std::function<bool(WindowResizeEvent&)> m_onResizeCallback = std::bind(&EventHandler::defaultOnResize, this, std::placeholders::_1);
		std::function<bool(WindowFocusEvent&)> m_onFocusCallback = std::bind(&EventHandler::defaultOnFocus, this, std::placeholders::_1);
		std::function<bool(WindowLostFocusEvent&)> m_onLostFocusCallback = std::bind(&EventHandler::defaultOnLostFocus, this, std::placeholders::_1);
		std::function<bool(WindowMovedEvent&)> m_onMovedCallback = std::bind(&EventHandler::defaultOnWindowMoved, this, std::placeholders::_1);

		bool defaultOnClose(WindowCloseEvent& e) { return false; }
		bool defaultOnResize(WindowResizeEvent& e) { return false; }
		bool defaultOnFocus(WindowFocusEvent& e) { return false; }
		bool defaultOnLostFocus(WindowLostFocusEvent& e) { return false; }
		bool defaultOnWindowMoved(WindowMovedEvent& e) { return false; }

		//Key Events

		std::function<bool(KeyPressedEvent&)> m_onKeyPressedCallback = std::bind(&EventHandler::defaultOnKeyPressed, this, std::placeholders::_1);
		std::function<bool(KeyReleasedEvent&)> m_onKeyReleasedCallback = std::bind(&EventHandler::defaultOnKeyReleased, this, std::placeholders::_1);
		std::function<bool(KeyTypedEvent&)> m_onKeyTypedCallback = std::bind(&EventHandler::defaultOnKeyTyped, this, std::placeholders::_1);

		bool defaultOnKeyPressed(KeyPressedEvent& e) { return false; }
		bool defaultOnKeyReleased(KeyReleasedEvent& e) { return false; }
		bool defaultOnKeyTyped(KeyTypedEvent& e) { return false; }

		// Mouse Events

		std::function<bool(MouseButtonPressedEvent&)> m_onMouseButtonDownCallback = std::bind(&EventHandler::defaultOnMouseDown, this, std::placeholders::_1);
		std::function<bool(MouseButtonReleasedEvent&)> m_onMouseButtonUpCallback = std::bind(&EventHandler::defaultOnMouseUp, this, std::placeholders::_1);
		std::function<bool(MouseMovedEvent&)> m_onMouseMovedCallback = std::bind(&EventHandler::defaultOnMouseMoved, this, std::placeholders::_1);
		std::function<bool(MouseScrolledEvent&)> m_onMouseWheelCallback = std::bind(&EventHandler::defaultOnMouseWheel, this, std::placeholders::_1);

		bool defaultOnMouseDown(MouseButtonPressedEvent& e) { return false; }
		bool defaultOnMouseUp(MouseButtonReleasedEvent& e) { return false; }
		bool defaultOnMouseMoved(MouseMovedEvent& e) { return false; }
		bool defaultOnMouseWheel(MouseScrolledEvent& e) { return false; }
	};
}
