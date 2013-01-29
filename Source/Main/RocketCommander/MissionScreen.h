#pragma once

#include "GameScreen.h"

namespace RocketCommander
{
	class MissionScreen : public GameScreen
	{
	public:
		MissionScreen(RocketCommanderGame &game, const Level &level);

		// Overrides GameScreen
		virtual Ogre::String GetName() const { return "Mission"; }
		virtual bool IsInGame() const { return true; }
		virtual void Enter();
		virtual void Run();

	private:
		void ZoomIntoRocket();
		void ShowHudContorls();
		void ShowScreenMessages();
		void ShowTarget(int levelLength);

		float m_rocketEndGameScale;
		CEGUI::ProgressBar *m_fuelBar;
		CEGUI::ProgressBar *m_healthBar;
		CEGUI::ProgressBar *m_speedBar;
		CEGUI::ProgressBar *m_lifeBar;
		CEGUI::Window	   *m_posText;
		CEGUI::Window	   *m_rankText;
		CEGUI::Window	   *m_timeText;
		CEGUI::Window	   *m_scoreText;
		CEGUI::Window	   *m_messageText;
		CEGUI::Window	   *m_targetIcon;
		Ogre::Vector2	   m_targetIconSize;
		CEGUI::Window	   *m_arrowRight;
		CEGUI::Window	   *m_arrowLeft;
		CEGUI::Window	   *m_arrowUp;
		CEGUI::Window	   *m_arrowDown;
		Ogre::Vector2      m_arrowSize;
	};
}