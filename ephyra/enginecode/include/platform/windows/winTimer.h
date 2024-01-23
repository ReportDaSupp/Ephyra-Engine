/** \file winTimer */
#pragma once

#include <Windows.h>
#include <core/timer.h>

namespace Ephyra
{
	/** \class WinTimer
	*	Windows Specific Timer for Performace Count
	*/
	
	class WinTimer : public Timer
	{
	public:
		virtual void start() override 
		{
			QueryPerformanceFrequency(&m_frequency);
			QueryPerformanceCounter(&m_startTime);
		};

		virtual inline void reset() override { QueryPerformanceCounter(&m_startTime); };

		virtual float getElapsedTime() override 
		{
			QueryPerformanceCounter(&m_endTime);
			float result = ((m_endTime.QuadPart - m_startTime.QuadPart) * 1000.0f) / m_frequency.QuadPart;
			result /= 1000.f;
			return result;
		}
	private:
		LARGE_INTEGER m_startTime; //!< Start time for the timer
		LARGE_INTEGER m_endTime; //!< End time for the timer
		LARGE_INTEGER m_frequency; //!< CPU ticks per second
	};
}
