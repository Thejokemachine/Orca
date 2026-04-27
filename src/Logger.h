#ifndef __ORCA_LOGGER__
#define __ORCA_LOGGER__

#include "App.h"
#include "IService.h"

#include <memory>
#include <string>
#include <unordered_map>

#ifdef _DEBUG
#define DebugLog(l) { static std::weak_ptr<orca::CLogger> _logger; if (_logger.expired()) _logger = orca::CApp::Instance().GetService<orca::CLogger>(); _logger.lock()->Log(l); }
#else
#define DebugLog(l) (l)
#endif // _DEBUG


namespace orca
{
	class CLogger : public IService
	{
	public:

		virtual ~CLogger() {};

		void Log(const std::string& output);
	};
}

#endif