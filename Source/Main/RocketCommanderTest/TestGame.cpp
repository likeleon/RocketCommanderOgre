#include "stdafx.h"
#include "TestGame.h"
#include "CEGuiHelpers.h"
#include "SpaceCamera.h"
#include "Directories.h"

using namespace CEGUI;

namespace RocketCommander
{
	TestGame::TestGame()
	: BaseGame(),
	  m_showCameraInfo(false),
	  m_leftTopDebug(NULL),
	  m_rightTopDebug(NULL),
	  m_leftBottomDebug(NULL),
	  m_rightBottomDebug(NULL),
	  m_debugWindow(NULL)
	{
	}

	void TestGame::Initialize()
	{
		BaseGame::Initialize();

		m_keyboard->setEventCallback(this);
		m_mouse->setEventCallback(this);

		m_showFps = true;	// always show fps statistics

		// Create debug windows
		Font &font = FontManager::getSingleton().create("courbd-10.font", "GUI");
		WindowManager &winMgr = WindowManager::getSingleton();
		m_debugWindow = winMgr.loadWindowLayout("TestGameDebug.layout");
		m_leftTopDebug = winMgr.getWindow("LeftTopDebug");
		m_rightTopDebug = winMgr.getWindow("RightTopDebug");
		m_leftBottomDebug = winMgr.getWindow("LeftBottomDebug");
		m_rightBottomDebug = winMgr.getWindow("RightBottomDebug");
		assert(m_leftTopDebug && m_rightTopDebug && m_leftBottomDebug && m_rightBottomDebug);

		System::getSingleton().subscribeEvent(System::EventGUISheetChanged, Event::Subscriber(&TestGame::OnGUISheetChanged, this));
		System::getSingleton().setGUISheet(m_debugWindow);

		// Initialize camera position and lookat
		GetCamera().setPosition(Ogre::Vector3(0, 0, 1.5));
		GetCamera().lookAt(Ogre::Vector3(0, 0, 0));

		// Call test initialization code
		InitTest();
	}

	void TestGame::LoadContent()
	{
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/Tests", "FileSystem", "General");
		BaseGame::LoadContent();
	}

	void TestGame::Update(double timeSinceLastFrame)
	{
		const float moveSpeed = 0.1f;
		float moveScale = moveSpeed * (float)timeSinceLastFrame;
		Ogre::Vector3 translateVector = Ogre::Vector3::ZERO;

		if (m_keyboard->isKeyDown(OIS::KC_A))
			translateVector.x = -moveScale;

		if (m_keyboard->isKeyDown(OIS::KC_D))
			translateVector.x = moveScale;

		if (m_keyboard->isKeyDown(OIS::KC_W))
			translateVector.z = -moveScale;

		if (m_keyboard->isKeyDown(OIS::KC_S))
			translateVector.z = moveScale;

		if (m_keyboard->isKeyDown(OIS::KC_LSHIFT)) 
			GetCamera().moveRelative(translateVector);
		else
			GetCamera().moveRelative(translateVector / 10);

		if (m_showCameraInfo)
		{
			const Ogre::Vector3 camPos = GetCamera().getPosition();
			const Ogre::Vector3 camDir = GetCamera().getDirection();

			String camInfo = "CamPos=" + Ogre::StringConverter::toString(camPos) +
				"\nCamDir=" + Ogre::StringConverter::toString(camDir);
			m_leftTopDebug->setText(camInfo);
		}

		BaseGame::Update(timeSinceLastFrame);

		UpdateTest(timeSinceLastFrame);

		if (m_keyboard->isKeyDown(OIS::KC_ESCAPE))
			ExitGame();
	}

	bool TestGame::mouseMoved(const OIS::MouseEvent &evt)
	{
		GetCamera().yaw(Ogre::Degree(evt.state.X.rel * -0.1f));
		GetCamera().pitch(Ogre::Degree(evt.state.Y.rel * -0.1f));

		return CEGuiHelpers::InjectMouseMove(evt);
	}

	bool TestGame::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return CEGuiHelpers::InjectMouseDown(id);
	}

	bool TestGame::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return CEGuiHelpers::InjectMouseReleased(id);
	}

	bool TestGame::OnGUISheetChanged(const EventArgs &e)
	{
		const WindowEventArgs &wea = static_cast<const WindowEventArgs &>(e);

		assert(m_debugWindow);
		
		if (wea.window && wea.window->isChild(m_debugWindow))
			wea.window->removeChildWindow(m_debugWindow);
		
		Window *rootWindow = System::getSingleton().getGUISheet();
		if (rootWindow && rootWindow != m_debugWindow)
			System::getSingleton().getGUISheet()->addChildWindow(m_debugWindow);
		else if (!rootWindow)
			System::getSingleton().setGUISheet(m_debugWindow);

		return true;
	}
	
	void TestGame::ShowCameraInfo(bool show)
	{
		if (m_showCameraInfo)
		{
			m_leftTopDebug->setText("");
		}

		m_showCameraInfo = show;
	}
}