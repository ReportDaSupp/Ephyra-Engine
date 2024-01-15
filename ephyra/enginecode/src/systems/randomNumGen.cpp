/** \file randomNumGen.cpp */

#include "engine_pch.h"
#include "systems/randomNumGen.h"
#include <chrono>

namespace Engine
{

	std::shared_ptr<std::mt19937> RandomNumGen::s_generator = nullptr; 
	std::uniform_int_distribution<int32_t> RandomNumGen::s_uniformInt(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());
	std::uniform_real_distribution<float> RandomNumGen::s_uniformFloat(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
	float RandomNumGen::s_intRange = static_cast<float>(std::numeric_limits<int32_t>::max()) - static_cast<float>(std::numeric_limits<int32_t>::min());
	float RandomNumGen::s_floatRange = std::numeric_limits<float>::max() - std::numeric_limits<float>::min();

	void Engine::RandomNumGen::start(SystemSignal init, ...)
	{
		auto now = std::chrono::high_resolution_clock::now();
		s_generator.reset(new std::mt19937(now.time_since_epoch().count()));
	}

	void Engine::RandomNumGen::stop(SystemSignal close, ...)
	{
		s_generator.reset();
	}

	int32_t Engine::RandomNumGen::uniformIntBetween(int32_t lower, int32_t upper)
	{
		return lower + 2*(fabs(static_cast<float>(s_uniformInt(*s_generator))) / (s_intRange))*(upper - lower);
	}

	float Engine::RandomNumGen::uniformFloatBetween(float lower, float upper)
	{
		return lower + (s_uniformFloat(*s_generator) / s_floatRange)*(upper - lower);
	}

	int32_t RandomNumGen::normalInt(float c, float sigma)
	{
		std::normal_distribution<float> distribution(c, sigma);
		return static_cast<int32_t>(distribution(*s_generator));
	}

	float RandomNumGen::normalfloat(float c, float sigma)
	{
		std::normal_distribution<float> distribution(c, sigma);
		return (distribution(*s_generator));
	}

}
