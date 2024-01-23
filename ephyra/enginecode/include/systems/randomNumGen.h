/** \file randomNumGen */
#pragma once

#include <random>
#include "system.h"

namespace Ephyra
{
	class RandomNumGen : public System
	{
	public:
		virtual void start(SystemSignal init = SystemSignal::None, ...) override; //!< Start the RandomNumGen
		virtual void stop(SystemSignal close = SystemSignal::None, ...) override; //!< Stop the RandomNumGen

		static int32_t uniformIntBetween(int32_t lower, int32_t upper); //!< Get an int in given range
		static float uniformFloatBetween(float lower, float upper); //!< Get an float in given range

		static int32_t normalInt(float c, float sigma); //!< Get a int from Gaussian distribution described b c and sigma
		static float normalfloat(float c, float sigma); //!< Get a float from Gaussian distribution described b c and sigma

	private:
		static std::shared_ptr<std::mt19937> s_generator; //!< Engine For Random Numbers
		static std::uniform_int_distribution<int32_t> s_uniformInt; //!< Uniform Integer distribution
		static std::uniform_real_distribution<float> s_uniformFloat; //!< Uniform Float distribution
		static float s_intRange; //!< Range of Int32_t
		static float s_floatRange; //!< Range of Float

	};
}