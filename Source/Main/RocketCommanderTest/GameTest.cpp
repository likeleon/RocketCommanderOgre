#include "stdafx.h"
#include "Game.h"
#include "GraphicsDeviceManager.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(GameTest)

BOOST_AUTO_TEST_CASE(RunTest)
{
	Game game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(UpdateTest)
{
	class UpdateTestGame : public Game
	{
	public:
		UpdateTestGame() : m_updateFired(false) {}
		bool m_updateFired;

	private:
		virtual void Update(double gameTime)
		{
			m_updateFired = true;
			ExitGame();
		}
	};

	UpdateTestGame game;
	BOOST_CHECK_EQUAL(false, game.m_updateFired);
	game.Run();
	
	BOOST_CHECK_EQUAL(true, game.m_updateFired);
}

BOOST_AUTO_TEST_CASE(KeyboardTest)
{
	class KeyboardTestGame : public Game
	{
	private:
		virtual void Update(double gameTime)
		{
			if (m_keyboard->isKeyDown(OIS::KC_ESCAPE))
				ExitGame();
		}
	};

	KeyboardTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(MouseTest)
{
	class MouseTestGame : public Game
	{
	private:
		virtual void Update(double gameTime)
		{
			if (m_mouse->getMouseState().buttonDown(OIS::MB_Left))
				ExitGame();
		}
	};

	MouseTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(ResolutionTest)
{
	class ResolutionTestGame : public Game
	{
	public:
		ResolutionTestGame() : desiredWidth(640), desiredHeight(480)
		{
			m_graphics.reset(new GraphicsDeviceManager(this));
			m_graphics->SetWidth(desiredWidth);
			m_graphics->SetHeight(desiredHeight);
		}

	private:
		virtual void Update(double gameTime)
		{
			if (m_keyboard->isKeyDown(OIS::KC_ESCAPE))
			{
				BOOST_CHECK_EQUAL(desiredWidth, m_graphics->GetWidth());
				BOOST_CHECK_EQUAL(desiredHeight, m_graphics->GetHeight());
				ExitGame();
			}
		}

		boost::shared_ptr<GraphicsDeviceManager> m_graphics;
		const unsigned int desiredWidth;
		const unsigned int desiredHeight;
	};

	ResolutionTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(FullScreenTest)
{
	class FullScreenTestGame : public Game
	{
	public:
		FullScreenTestGame() : m_fullScreen(true)
		{
			m_graphics.reset(new GraphicsDeviceManager(this));
			m_graphics->SetFullScreen(m_fullScreen);
		}

	private:
		virtual void Update(double gameTime)
		{
			if (m_keyboard->isKeyDown(OIS::KC_ESCAPE))
			{
				BOOST_CHECK_EQUAL(m_fullScreen, m_graphics->IsFullScreen());
				ExitGame();
			}
		}

		boost::shared_ptr<GraphicsDeviceManager> m_graphics;
		const bool m_fullScreen;
	};

	FullScreenTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(InitializeTest)
{
	class InitializeTestGame : public Game
	{
	public:
		InitializeTestGame() : m_initializeFired(false) {}
		bool m_initializeFired;

	private:
		virtual void Initialize() { m_initializeFired = true; Game::Initialize(); }
		virtual void Update(double gameTime) { ExitGame(); }
	};

	InitializeTestGame game;
	BOOST_CHECK_EQUAL(false, game.m_initializeFired);
	game.Run();
	BOOST_CHECK_EQUAL(true, game.m_initializeFired);
}

BOOST_AUTO_TEST_CASE(LoadContentTest)
{
	class LoadContentTestGame : public Game
	{
	public:
		LoadContentTestGame() : m_loadContentFired(false) {}
		bool m_loadContentFired;

	private:
		virtual void LoadContent() { m_loadContentFired = true; }
		virtual void Update(double gameTime) { ExitGame(); }
	};

	LoadContentTestGame game;
	BOOST_CHECK_EQUAL(false, game.m_loadContentFired);
	game.Run();
	BOOST_CHECK_EQUAL(true, game.m_loadContentFired);
}

BOOST_AUTO_TEST_SUITE_END()