#pragma once

#include "GameScreen.h"

namespace RocketCommander
{
	class MainMenuScreen : public GameScreen
	{
	public:
		MainMenuScreen(RocketCommanderGame &game);

		// Overrides GameScreen
		virtual Ogre::String GetName() const { return "Main Menu"; }
		virtual void Enter();

	private:
		bool OnMissionsButtonClicked(const CEGUI::EventArgs &e);
		bool OnCreditsButtonClicked(const CEGUI::EventArgs &e);
		bool OnHelpButtonClicked(const CEGUI::EventArgs &e);
		bool OnOptionsButtonClicked(const CEGUI::EventArgs &e);
		bool OnExitButtonClicked(const CEGUI::EventArgs &e);
		bool OnButtonMouseEnter(const CEGUI::EventArgs &e);

		// Overrides GameScreen
		virtual bool keyPressed(const OIS::KeyEvent &evt);

		bool m_resetCamera;
	};
}