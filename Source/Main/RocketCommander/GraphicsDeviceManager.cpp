#include "stdafx.h"
#include "GraphicsDeviceManager.h"
#include "Game.h"

namespace RocketCommander
{
	GraphicsDeviceManager::GraphicsDeviceManager(Game *game)
	: m_width(DefaultWidth),
	  m_height(DefaultHeight),
	  m_isFullScreen(DefaultIsFullScreen)
	{
		// TODO: assertion game != NULL
		game->SetGraphicsDeviceManager(this);
	}

	void GraphicsDeviceManager::SetWidth(unsigned int width)
	{
		m_width = width;
		if (m_width == 0)
			m_width = DefaultWidth;
	}

	void GraphicsDeviceManager::SetHeight(unsigned int height)
	{
		m_height = height;
		if (m_height == 0)
			m_height = DefaultHeight;
	}
}