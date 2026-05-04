#include "App.h"

#include "Logger.h"
#include "FileWatcher.h"

using namespace orca;

std::unique_ptr<CApp> CApp::mInstance;

CApp& CApp::Instance()
{
	if (!mInstance)
	{
		mInstance = std::unique_ptr<CApp>(new CApp());
	}
	return *mInstance;
}

CApp::CApp()
{
}

void orca::CApp::Run()
{
	ReadLocalConfig();
	RegisterServices();

	mMainWindow = std::make_unique<CWindow>(sf::VideoMode({1600, 900}, 32U), "Orca Engine");

	bool running = true;
	while (running)
	{
		mMainWindow->EventLoop();
		running = mMainWindow->GetRenderWindow().has_value();
	}
}

void CApp::ReadLocalConfig()
{
	glz::generic config;
	if (glz::read_file_json(config, "local_config.json", std::string{}) == glz::error_code::none)
	{
		
	}
}

void CApp::RegisterServices()
{
	RegisterService(std::make_shared<CLogger>());
	RegisterService(std::make_shared<CFileWatcher>());
}