#include "FileWatcher.h"

#include "Logger.h"

#include <functional>

using namespace orca;



CFileWatcher::CFileWatcher()
{
	Start();
}

orca::CFileWatcher::~CFileWatcher()
{
	Stop();
}

void CFileWatcher::Start()
{
	mThread = std::thread([this]() { ThreadFunc(); });
}

void CFileWatcher::Stop()
{
	mThread.join();
}

void CFileWatcher::Observe(const std::filesystem::path& path, const std::function<void()>& callback)
{
	std::lock_guard lock(mContainerMutex);
	auto observer = new SObserver;
	observer->path = path;
	observer->lastWriteTime = std::filesystem::last_write_time(path);
	observer->callback = callback;
	mAddedObservers.emplace_back().reset(observer);
}

void orca::CFileWatcher::Unobserve(const std::filesystem::path& path)
{
	std::lock_guard lock(mContainerMutex);
	mRemovedObservers.emplace_back(path);
}

void CFileWatcher::ThreadFunc()
{
	while (true)
	{
		mContainerMutex.lock();
		for (auto& removed : mRemovedObservers)
		{
			mObservers.erase(removed);
		}
		mRemovedObservers.clear();

		for (auto& added : mAddedObservers)
		{
			mObservers[added->path] = std::move(added);
		}
		mAddedObservers.clear();
		mContainerMutex.unlock();

		mObserverMutex.lock();
		for (auto& [path, observer] : mObservers)
		{
			auto lastWriteTime = std::filesystem::last_write_time(path);
			if (lastWriteTime > observer->lastWriteTime)
			{
				DebugLogF("{} updated!", path.string());
				observer->lastWriteTime = lastWriteTime;
				observer->callback();
			}
		}
		mObserverMutex.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}