#pragma once

#define USE_OGRE_RENDER_LOOP

namespace RocketCommander
{
	class GraphicsDeviceManager;

	class Game : 
		public Ogre::WindowEventListener
#ifdef USE_OGRE_RENDER_LOOP
		, public Ogre::FrameListener
#endif
	{
	public:
		Game();
		virtual ~Game();

		void Run();

		// Set methods
		void SetGraphicsDeviceManager(GraphicsDeviceManager *graphics) { m_graphics = graphics; }

		// Get methods
		OIS::Keyboard& GetKeyboard();
		OIS::Mouse& GetMouse();
		Ogre::Root& GetRoot();

		// Reconfigures the context
		void Reconfigure(const Ogre::String &renderer, Ogre::NameValuePairList &options);

	protected:
		// Called after the Game created but before LoadContent
		virtual void Initialize();

		// Called when the game has determined that game logic and drawing needs to be processed.
		virtual void Update(double timeSinceLastFrame) {}

		// Called when graphics resources need to be loaded.
		virtual void LoadContent();

		// Called when the game need to free resources and other cleanup operations
		virtual void Cleanup();

		// Exit game.
		void ExitGame(bool reconfigure = false);

		// Overrides Ogre::WindowEventListener
		virtual void windowResized(Ogre::RenderWindow* rw);

		Ogre::Root			*m_root;
		Ogre::RenderWindow	*m_renderWnd;
		Ogre::Log			*m_log;
		Ogre::Timer			*m_timer;

		OIS::InputManager	*m_inputMgr;
		OIS::Keyboard		*m_keyboard;
		OIS::Mouse			*m_mouse;
		bool				m_firstRun;

	private:
#ifdef USE_OGRE_RENDER_LOOP
		// Overrides Ogre::FrameListener
		virtual bool frameStarted(const Ogre::FrameEvent& evt);
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
        virtual bool frameEnded(const Ogre::FrameEvent& evt);
#endif

		void CreateRoot();
		bool CreateRenderWindow(Ogre::RenderSystem *requestedRenderSystem);
		void Setup();
		void SetupInput();
		void Shutdown();
		bool DefaultRenderWindowConfig(bool fullReset);

		GraphicsDeviceManager *m_graphics;
		bool				  m_shutdownOgre;
		bool				  m_lastRun;
		std::string			  m_nextRenderer;
	};
}