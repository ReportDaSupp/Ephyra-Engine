#pragma once

#include "Core/Systems/Utility/System.h"

#include <memory>
#include <unordered_map>
#include <soloud.h>
#include <soloud_wav.h>

namespace Engine
{
	class audioSystem2D : public System
	{
	public:

		virtual void start(SystemSignal init = SystemSignal::None, ...) override//!< Start the system
		{
			m_audioSystem.init();
		}

		virtual void stop(SystemSignal close = SystemSignal::None, ...) override //!< Stop the system
		{
			m_audioSystem.deinit();
		}

		bool addSound(const std::string& tag, const std::string& filePath); //!< Add a sound file to audio system 
		bool playSound(const std::string& tag, float volume = 0.6f, float pan = 0.0f, float playSpeed = 1.0f); //!< Play a sound already loaded into audio system
		bool setSoundLooping(const std::string& tag, bool looping); //<! Set sound looping properties
		//add a delete sound fuction at some point!!!!!!!!!!!!!!!!!!!!!!!

	private:
		SoLoud::Soloud m_audioSystem;
		std::unordered_map<std::string, SoLoud::Wav> m_sounds;
	};
}