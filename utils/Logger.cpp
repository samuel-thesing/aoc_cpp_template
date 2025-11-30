#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <memory>
#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;
bool Logger::initialized = false;

void Logger::init() {
	if (initialized) return;
	initialized = true;

#ifdef WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif

	std::vector<spdlog::sink_ptr> sinks;
	sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Log.log", true));

	sinks[0]->set_pattern("%^%v%$");
	sinks[1]->set_pattern("[%T] [%l]: %v");

	logger = std::make_shared<spdlog::logger>("CORE", sinks.begin(), sinks.end());
	spdlog::register_logger(logger);
#ifdef DEBUG
	logger->set_level(spdlog::level::trace);
	logger->flush_on(spdlog::level::trace);
#else
	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);
#endif
}