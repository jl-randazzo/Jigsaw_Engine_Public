#ifndef _J_LOG_H_
#define _J_LOG_H_

#ifndef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif

/// Sets the log level if it was not specified in the preprocessor
#ifndef _LOG_LEVEL_
#define _LOG_LEVEL_ spdlog::level::trace
#endif

#include <stdlib.h>
#include "Ref.h"
#include "Util/etype_info.h"

#include <spdlog/async_logger.h>
#include <spdlog/logger.h>

namespace Jigsaw {
	namespace Debug {
		class JLogger {
		public:
			/// <summary>
			/// Primary logger get call
			/// </summary>
			/// <returns></returns>
			static spdlog::logger& GetLogger();

		private:
			/// <summary>
			/// JLogger private constructor used for the logging implementation
			/// </summary>
			JLogger();

			Jigsaw::Ref<spdlog::async_logger> logger; 

		};
	}
}

#ifdef __JIGSAW_LOG_ENABLED
#define LOG_INFO(info) std::cout << info << std::endl;
#define LOG_ERR(err) std::cout << info << std::endl;
#else
#define LOG_INFO(info) 
#define LOG_ERR(err) 
#endif
#endif

#define _EXPAND(...) __VA_ARGS__
#define _VA_EXP(...) __VA_ARGS__
#define _VA_REM(...)
#define __PRE_STR_GET(format_string, ...) format_string 
#define __PST_STR_GET(message, format_string, ...) message, __VA_ARGS__
#define __LOG_EXPANDER(classname, ...) { \
std::string __log_msg = Jigsaw::Util::etype_info::Id<classname>().GetQualifiedName() + ": " + _EXPAND(__PRE_STR_GET(__VA_ARGS__))

#define J_LOG_DEBUG(classname, ...) \
_EXPAND(__LOG_EXPANDER(classname, __VA_ARGS__)); \
Jigsaw::Debug::JLogger::GetLogger().debug(_VA_EXP(_EXPAND(__PST_STR_GET(__log_msg.c_str(), __VA_ARGS__))));\
}

#define J_LOG_ERROR(classname, ...) \
_EXPAND(__LOG_EXPANDER(classname, __VA_ARGS__)); \
Jigsaw::Debug::JLogger::GetLogger().error(_VA_EXP(_EXPAND(__PST_STR_GET(__log_msg.c_str(), __VA_ARGS__))));\
}

#define J_LOG_TRACE(classname, ...) \
_EXPAND(__LOG_EXPANDER(classname, __VA_ARGS__)); \
Jigsaw::Debug::JLogger::GetLogger().trace(_VA_EXP(_EXPAND(__PST_STR_GET(__log_msg.c_str(), __VA_ARGS__))));\
}

#define J_LOG_WARN(classname, ...) \
_EXPAND(__LOG_EXPANDER(classname, __VA_ARGS__)); \
Jigsaw::Debug::JLogger::GetLogger().warn(_VA_EXP(_EXPAND(__PST_STR_GET(__log_msg.c_str(), __VA_ARGS__))));\
}

#define J_LOG_INFO(classname, ...) \
_EXPAND(__LOG_EXPANDER(classname, __VA_ARGS__)); \
Jigsaw::Debug::JLogger::GetLogger().info(_VA_EXP(_EXPAND(__PST_STR_GET(__log_msg.c_str(), __VA_ARGS__))));\
}

