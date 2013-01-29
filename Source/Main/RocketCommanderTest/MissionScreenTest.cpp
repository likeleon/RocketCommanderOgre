#include "stdafx.h"
#include "TestGame.h"
#include "Level.h"
#include "RocketCommanderGame.h"
#include "Directories.h"
#include "MissionScreen.h"
#include "TextRenderer.h"

using namespace RocketCommander;
using namespace CEGUI;

BOOST_AUTO_TEST_SUITE(MissionScreenTest)

BOOST_AUTO_TEST_CASE(GameHudTest)
{
	class GameHudTestGame : public TestGame
	{
		virtual void InitTest()
		{
			EnableSkyBox();

			ImagesetManager::getSingleton().create("Hud.imageset");
			ImagesetManager::getSingleton().create("InGame.imageset");

			Window *window = WindowManager::getSingleton().loadWindowLayout("Mission.layout");
			System::getSingleton().setGUISheet(window);

			m_progressBars.push_back(static_cast<ProgressBar *>(WindowManager::getSingleton().getWindow("Mission/FuelBar")));
			m_progressBars.push_back(static_cast<ProgressBar *>(WindowManager::getSingleton().getWindow("Mission/HealthBar")));
			m_progressBars.push_back(static_cast<ProgressBar *>(WindowManager::getSingleton().getWindow("Mission/SpeedBar")));
			m_progressBars.push_back(static_cast<ProgressBar *>(WindowManager::getSingleton().getWindow("Mission/RocketLife")));
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1 || evt.key == OIS::KC_2)
			{
				const bool increase = (evt.key == OIS::KC_2);
				BOOST_FOREACH(ProgressBar *progBar, m_progressBars)
				{
					const float amount = progBar->getStep() * (increase? 1.0f : -1.0f);
					progBar->setProgress(progBar->getProgress() + amount);
				}
			}
			return TestGame::keyPressed(evt);
		}

		std::vector<ProgressBar *> m_progressBars;
	};
	GameHudTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(EasyFlightLevelTest)
{
	class LevelTestGame : public RocketCommanderGame
	{
	public:
		virtual void Initialize()
		{
			RocketCommanderGame::Initialize();
			
			// Ogre overlay
			const Ogre::Real margin = 10;
			TextRenderer::getSingleton().AddTextBox("txtDebug", "Debug", 
				m_viewport->getActualLeft() + margin, m_viewport->getActualTop() + margin + m_fpsOverlayHeight, 
				m_viewport->getActualWidth() - margin, m_viewport->getActualHeight() - margin);

			AddGameScreen(new MissionScreen(*this, m_levels[0]));
		}

		virtual void Update(double timeSinceLastFrame)
		{
			if (m_keyboard->isKeyDown(OIS::KC_ESCAPE))
				ExitGame();

			const Ogre::Vector3 camPos = GetCamera().getPosition();
			TextRenderer::getSingleton().SetText("txtDebug", "CamPos={X:%f Y:%f Z:%f}", 
				camPos.x, camPos.y, camPos.z);

			RocketCommanderGame::Update(timeSinceLastFrame);
		}
	};

	LevelTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
