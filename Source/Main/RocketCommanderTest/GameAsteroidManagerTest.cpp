#include "stdafx.h"
#include "TestGame.h"
#include "GameAsteroidManager.h"
#include "LensFlare.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(GameAsteroidManagerTest)

BOOST_AUTO_TEST_CASE(RenderSceneTest)
{
	class RenderSceneTestGame : public TestGame
	{
		virtual void InitTest()
		{
			ShowCameraInfo();
			EnableSkyBox();

			m_levels = Level::LoadAllLevels();

			m_nowLevel = 0;
			m_asteroidManager.reset(new GameAsteroidManager(*this, m_levels[m_nowLevel]));
			m_asteroidManager->SetLevel(m_levels[m_nowLevel]);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			m_asteroidManager->Update();
			m_lensFlare->Render(m_remLensFlareColor);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key >= OIS::KC_1 && evt.key < OIS::KC_1 + static_cast<int>(m_levels.size()))
			{
				m_nowLevel = evt.key - OIS::KC_1;
				m_asteroidManager->SetLevel(m_levels[m_nowLevel]);
			}
			else if (evt.key == OIS::KC_1 + m_levels.size())
			{
				const Vector3 &targetPos = m_asteroidManager->GetTargetPosition();
				GetCamera().setPosition(targetPos.x, targetPos.y, targetPos.z - 5);
				GetCamera().lookAt(targetPos);
			}
			else if (evt.key == OIS::KC_1 + m_levels.size() + 1)
			{
				GetCamera().setPosition(0, 0, -5.0f);
				GetCamera().lookAt(Vector3::ZERO);
			}
			return TestGame::keyPressed(evt);
		}

		boost::shared_ptr<GameAsteroidManager> m_asteroidManager;
		std::vector<Level> m_levels;
		unsigned int m_nowLevel;
	};

	RenderSceneTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
