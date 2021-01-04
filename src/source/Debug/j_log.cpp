#include "j_log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/async.h"
#include <vector>
#include "spdlog/spdlog.h"

namespace Jigsaw {
	namespace Debug {
		spdlog::logger& JLogger::GetLogger() {
			static JLogger j_logger;
			return *j_logger.logger.get();
		}

		JLogger::JLogger() {
			spdlog::init_thread_pool(8192, 1);
			auto c_sink = Jigsaw::MakeRef<spdlog::sinks::stdout_color_sink_mt>();
			c_sink->set_level(_LOG_LEVEL_);

			auto f_sink = Jigsaw::MakeRef<spdlog::sinks::rotating_file_sink_mt>("mylog.txt", 1024 * 1024 * 10, 3);
			f_sink->set_level(_LOG_LEVEL_);

			std::vector<spdlog::sink_ptr> sinks{ c_sink, f_sink };

			logger = Jigsaw::MakeRef<spdlog::async_logger>("JIGSAW_LOGGER", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

			spdlog::register_logger(logger);

			spdlog::set_pattern("*** [%H:%M:%S %z] [thread %t] [LEVEL: %l] %v ***");
		}
	}
}
