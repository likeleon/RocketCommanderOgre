#include "stdafx.h"
#include "RocketCommanderGame.h"
#include "GameScreen.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(RocketCommanderGameTest)

BOOST_AUTO_TEST_CASE(RunTest)
{
	RocketCommanderGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(AddRemoveGameScreenTest)
{
	class SimpleGameScreen : public GameScreen
	{
	public:
		SimpleGameScreen(RocketCommanderGame &game) 
			: GameScreen(game, "non_existing.layout"), m_enterFired(0), m_runFired(0), m_exitFired(0), m_quit(false) {}
		virtual ~SimpleGameScreen()
		{
			BOOST_CHECK_EQUAL(1, m_enterFired);
			BOOST_CHECK_EQUAL(1, m_runFired);
			BOOST_CHECK_EQUAL(1, m_exitFired);
		}

		int m_enterFired;
		int m_runFired;
		int m_exitFired;
		bool m_quit;

	private:
		virtual Ogre::String GetName() const { return "SimpleGameScreen"; }
		virtual void Enter() { m_enterFired++; }
		virtual void Exit() { m_exitFired++; }
		virtual void Run() { m_runFired++; m_quit = true; }
		virtual bool GetQuit() const { return m_quit; }
	};

	class AddRemoveGameScreenTestGame : public RocketCommanderGame
	{
		virtual void Initialize()
		{
			RocketCommanderGame::Initialize();
			AddGameScreen(new SimpleGameScreen(*this));
		}

		virtual void Update(double timeSinceLastFrame)
		{
			if (m_gameScreens.size() == 1)
				ExitGame();

			RocketCommanderGame::Update(timeSinceLastFrame);
		}
	};

	AddRemoveGameScreenTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()