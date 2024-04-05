/** \file textureUnitManager.h */

#include "Ephyra_pch.h"

#include "Core/Resources/Utility/TextureUnitManager.h"

namespace Engine
{
	void TextureUnitManager::clear()
	{

		m_head = 0;
		m_tail = 0;
		m_full = false;
		std::fill(m_buffer.begin(), m_buffer.end(), 0xFFFFFFFF);

	}
	bool TextureUnitManager::getUnit(uint32_t textureID, uint32_t& textureUnit)
	{
		// Is the texture already bound?
		for (int i = m_tail; i < m_head; i++)
		{
			if (m_buffer.at(i) == textureID)
			{
				textureUnit = i;
				return true;
			}
		}
		//Texture Unit is not bound

		// Is there space in the buffer?
		if (m_full) clear();

		// Put Texture ID in buffer
		m_buffer.at(m_head) = textureID;
		textureUnit = m_head;

		// Check if over capacity
		m_head = (++m_head) % m_capacity;
		if (m_head == m_tail) m_full = true;

		return false;
	
	}
}