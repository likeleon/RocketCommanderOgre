#include "stdafx.h"
#include "Game.h"
#include "Ogre.h"
#include "OIS.h"
#include "GraphicsDeviceManager.h"
#include "Directories.h"

using namespace Ogre;

namespace RocketCommander
{
	Game::Game()
	: m_root(NULL)
	, m_renderWnd(NULL)
	, m_log(NULL)
	, m_timer(NULL)
	, m_inputMgr(NULL)
	, m_keyboard(NULL)
	, m_mouse(NULL)
	, m_firstRun(true)
	, m_shutdownOgre(false)
	, m_graphics(NULL)
	, m_lastRun(false)
	{
	}

	Game::~Game()
	{
		Shutdown();
	}

	void Game::CreateRoot()
	{
		m_root = new Root("", Directories::GetWritablePath("ogre.cfg"));

		// Load render system
#ifdef _DEBUG
		m_root->loadPlugin("RenderSystem_GL_d");
		m_root->loadPlugin("RenderSystem_Direct3D9_d");
#else
		m_root->loadPlugin("RenderSystem_GL");
		m_root->loadPlugin("RenderSystem_Direct3D9");
#endif

		// Load common plugins
#ifdef _DEBUG
		m_root->loadPlugin("Plugin_OctreeSceneManager_d");
		m_root->loadPlugin("Plugin_CgProgramManager_d");
#else
		m_root->loadPlugin("Plugin_OctreeSceneManager");
		m_root->loadPlugin("Plugin_CgProgramManager");
#endif
	}

	bool Game::CreateRenderWindow(RenderSystem *requestedRenderSystem)
	{
		// Try lload an existing ogre config, failing that try default the rendering settings
		if (!m_root->restoreConfig() && !DefaultRenderWindowConfig(false))
			return false;

		// Initialize ogre root using the loaded config, telling it to create a render window for us
		try
		{
			if (requestedRenderSystem != NULL)
				m_root->setRenderSystem(requestedRenderSystem);
			
			m_renderWnd = m_root->initialise(true);
		}
		catch (Ogre::Exception &)
		{
			// If this failed, the settings are invalid, try defaulting them fully before trying again
			if (!DefaultRenderWindowConfig(true))
				return false;

			try
			{
				m_renderWnd = m_root->initialise(true);
			}
			catch (Ogre::Exception &)
			{
				// Epic fail, just give up...
				return false;
			}
		}
		return true;
	}

	void Game::Setup()
	{
		SetupInput();

		// Initialize game
		Initialize();

		TextureManager::getSingleton().setDefaultNumMipmaps(5);

#ifdef USE_OGRE_RENDER_LOOP
		m_root->addFrameListener(this);
#endif
		WindowEventUtilities::addWindowEventListener(m_renderWnd, this);
	}

	void Game::SetupInput()
	{
		unsigned long hWnd = 0;
		OIS::ParamList paramList;
		m_renderWnd->getCustomAttribute("WINDOW", &hWnd);

		paramList.insert(OIS::ParamList::value_type("WINDOW", StringConverter::toString(hWnd)));

		m_inputMgr = OIS::InputManager::createInputSystem(paramList);

		m_keyboard = static_cast<OIS::Keyboard *>(m_inputMgr->createInputObject(OIS::OISKeyboard, true));
		m_mouse = static_cast<OIS::Mouse *>(m_inputMgr->createInputObject(OIS::OISMouse, true));

		windowResized(m_renderWnd);
	}

	void Game::Run()
	{
		LogManager *logMgr = new LogManager();
		m_log = LogManager::getSingleton().createLog(Directories::GetWritablePath("ogre.log"), true, true, false);
		m_log->setDebugOutputEnabled(true);

		while (!m_lastRun)
		{
			// Assume this is our last run
			m_lastRun = true;

			CreateRoot();

			RenderSystem *desiredRenderSystem = NULL;
			if (!m_firstRun)
			{
				// if the context was reconfigured, set requested renderer
				desiredRenderSystem = m_root->getRenderSystemByName(m_nextRenderer);
			}

			if (!CreateRenderWindow(desiredRenderSystem))
				return;

			Setup();

			// Initialization done, prepare main loop...
			if (!m_timer)
			{
				m_timer = new Timer();
				m_timer->reset();
			}

			m_renderWnd->setActive(true);

			// Start main loop
			m_log->logMessage("Start main loop...");

#ifdef USE_OGRE_RENDER_LOOP
			m_root->startRendering();
#else
			double timeSinceLastFrame = 0;

			m_shutdownOgre = false;
			while (!m_shutdownOgre)
			{
				if (m_renderWnd->isClosed())
					m_shutdownOgre = true;

				WindowEventUtilities::messagePump();
				
				double startTime = m_timer->getMillisecondsCPU();
				
				m_keyboard->capture();
				m_mouse->capture();
				
				Update(timeSinceLastFrame);
				m_root->renderOneFrame();

				timeSinceLastFrame = m_timer->getMillisecondsCPU() - startTime;
			}
#endif

			m_log->logMessage("Quit main loop...");

			m_root->saveConfig();
			
			// Cleanup game
			Cleanup();

			m_firstRun = false;
		}
	}

#ifdef USE_OGRE_RENDER_LOOP
	bool Game::frameStarted(const FrameEvent& evt)
	{
		m_keyboard->capture();
		m_mouse->capture();
		return true;
	}

