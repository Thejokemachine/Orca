#include "Window.h"
#include <iostream>
#include <format>
#include <string>
#include "StringUtilities.h"

#include "Logger.h"
#include "FileWatcher.h"

#include "Layout.h"
#include "LayoutParser.h"

#define DEFINE_EVENT_HANDLER(e) void CWindow::operator()(e)

using namespace orca;

CWindow::CWindow(const sf::VideoMode& videoMode, std::string_view title, uint32_t styleFlags, sf::State state, const std::optional<sf::ContextSettings>& contextSettings)
{
	mTitle = title;
	mWindow = std::make_unique<sf::RenderWindow>(videoMode, title, styleFlags, state, contextSettings.value_or({}));
	ReadConfig();

	circle.setFillColor(sf::Color::White);
	circle.setRadius(25.0f);

	std::filesystem::path path = "layout.layout";
	auto createLayout = [this, path]() {
		CLayoutParser parser;
		if (auto result = parser.ParseLayoutFromFile(path); result.has_value())
		{
			mLayout = std::unique_ptr<CLayout>(result.value());
			mLayout->SetWidth(mWindow->getSize().x);
			mLayout->SetHeight(mWindow->getSize().y);
			mLayout->OnLayout();
		}
	};

	if (auto fileWatcher = CApp::Instance().GetService<CFileWatcher>().lock())
	{
		fileWatcher->Observe(path, createLayout);
	}
	createLayout();
}

std::optional<sf::RenderWindow*> CWindow::GetRenderWindow()
{
	if (mWindow && mWindow->isOpen())
	{
		return mWindow.get();
	}
	return std::nullopt;
}

std::string orca::CWindow::GetConfigFilename()
{
	if (mConfigFilename.empty())
	{
		std::string titleLowercase = mTitle;
		std::transform(titleLowercase.begin(), titleLowercase.end(), titleLowercase.begin(), ::tolower);
		utils::ReplaceAll(titleLowercase, " ", "_", mConfigFilename);
		mConfigFilename.append(".windowconfig");
	}
	return mConfigFilename;
}

void CWindow::ReadConfig()
{
	std::string buffer;
	CWindow::SWindowConfig config;

	if (glz::read_file_json(config, GetConfigFilename(), buffer) != glz::error_code::none)
	{
		// Failed to read config, setting default values
		config.refreshRate = 60.0;
		config.width = 1600;
		config.height = 900;
		config.posX = 0;
		config.posY = 0;
	}

	mWindow->setPosition({ config.posX, config.posY });
	mWindow->setSize({ config.width, config.height });
	mFps = config.refreshRate;
}

void CWindow::WriteConfig()
{
	SWindowConfig config;
	auto position = mWindow->getPosition();
	config.posX = position.x;
	config.posY = position.y;
	auto size = mWindow->getSize();
	config.width = size.x;
	config.height = size.y;
	config.refreshRate = mFps;

	std::string buffer;
	if (glz::write_file_json<glz::opts{ .prettify = true } > (config, GetConfigFilename(), buffer) != glz::error_code::none)
	{
		// Failed
	}
}

void CWindow::SetFramerate(double fps)
{
	mFps = fps;
}

void CWindow::EventLoop()
{
	mWindow->handleEvents(*this);

	if (mClock.getElapsedTime().asSeconds() >= 1.f / mFps)
	{
		float dt = mClock.restart().asSeconds();
		mLifetime += dt;
		if (!pause)
		{
			if (mLayout)
			{
				if (auto obj = mLayout->FindObject("Popup").lock())
				{
					auto hSize = ((sf::Vector2f)mWindow->getSize()) * 0.5f;
					auto pos = hSize + sf::Vector2f(150.f * std::sinf(mLifetime), 150.f * std::cosf(mLifetime));
					obj->SetX(pos.x);
					obj->SetY(pos.y);
				}
			}

			auto hSize = ((sf::Vector2f)mWindow->getSize()) * 0.5f;
			circle.setPosition(hSize + sf::Vector2f(150.f * std::sinf(mLifetime), 150.f * std::cosf(mLifetime)));
			if (mLayout)
			{
				mLayout->Update();
			}
			Render();
		}
	}
}

DEFINE_EVENT_HANDLER(sf::Event::Closed event)
{
	std::puts("CLOSED");
	WriteConfig();
	mWindow->close();
}

DEFINE_EVENT_HANDLER(sf::Event::Resized event)
{
	auto size = event.size;
	auto view = mWindow->getView();
	view.setSize(sf::Vector2f{ (float)size.x, (float)size.y });
	view.setCenter(0.5f * view.getSize());
	mWindow->setView(view);

	if (mLayout)
	{
		mLayout->SetWidth(size.x);
		mLayout->SetHeight(size.y);
		mLayout->OnLayout();
	}
	std::puts(std::format("RESIZED: W:{} H:{}", size.x, size.y).c_str());
}

DEFINE_EVENT_HANDLER(sf::Event::KeyPressed event)
{
	if (event.code == sf::Keyboard::Key::Space)
	{
		pause = !pause;
	}
}

DEFINE_EVENT_HANDLER(sf::Event::KeyReleased event)
{
}

DEFINE_EVENT_HANDLER(sf::Event::MouseMoved event)
{
	if (mLayout)
	{
		mLayout->HandleInput(event.position);
	}
}

void CWindow::Render()
{
	mWindow->clear();
	if (mLayout)
	{
		mWindow->draw(*mLayout);
	}
	mWindow->draw(circle);
	mWindow->display();
}
