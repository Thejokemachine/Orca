#ifndef __ORCA_LOGGER__
#define __ORCA_LOGGER__

#include "App.h"
#include "IService.h"

#include <memory>
#include <string>
#include <iostream>

#ifdef _DEBUG
#define DebugLog(l) { static std::weak_ptr<orca::CLogger> _logger; if (_logger.expired()) _logger = orca::CApp::Instance().GetService<orca::CLogger>(); _logger.lock()->Log(l); }
#define DebugLogF(l, ...) { static std::weak_ptr<orca::CLogger> _logger; if (_logger.expired()) _logger = orca::CApp::Instance().GetService<orca::CLogger>(); _logger.lock()->Log(l, __VA_ARGS__); }
#else
#define DebugLog(l) (l)
#define DebugLogF(l, ...) (l)
#endif // _DEBUG


namespace orca
{
	class CLogger : public IService
	{
	public:

		virtual ~CLogger() {};

		template <typename... Args>
		void Log(std::string_view fmt, Args&&... args) {
			std::cout << std::vformat(fmt, std::make_format_args(args...)) << '\n';
		}
	};
}

#endif