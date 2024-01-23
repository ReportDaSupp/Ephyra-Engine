/** \file keyEvent.h */
#pragma once

#include "event.h"

#include <glm/glm.hpp>

namespace Ephyra
{
	class KeyEvent : public Event
	{
	protected:
		KeyEvent(int32_t keycode) : m_keyCode(keycode) {}
		int32_t m_keyCode;
	public:
		inline int32_t getKeyCode() const { return m_keyCode; }
		virtual inline int32_t getCategoryFlags() const override { return EventCategoryKeyboard | EventCategoryInput; }


	};

	class KeyPressedEvent : public KeyEvent
	{
	private:
		int32_t m_repeatCount;

	public:
		KeyPressedEvent(int32_t keyCode ,int32_t repeatCount) : KeyEvent(keyCode), m_repeatCount(repeatCount) {}
		
		inline int32_t getRepeatCount() const { return m_repeatCount; }
		static EventType getStaticType() { return EventType::KeyPressed; }
		virtual inline EventType getEventType() const override { return getStaticType(); }

	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int32_t keycode) : KeyEvent(keycode) {}

		static EventType getStaticType() { return EventType::KeyReleased; }
		virtual inline EventType getEventType() const override { return getStaticType(); }
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int32_t keycode) : KeyEvent(keycode) {}

		static EventType getStaticType() { return EventType::KeyTyped; }
		virtual inline EventType getEventType() const override { return getStaticType(); }
	};
}