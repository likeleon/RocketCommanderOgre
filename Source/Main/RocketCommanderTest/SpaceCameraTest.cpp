#include "stdafx.h"
#include "TestGame.h"
#include "SpaceCamera.h"
#include "Player.h"
#include "GameAsteroidManager.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(SpaceCameraTest)

BOOST_AUTO_TEST_CASE(CameraModeTest)
{
	class CameraModeTestGame : public TestGame
	{
		virtual void InitTest()
		{
			ShowCameraInfo();
			m_inGameMode = true;

			EnableSkyBox();
			GetSpaceCamera().SetInGame(m_inGameMode);

			m_asteroidManager.reset(new GameAsteroidManager(*this, Level::LoadAllLevels()[0]));
			GetPlayer().SetAsteroidManager(m_asteroidManager.get());
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			GetSpaceCamera().Update(timeSinceLastFrame);
			m_asteroidManager->Update();
			GetPlayer().HandleGameLogic();
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1)
			{
				m_inGameMode = !m_inGameMode;	
				GetSpaceCamera().SetInGame(m_inGameMode);
			}
			return TestGame::keyPressed(evt);
		}

		bool m_inGameMode;
		boost::shared_ptr<GameAsteroidManager> m_asteroidManager;
	};

	CameraModeTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()