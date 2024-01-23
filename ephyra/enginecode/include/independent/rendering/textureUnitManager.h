/** \file textureUnitManager.h */
#pragma once

#include <vector>

namespace Ephyra
{
	class TextureUnitManager
	{
	public:
		TextureUnitManager(uint32_t capacity) : m_capacity(capacity), m_buffer(capacity, 0xFFFFFFFF) {} //!< Constructor that takes a capacity
		inline bool isFull() { return m_full; } //!< Is the buffer full
		void clear(); //!< Clear and reset the buffer
		bool getUnit(uint32_t textureID, uint32_t& textureUnit); //!< Returns whether the texture needs binding to the unit

	private:
		uint32_t m_capacity; //!< Capacity Of Buffer
		std::vector<uint32_t> m_buffer; //!< Internal Buffer
		bool m_full = false; //!< Is the buffer full
		uint32_t m_head = 0; //!< Head of the Buffer
		uint32_t m_tail = 0; //!< Tail of the Buffer
	};
}
