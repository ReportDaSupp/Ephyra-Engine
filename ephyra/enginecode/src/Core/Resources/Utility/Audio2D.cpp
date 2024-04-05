#include "Ephyra_pch.h"

#include "Core/Resources/Utility/Audio2D.h"

namespace Engine {
	bool audioSystem2D::addSound(const std::string& tag, const std::string& filePath)
	{
		m_sounds[tag]= SoLoud::Wav();
		auto result = m_sounds[tag].load(filePath.c_str());
		m_sounds[tag].setVolume(0.6f);
		return result == 0;
	}

	bool audioSystem2D::playSound(const std::string& tag, float volume, float pan, float playSpeed)
	{
		auto sound = m_sounds.find(tag);
		if (sound != m_sounds.end())
		{
			int handle = m_audioSystem.play(sound->second);
			m_audioSystem.setVolume(handle, volume);
			m_audioSystem.setPan(handle, pan);
			m_audioSystem.setRelativePlaySpeed(handle, playSpeed);
			return true;
		}
		return false;
	}

	bool audioSystem2D::setSoundLooping(const std::string& tag, bool looping)
	{
		auto sound = m_sounds.find(tag);
		if (sound != m_sounds.end())
		{
			sound->second.setLooping(looping);
			return true;
		}
		return false;
	}
}
