#include "stdafx.h"
#include "TestGame.h"
#include "LensFlare.h"
#include "Sprite.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(LensFlareTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGame
	{
	public:
		RenderTestGame() : m_showSky(true) {}

	private:
		virtual void InitTest()
		{
			EnableSkyBox(m_showSky);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			EnableSkyBox(m_showSky);

			m_lensFlare->Render(Ogre::ColourValue::White);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_B)
				m_showSky = !m_showSky;
			return true;
		}

		bool m_showSky;
	};
	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()