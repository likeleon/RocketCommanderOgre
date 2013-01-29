#pragma once

namespace RocketCommander
{
	class Game;

	class GraphicsDeviceManager
	{
	public:
		static const unsigned int DefaultWidth = 1024;
		static const unsigned int DefaultHeight = 768;
		static const bool DefaultIsFullScreen = false;

		GraphicsDeviceManager(Game *game);

		void SetWidth(unsigned int width);
		unsigned int GetWidth(void) const { return m_width; }
		
		void SetHeight(unsigned int height);
		unsigned int GetHeight(void) const { return m_height; }

		void SetFullScreen(bool isFullScreen) { m_isFullScreen = isFullScreen; }
		bool IsFullScreen() const { return m_isFullScreen; }

	private:
		unsigned int m_width;
		unsigned int m_height;
		bool m_isFullScreen;
	};
}