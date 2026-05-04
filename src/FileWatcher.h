#ifndef __ORCA_FILEWATCHER__
#define __ORCA_FILEWATCHER__

#include "IService.h"

#include <thread>
#include <functional>
#include <filesystem>
#include <mutex>

namespace orca
{
	class CFileWatcher : public IService
	{
	public:
		CFileWatcher();
		virtual ~CFileWatcher();

		void Start();
		void Stop();

		void Observe(const std::filesystem::path& path, const std::function<void()>& callback);
		void Unobserve(const std::filesystem::path& path);

	private:

		void ThreadFunc();

		std::thread mThread;

		std::mutex mObserverMutex;
		std::mutex mContainerMutex;

		struct SObserver
		{
			std::filesystem::path path;
			std::filesystem::file_time_type lastWriteTime;
			std::function<void()> callback;
		};
		std::unordered_map<std::filesystem::path, std::unique_ptr<SObserver>> mObservers;

		std::vector<std::unique_ptr<SObserver>> mAddedObservers;
		std::vector<std::filesystem::path> mRemovedObservers;
	};
}

#endif