	bool Game::frameRenderingQueued(const FrameEvent& evt)
	{
		Update(evt.timeSinceLastFrame * 1000.0);
		return true;
	}

	bool Game::frameEnded(const FrameEvent& evt)
	{
		if (m_renderWnd->isClosed())
			return false;
		return true;
	}
#endif

	void Game::Shutdown()
	{
		m_log->logMessage("Shutdown OGRE...");

		WindowEventUtilities::removeWindowEventListener(m_renderWnd, this);
		
		if (m_inputMgr)
		{
			m_inputMgr->destroyInputObject(m_keyboard);
			m_inputMgr->destroyInputObject(m_mouse);
			OIS::InputManager::destroyInputSystem(m_inputMgr);
			m_inputMgr = NULL;
		}

		if (m_root)
		{
			delete m_root;
			m_root = NULL;
		}
	}

	void Game::Initialize()
	{
		LoadContent();
	}

	void Game::LoadContent()
	{
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	void Game::Cleanup()
	{
		Shutdown();
	}


	OIS::Keyboard& Game::GetKeyboard()
	{ 
		assert(m_keyboard);
		return *m_keyboard; 
	}

	OIS::Mouse& Game::GetMouse()
	{ 
		assert(m_mouse);
		return *m_mouse; 
	}

	void Game::ExitGame(bool reconfigure)
	{
#ifdef USE_OGRE_RENDER_LOOP
		m_root->queueEndRendering(); 
#else
		m_shutdownOgre = true;
#endif
		m_lastRun = !reconfigure;
	}

	void Game::windowResized(Ogre::RenderWindow* rw)
	{
		assert(rw);

		m_mouse->getMouseState().height = m_renderWnd->getHeight();
		m_mouse->getMouseState().width = m_renderWnd->getWidth();
	}

	void Game::Reconfigure(const Ogre::String &renderer, Ogre::NameValuePairList &options)
	{
		m_nextRenderer = renderer;
		RenderSystem *rs = m_root->getRenderSystemByName(renderer);
		assert(rs);

		// Set all given render system options
		for (NameValuePairList::iterator it = options.begin(); it != options.end(); ++it)
		{
			rs->setConfigOption(it->first, it->second);
		}

		ExitGame(true);
	}

	Root& Game::GetRoot()
	{
		assert(m_root);
		return *m_root;
	}

	bool Game::DefaultRenderWindowConfig(bool fullReset)
	{
		// Default to using D3D for win32
		m_root->setRenderSystem(m_root->getRenderSystemByName("Direct3D9 Rendering Subsystem"));

		// If that failed, try to use OpenGL
		if (!m_root->getRenderSystem())
			m_root->setRenderSystem(m_root->getRenderSystemByName("OpenGL Rendering Subsystem"));

		RenderSystem *renderSystem = m_root->getRenderSystem();
		if (!renderSystem)
			return false;

		if (fullReset)
		{
			// Default all the config options to sane values
			ConfigOptionMap &options = renderSystem->getConfigOptions();
			for (ConfigOptionMap::iterator option = options.begin(); option != options.end(); ++option)
			{
				StringVector &possibleValues = option->second.possibleValues;
				renderSystem->setConfigOption(option->first, (!possibleValues.empty())? possibleValues[0] : "");
			}
		}
		else
		{
			// Find best-fit option values
			const StringVector &videoModes = m_root->getRenderSystem()->getConfigOptions().find("Video Mode")->second.possibleValues;
			unsigned int highestWidth = 0;
			unsigned int highestHeight = 0;
			for (StringVector::const_iterator videoMode = videoModes.begin(); videoMode != videoModes.end(); ++videoMode)
			{
				StringVector vmopts = StringUtil::split((*videoMode), " x");
				const unsigned int width = StringConverter::parseUnsignedInt(vmopts[0]);
				const unsigned int height = StringConverter::parseUnsignedInt(vmopts[1]);

				if (width >= highestWidth && height >= highestHeight)
				{
					highestWidth = width;
					highestHeight = height;
				}
			}

			renderSystem->setConfigOption("Full Screen", "Yes");
			renderSystem->setConfigOption("VSync", "No");

			String bestVideoMode = StringConverter::toString(highestWidth) + " x " + StringConverter::toString(highestHeight);
			if (m_root->getRenderSystem()->getName().find("Direct3D") != std::string::npos)
			{
				bestVideoMode += " @ 32-bit colour";
			}
			renderSystem->setConfigOption("Video Mode", bestVideoMode);
		}
		renderSystem->setConfigOption("sRGB Gamma Conversion", "No");
		//renderSystem->setConfigOption("VSync Interval", "1");

		m_root->saveConfig();
		return true;
	}
}