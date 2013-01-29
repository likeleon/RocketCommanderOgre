#include "stdafx.h"
#include <map>
#include "TestGame.h"
#include "CEGuiHelpers.h"
#include "GraphicsDeviceManager.h"

using namespace RocketCommander;
using namespace CEGUI;

BOOST_AUTO_TEST_SUITE(GuiTest)

BOOST_AUTO_TEST_CASE(ShowMouseCursorTest)
{
	TestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(LayoutTest)
{
	class LayoutTestGame : public TestGame
	{
		virtual void InitTest()
		{
			m_showGameBackground = false;

			ImagesetManager::getSingleton().create("MainMenu.imageset");
			ImagesetManager::getSingleton().create("EasyFlight.imageset");
			ImagesetManager::getSingleton().create("LostCivilization.imageset");
			ImagesetManager::getSingleton().create("ValleyofDeath.imageset");
			ImagesetManager::getSingleton().create("TheRevenge.imageset");
			ImagesetManager::getSingleton().create("ExtraButtons.imageset");
			ImagesetManager::getSingleton().create("HelpScreen.imageset");
			ImagesetManager::getSingleton().create("Hud.imageset");
			ImagesetManager::getSingleton().create("InGame.imageset");

			std::string layouts[] = {"MainMenu", "MissionSelection", "Highscores", "Credits", "Help", "Options", "Mission"};
			BOOST_FOREACH(std::string layout, layouts)
			{
				Window *w = WindowManager::getSingleton().loadWindowLayout(layout + ".layout");
				m_windows.push_back(std::make_pair(layout, w));
			}
			
			m_nowWindowIndex = 0;
			SetWindow(m_windows[m_nowWindowIndex]);
			
			CEGuiHelpers::SubscribeEvent("MainMenu/ExitButton", PushButton::EventClicked, 
				Event::Subscriber(&LayoutTestGame::OnExitButtonClicked, this));
		}
		
		bool OnExitButtonClicked(const CEGUI::EventArgs &e)
		{
			ExitGame();
			return true;
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key >= OIS::KC_1 && evt.key < OIS::KC_1 + (int)m_windows.size())
			{
				m_nowWindowIndex = evt.key - OIS::KC_1;
				SetWindow(m_windows[m_nowWindowIndex]);
			}
			else if (evt.key == OIS::KC_0)
			{
				m_showGameBackground = !m_showGameBackground;
				EnableSkyBox(m_showGameBackground);
			}
			return true;
		}

		void SetWindow(const std::pair<std::string, Window *> &window)
		{
			System::getSingleton().setGUISheet(window.second);
			m_leftTopDebug->setText(window.first);
		}

		std::vector<std::pair<std::string, Window *> > m_windows;
		int m_nowWindowIndex;
		bool m_showGameBackground;
	};

	LayoutTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(GameFontTest)
{
	class GameFontTest : public TestGame
	{
	public:
		GameFontTest()
		{
			m_graphics->SetWidth(800);
			m_graphics->SetHeight(600);
		}

	private:
		virtual void InitTest()
		{
			//Font &font = FontManager::getSingleton().createFreeTypeFont("DefaultFont", 30, true, "DejaVuSans.ttf");
			//Font &font = FontManager::getSingleton().create("DejaVuSans-10.font", "GUI");
			Font &font = FontManager::getSingleton().create("courbd-10.font", "GUI");
			System::getSingleton().setDefaultFont(&font);

			WindowManager& winMgr = WindowManager::getSingleton ();
			Window *window = winMgr.createWindow("TaharezLook/StaticText");
			window->setArea(URect(cegui_reldim(0), cegui_reldim(0), cegui_reldim(1), cegui_reldim(1)));
			window->setProperty("FrameEnabled", "false");
			window->setProperty("HorzFormatting", "WordWrapCentred");
			System::getSingleton().setGUISheet(window);
			
			window->setText("!\"#$%^'()*+,-./0\n123456789:;<=\n>?@ABCDEFGHIJKLMNOPQRST\nUVWXYZ[\\]^_`a\n"
				"bcdefghijklmnopqrstuvwxyz{|}~\n\n"
				"Try Catching The Brown Fox While It's Jumping Over The Lazy Dog");
		}
	};

	GameFontTest game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(DebugWindowsTest)
{
	class DebugWindowsTest : public TestGame
	{
		virtual void InitTest()
		{
			m_leftTopDebug->setText("Left-Top");
			m_rightTopDebug->setText("Right-Top");
			m_leftBottomDebug->setText("Left-Bottom");
			m_rightBottomDebug->setText("Right-Bottom");
		}
	};

	DebugWindowsTest game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()