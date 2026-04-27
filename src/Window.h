#ifndef __ORCA_WINDOW__
#define __ORCA_WINDOW__

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics.hpp>

#include "Layout.h"

#define DECLARE_EVENT_HANDLER(e) void operator()(e)

const float DEFAULT_RENDER_FRAMERATE = 60.0f;

namespace orca
{
	class CWindow
	{
	public:

		struct SWindowConfig
		{
			double refreshRate;
			unsigned width, height;
			int posX, posY;
		};

		CWindow(const sf::VideoMode& videoMode, const std::string& title, uint32_t styleFlags = sf::Style::Default, sf::State state = sf::State::Windowed, const std::optional<sf::ContextSettings>& contextSettings = std::nullopt);
		CWindow(const CWindow&) = delete;
		CWindow(CWindow&&) = delete;

		std::optional<sf::RenderWindow*> GetRenderWindow();

		void SetFramerate(double fps);

		void EventLoop();

		/*
		* Event handlers
		*/
		DECLARE_EVENT_HANDLER(sf::Event::Closed);
		DECLARE_EVENT_HANDLER(sf::Event::Resized);
		DECLARE_EVENT_HANDLER(sf::Event::KeyPressed);
		DECLARE_EVENT_HANDLER(sf::Event::KeyReleased);

	private:
		std::string GetConfigFilename();
		void ReadConfig();
		void WriteConfig();
		void Render();

	private:
		std::string mTitle;
		std::string mConfigFilename;
		std::unique_ptr<sf::RenderWindow> mWindow;
		sf::Clock mClock;
		float mLifetime{ 0 };
		float mFps{ DEFAULT_RENDER_FRAMERATE };

		std::unique_ptr<CLayout> mLayout;
		sf::CircleShape circle;
		bool pause{ false };
	};
}

#endif