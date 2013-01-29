#include "stdafx.h"
#include "RocketCommanderGame.h"
#include "OptionsScreen.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(OptionsScreenTest)

BOOST_AUTO_TEST_CASE(RunTest)
{
	class LevelTestGame : public RocketCommanderGame
	{
	public:
		virtual void Initialize()
		{
			RocketCommanderGame::Initialize();
			
			AddGameScreen(new OptionsScreen(*this));
		}

		virtual void Update(double timeSinceLastFrame)
		{
			if (m_keyboard->isKeyDown(OIS::KC_ESCAPE))
				ExitGame();

			RocketCommanderGame::Update(timeSinceLastFrame);
		}
	};

	LevelTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()