#pragma once

#include "BaseGame.h"

namespace RocketCommander
{
	class TestGame : public BaseGame, OIS::KeyListener, OIS::MouseListener
	{
	public:
		TestGame();
		virtual ~TestGame() {}

	protected:
		virtual void InitTest() {}
		virtual void UpdateTest(double timeSinceLastFrame) {}

		void ShowCameraInfo(bool show = true);

		// Overrides BaseGame
		virtual void Initialize();
		virtual void LoadContent();
		virtual void Update(double timeSinceLastFrame);

		// Implements OIS::KeyListener
		virtual bool keyPressed(const OIS::KeyEvent &evt) { return true; }
		virtual bool keyReleased(const OIS::KeyEvent &evt) { return true; }

		// Implements OIS:MouseListener
		virtual bool mouseMoved(const OIS::MouseEvent &evt);
		virtual bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

		CEGUI::Window		*m_leftTopDebug;
		CEGUI::Window		*m_rightTopDebug;
		CEGUI::Window		*m_leftBottomDebug;
		CEGUI::Window		*m_rightBottomDebug;

	private:
		bool OnGUISheetChanged(const CEGUI::EventArgs &e);

		bool			m_showCameraInfo;
		CEGUI::Window	*m_debugWindow;
	};
}