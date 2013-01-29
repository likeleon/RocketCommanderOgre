#include "stdafx.h"
#include "RocketCommanderGame.h"
#include "IGameScreen.h"
#include "MainMenuScreen.h"
#include "Rocket.h"
#include "LensFlare.h"
#include "SpaceCamera.h"
#include "Level.h"
#include "GameAsteroidManager.h"
#include "Player.h"
#include "Sound.h"

using namespace Ogre;

namespace RocketCommander
{
	RocketCommanderGame::RocketCommanderGame()
	: m_asteroidManager(NULL)
	, m_rocketModel(NULL)
	, m_inGame(false)
	{
	}

	RocketCommanderGame::~RocketCommanderGame()
	{
		Cleanup();
	}

	void RocketCommanderGame::Initialize()
	{
		BaseGame::Initialize();

		// Space sky box
		EnableSkyBox();

		// Load all available levels
		m_levels = Level::LoadAllLevels();

		// Initialize asteroidmanager and use last available level.
		m_asteroidManager = new GameAsteroidManager(*this, m_levels[m_levels.size() - 1]);
		GetPlayer().SetAsteroidManager(m_asteroidManager);

		// Rocket
		m_rocketModel = new Rocket(*m_sceneMgr);

		// Reset camera
		GetCamera().setPosition(Vector3(0, 0, 0));

		// Load CEGUI imagesets
		CEGUI::ImagesetManager::getSingleton().create("MainMenu.imageset");
		CEGUI::ImagesetManager::getSingleton().create("EasyFlight.imageset");
		CEGUI::ImagesetManager::getSingleton().create("LostCivilization.imageset");
		CEGUI::ImagesetManager::getSingleton().create("ValleyofDeath.imageset");
		CEGUI::ImagesetManager::getSingleton().create("TheRevenge.imageset");
		CEGUI::ImagesetManager::getSingleton().create("ExtraButtons.imageset");
		CEGUI::ImagesetManager::getSingleton().create("HelpScreen.imageset");
		CEGUI::ImagesetManager::getSingleton().create("Hud.imageset");
		CEGUI::ImagesetManager::getSingleton().create("InGame.imageset");

		// Create main menu screen
		AddGameScreen(new MainMenuScreen(*this));
	}

	void RocketCommanderGame::Cleanup()
	{
		while (!m_gameScreens.empty())
		{
			delete m_gameScreens.top();
			m_gameScreens.pop();
		}

		if (m_rocketModel)
		{
			delete m_rocketModel;
			m_rocketModel = NULL;
		}

		if (m_asteroidManager)
		{
			delete m_asteroidManager;
			m_asteroidManager = NULL;
		}

		BaseGame::Cleanup();
	}
	
	void RocketCommanderGame::Update(double timeSinceLastFrame)
	{
		GetSpaceCamera().Update(timeSinceLastFrame);

		// Render rocket in front of view in menu mode
		if (GetSpaceCamera().IsInGame() == false)
		{
			Vector3 inFrontOfCameraPos = Vector3(0, -1.33f, -2.5f);
			inFrontOfCameraPos = GetCamera().getViewMatrix().inverse() * inFrontOfCameraPos;
			m_rocketModel->SetPosition(inFrontOfCameraPos);

			const Quaternion rocketOrient = GetCamera().getOrientation() *
				Quaternion(Radian(GetTotalTimeMs() / 8400.0f), Vector3::UNIT_Z) *
				Quaternion(-Radian(Ogre::Math::PI / 2.2f), Vector3::UNIT_X);
			m_rocketModel->SetOrientation(rocketOrient);
		}
		else if (GetPlayer().IsGameOver() == false)
		{
			// Zoom in when starting or a new life was started.
			if (GetPlayer().GetExplosionTimeoutMs() < 0  && 
				GetPlayer().GetLifeTimeMs() < 3000)
			{
				// Position
				Matrix3 invViewMat3;
				GetCamera().getViewMatrix().inverse().extract3x3Matrix(invViewMat3);
				
				Vector3 inFrontOfCameraPos = Vector3(0, -1.5f, -3.0f) * 2.5f;
				inFrontOfCameraPos = invViewMat3 * inFrontOfCameraPos;
				const Vector3 rocketPosition = 
					inFrontOfCameraPos * (1.0f - (GetPlayer().GetLifeTimeMs() / 3000.0f)) +
					Vector3(0, -0.485f, +0.4f) +
					GetCamera().getPosition();
				m_rocketModel->SetPosition(rocketPosition);

				// Orientation
				const Quaternion rocketOrient = GetCamera().getOrientation() *
					Quaternion(Radian(Ogre::Math::PI / 4.0f), Vector3::UNIT_Z) *
					Quaternion(-Radian(Ogre::Math::PI / 2.0f), Vector3::UNIT_X);
				m_rocketModel->SetOrientation(rocketOrient);
			}
		}

		m_asteroidManager->Update();

		m_lensFlare->Render(m_remLensFlareColor);

		// If that game screen should be quitted, remove it from stack
		if (!m_gameScreens.empty() &&
			m_gameScreens.top()->GetQuit())
			ExitCurrentGameScreen();

		// If no more game screens are left, it is time to quit
		if (m_gameScreens.empty())
			ExitGame();

		try
		{
			// Execute the game screen on top
			if (!m_gameScreens.empty())
				m_gameScreens.top()->Run();
		}
		catch (std::exception &e)
		{
			LogManager::getSingleton().logMessage("Failed to execute " + m_gameScreens.top()->GetName() + 
				"\nError: " + e.what());
		}

		BaseGame::Update(timeSinceLastFrame);
	}

