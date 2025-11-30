#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>

class Logger {
private:
	static bool initialized;
	static std::shared_ptr<spdlog::logger> logger;

public:
	static void init();
	static std::shared_ptr<spdlog::logger>& getLogger() { return logger; }

	template <typename... Args>
	static void trace(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		logger->trace(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void debug(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		logger->debug(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		logger->info(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		logger->warn(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		logger->error(fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	[[noreturn]] static void critical(fmt::format_string<Args...> fmt, Args &&...args) {
		logger->critical(fmt, std::forward<Args>(args)...);
		std::exit(0xDEAD);
	}
};

#endif //LOGGER_H
