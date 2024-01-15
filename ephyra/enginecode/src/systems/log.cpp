/** \file log.cpp */
#include "engine_pch.h"
#include "systems/log.h"

namespace Engine {

	std::shared_ptr<spdlog::logger> Log::s_consoleLogger = nullptr;
	std::shared_ptr<spdlog::logger> Log::s_fileLogger = nullptr;

	void Log::start(SystemSignal init, ...)
	{
		spdlog::set_pattern("%^[%X]: %v%$");
		spdlog::set_level(spdlog::level::trace);

		s_consoleLogger = spdlog::stdout_color_mt("Console");

		char filePath[256] = "logs/";
		char time[128];

		/*try
		{
			std::time_t t = std::time(nullptr);
			std::strftime(time, sizeof(time), "%d_%m_%y %I_%M_%S", std::localtime(&t));
			strcat_s(filePath, time);
			strcat_s(filePath, ".txt");
			s_fileLogger = spdlog::basic_logger_mt("File", filePath);
		}
		catch (const spdlog::spdlog_ex& e)
		{
			s_consoleLogger->error("Could not start file logger: {0}", e.what());
			s_fileLogger.reset();
		}*/
	}

	void Log::stop(SystemSignal close, ...)
	{
		s_consoleLogger->info("Stopping console logger");
		s_consoleLogger.reset();
	}

}