	void RocketCommanderGame::AddGameScreen(IGameScreen *gameScreen)
	{
		assert(gameScreen);

		if (!m_gameScreens.empty())
		{
			m_gameScreens.top()->Exit();
		}
		EnterGameScreen(gameScreen, true);
	}

	void RocketCommanderGame::ChangeGameScreen(IGameScreen *gameScreen)
	{
		assert(gameScreen);
		assert(!m_gameScreens.empty());
		
		RemoveCurrentGameScreen();
		EnterGameScreen(gameScreen, true);
	}

	void RocketCommanderGame::EnterGameScreen(IGameScreen *gameScreen, bool push)
	{
		assert(gameScreen);

		m_inGame = gameScreen->IsInGame();

		// Space camera
		GetSpaceCamera().SetInGame(m_inGame);

		// Sound
		GetSound().SetCurrentMusicMode(m_inGame);

		// CEGUI
		if (m_inGame)
			CEGUI::MouseCursor::getSingleton().hide();
		else
			CEGUI::MouseCursor::getSingleton().show();

		// OIS
		m_keyboard->setEventCallback(gameScreen);
		m_mouse->setEventCallback(gameScreen);

		// Enable bloom...
		if (m_inGame && m_root->getRenderSystem()->getName().find("Direct3D") != std::string::npos)
		{
			CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", true);
		}

		// Enter game screen
		gameScreen->Enter();
		
		if (push)
			m_gameScreens.push(gameScreen);
	}

	void RocketCommanderGame::ExitCurrentGameScreen()
	{
		RemoveCurrentGameScreen();

		if (!m_gameScreens.empty())
			EnterGameScreen(m_gameScreens.top(), false);
	}

	void RocketCommanderGame::RemoveCurrentGameScreen()
	{
		assert(!m_gameScreens.empty());

		// Disable bloom...
		if (m_inGame && m_root->getRenderSystem()->getName().find("Direct3D") != std::string::npos)
		{
			CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", false);
		}

		m_keyboard->setEventCallback(NULL);
		m_mouse->setEventCallback(NULL);

		m_gameScreens.top()->Exit();
		delete m_gameScreens.top();
		m_gameScreens.pop();
	}

	GameAsteroidManager& RocketCommanderGame::GetAsteroidManager()
	{
		assert(m_asteroidManager);
		return *m_asteroidManager;
	}

	const GameAsteroidManager& RocketCommanderGame::GetAsteroidManager() const
	{
		assert(m_asteroidManager);
		return *m_asteroidManager;
	}

	Rocket& RocketCommanderGame::GetRocket()
	{
		assert(m_rocketModel);
		return *m_rocketModel;
	}

	const Rocket& RocketCommanderGame::GetRocket() const
	{
		assert(m_rocketModel);
		return *m_rocketModel;
	}
}