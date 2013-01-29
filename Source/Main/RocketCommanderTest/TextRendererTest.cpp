#include "stdafx.h"
#include "TestGame.h"
#include "TextRenderer.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(TextRendererTest)

BOOST_AUTO_TEST_CASE(SetTextTest)
{
	class SetTextTestGame : public TestGame
	{		
	public:
		SetTextTestGame::~SetTextTestGame()
		{
			TextRenderer::getSingleton().RemoveTextBox("txtDebug");
		}

	private:
		virtual void InitTest()
		{
			// Ogre overlay
			const Ogre::Real margin = 10;
			TextRenderer::getSingleton().AddTextBox("txtDebug", "Debug", 
				m_viewport->getActualLeft() + margin, m_viewport->getActualTop() + margin + m_fpsOverlayHeight, 
				m_viewport->getActualWidth() - margin, m_viewport->getActualHeight() - margin);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			TextRenderer::getSingleton().SetText("txtDebug", "SetTextTestGame");
		}
	};

	SetTextTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()