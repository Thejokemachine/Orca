#ifndef __ORCA_APP__
#define __ORCA_APP__

#include <unordered_map>
#include <memory>
#include <type_traits>
#include "IService.h"
#include "Window.h"
#include <glaze/json.hpp>

namespace orca
{
	class CApp
	{
	public:
		static CApp& Instance();
		virtual ~CApp() {};

		template<typename T>
		void RegisterService(std::shared_ptr<T> service)
		{
			static_assert(std::is_base_of_v<IService, T>);
			mServices[typeid(T).hash_code()] = service;
		}

		template<typename T>
		std::weak_ptr<T> GetService()
		{
			static_assert(std::is_base_of_v<IService, T>);
			auto it = mServices.find(typeid(T).hash_code());
			if (it != mServices.end())
			{
				return std::static_pointer_cast<T>(it->second);
			}
			return std::weak_ptr<T>();
		}

		virtual void Run();

	protected:
		virtual void ReadLocalConfig();
		virtual void RegisterServices();

		CApp();

		static std::unique_ptr<CApp> mInstance;

		std::unordered_map<size_t, std::shared_ptr<IService>> mServices;
		std::unique_ptr<CWindow> mMainWindow;
		glz::generic mConfig;
	};
}

#endif