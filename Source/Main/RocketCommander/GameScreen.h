#pragma once

#include "IGameScreen.h"
#include "RocketCommanderGame.h"

namespace RocketCommander
{
	// Default implementations of IGameScreen
	class GameScreen : public IGameScreen
	{
	public:
		GameScreen(RocketCommanderGame &game, const CEGUI::String &layoutFileName, bool quitOnEscapeKey = true);
		virtual ~GameScreen();

		// Implements IGameScreen
		virtual bool GetQuit() const { return m_quit; }
		virtual bool IsInGame() const { return false; }

		bool OnBackButtonMouseEnters(const CEGUI::EventArgs &e);
		bool OnBackButtonClicked(const CEGUI::EventArgs &e);

	protected:
		// Implements IGameScreen
		virtual void Enter();
		virtual void Exit();
		virtual void Run();

		// Implements OIS::KeyListener
		virtual bool keyPressed(const OIS::KeyEvent &evt);
		virtual bool keyReleased(const OIS::KeyEvent &evt) { return true; }

		// Implements OIS:MouseListener
		virtual bool mouseMoved(const OIS::MouseEvent &evt);
		virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

		RocketCommanderGame &m_game;
		const CEGUI::String m_layoutFileName;
		CEGUI::Window		*m_window;
		bool				m_quit;
		const bool			m_quitOnEscapeKey;
	};
}