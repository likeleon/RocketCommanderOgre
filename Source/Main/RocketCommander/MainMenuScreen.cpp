#include "stdafx.h"
#include "MainMenuScreen.h"
#include "CEGuiHelpers.h"
#include "RocketCommanderGame.h"
#include "HelpScreen.h"
#include "CreditsScreen.h"
#include "MissionSelectionScreen.h"
#include "SpaceCamera.h"
#include "OptionsScreen.h"
#include "Sound.h"

namespace RocketCommander
{
	MainMenuScreen::MainMenuScreen(RocketCommanderGame &game)
	: GameScreen(game, "MainMenu.layout"),
	 m_resetCamera(true)
	{
	}

	void MainMenuScreen::Enter()
	{
		GameScreen::Enter();

		CEGuiHelpers::SubscribeEvent("MainMenu/MissionsButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnMissionsButtonClicked, this));
		CEGuiHelpers::SubscribeEvent("MainMenu/MissionsButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnButtonMouseEnter, this));

		CEGuiHelpers::SubscribeEvent("MainMenu/HighscoresButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnButtonMouseEnter, this));

		CEGuiHelpers::SubscribeEvent("MainMenu/CreditsButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnCreditsButtonClicked, this));
		CEGuiHelpers::SubscribeEvent("MainMenu/CreditsButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnButtonMouseEnter, this));

		CEGuiHelpers::SubscribeEvent("MainMenu/HelpButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnHelpButtonClicked, this));
		CEGuiHelpers::SubscribeEvent("MainMenu/HelpButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnButtonMouseEnter, this));

		CEGuiHelpers::SubscribeEvent("MainMenu/OptionsButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnOptionsButtonClicked, this));
		CEGuiHelpers::SubscribeEvent("MainMenu/OptionsButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnButtonMouseEnter, this));

		CEGuiHelpers::SubscribeEvent("MainMenu/ExitButton", CEGUI::PushButton::EventClicked, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnExitButtonClicked, this));
		CEGuiHelpers::SubscribeEvent("MainMenu/ExitButton", CEGUI::PushButton::EventMouseEnters, 
			CEGUI::Event::Subscriber(&MainMenuScreen::OnButtonMouseEnter, this));

		if (m_resetCamera)
		{
			m_game.GetCamera().setPosition(Ogre::Vector3::ZERO);
			m_game.GetSpaceCamera().SetInGame(false);
			m_resetCamera = false;
		}
	}

	bool MainMenuScreen::OnMissionsButtonClicked(const CEGUI::EventArgs &e)
	{
		m_game.AddGameScreen(new MissionSelectionScreen(m_game));
		return true;
	}

	bool MainMenuScreen::OnCreditsButtonClicked(const CEGUI::EventArgs &e)
	{
		m_game.AddGameScreen(new CreditsScreen(m_game));
		return true;
	}

	bool MainMenuScreen::OnHelpButtonClicked(const CEGUI::EventArgs &e)
	{
		m_game.AddGameScreen(new HelpScreen(m_game));
		return true;
	}

	bool MainMenuScreen::OnOptionsButtonClicked(const CEGUI::EventArgs &e)
	{
		m_game.AddGameScreen(new OptionsScreen(m_game));
		return true;
	}

	bool MainMenuScreen::OnExitButtonClicked(const CEGUI::EventArgs &e)
	{
		m_quit = true;
		return true;
	}

	bool MainMenuScreen::keyPressed(const OIS::KeyEvent &evt)
	{
		if (evt.key == OIS::KC_M)
			m_game.AddGameScreen(new MissionSelectionScreen(m_game));
		else if (evt.key == OIS::KC_C)
			m_game.AddGameScreen(new CreditsScreen(m_game));
		else if (evt.key == OIS::KC_H)
			m_game.AddGameScreen(new HelpScreen(m_game));
		else if (evt.key == OIS::KC_O)
			m_game.AddGameScreen(new OptionsScreen(m_game));
		return GameScreen::keyPressed(evt);
	}

	bool MainMenuScreen::OnButtonMouseEnter(const CEGUI::EventArgs &e)
	{
		m_game.GetSound().Play(Sound::Sounds_Highlight);
		return true;
	}

}