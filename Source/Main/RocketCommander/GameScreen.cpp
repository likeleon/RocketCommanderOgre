#include "stdafx.h"
#include "GameScreen.h"
#include "CEGuiHelpers.h"
#include "Sound.h"

namespace RocketCommander
{
	GameScreen::GameScreen(RocketCommanderGame &game, const CEGUI::String &layoutFileName, bool quitOnEscapeKey) 
		: m_game(game), m_layoutFileName(layoutFileName), m_window(NULL), m_quit(false), m_quitOnEscapeKey(quitOnEscapeKey)
	{
	}

	GameScreen::~GameScreen()
	{
		Exit();
	}

	bool GameScreen::mouseMoved(const OIS::MouseEvent &evt)
	{
		return CEGuiHelpers::InjectMouseMove(evt);
	}

	bool GameScreen::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return CEGuiHelpers::InjectMouseDown(id);
	}

	bool GameScreen::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return CEGuiHelpers::InjectMouseReleased(id);
	}

	void GameScreen::Enter()
	{
		using namespace CEGUI;

		m_window = WindowManager::getSingleton().loadWindowLayout(m_layoutFileName);
		assert(m_window);
		System::getSingleton().setGUISheet(m_window);
	}

	void GameScreen::Exit()
	{
		if (m_window)
		{
			CEGUI::System::getSingleton().setGUISheet(NULL);
			CEGUI::WindowManager::getSingleton().destroyWindow(m_window);
			m_window = NULL;
		}
	}

	void GameScreen::Run()
	{
	}

	bool GameScreen::OnBackButtonMouseEnters(const CEGUI::EventArgs &e)
	{
		m_game.GetSound().Play(Sound::Sounds_Highlight);
		return true;
	}

	bool GameScreen::OnBackButtonClicked(const CEGUI::EventArgs &e)
	{
		m_quit = true;
		return true;
	}

	bool GameScreen::keyPressed(const OIS::KeyEvent &evt)
	{
		if (m_quitOnEscapeKey && evt.key == OIS::KC_ESCAPE)
			m_quit = true;
		return true;
	}
}