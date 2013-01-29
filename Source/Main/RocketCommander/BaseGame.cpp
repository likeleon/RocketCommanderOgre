#include "stdafx.h"
#include "BaseGame.h"
#include "GraphicsDeviceManager.h"
#include "CEGUIRQListener.h"
#include "Sprite.h"
#include "LensFlare.h"
#include "TextRenderer.h"
#include "SpaceCamera.h"
#include "GameAsteroidManager.h"
#include "Directories.h"
#include "Player.h"
#include "RadialBlurListener.h"
#include "Sound.h"
#include "RandomHelper.h"

using namespace Ogre;

namespace RocketCommander
{
	const float BaseGame::m_fpsOverlayHeight = 20.0f;

	const Radian BaseGame::FieldOfView = Radian(Math::PI / 1.8f);
	const Real BaseGame::NearPlane = GameAsteroidManager::GetMinViewDepth();
	const Real BaseGame::FarPlane = GameAsteroidManager::GetMaxViewDepth();

	BaseGame::BaseGame()
		: m_graphics(NULL),
		  m_sceneMgr(NULL),
		  m_camera(NULL),
		  m_viewport(NULL),
		  m_sprite(NULL),
		  m_lensFlare(NULL),
		  m_remLensFlareColor(ColourValue::White),
#ifdef _DEBUG
		  m_showFps(true),
#else
		  m_showFps(true),
#endif
		  m_light(NULL),
		  m_sound(NULL),
		  m_backgroundColour(ColourValue(0, 0, 0)),
		  m_renderer(NULL),
		  m_textRenderer(NULL),
		  m_elapsedTimeThisFrameInMs(0.001f),
		  m_totalTimeMs(0.0f),
		  m_player(NULL),
		  m_totalFrameCount(0),
		  m_loadingTxtName("txtLoading"),
		  m_ceguiLogger(NULL)
	{
		m_graphics = new GraphicsDeviceManager(this);
		m_graphics->SetFullScreen(false);

	}

	BaseGame::~BaseGame()
	{
		Cleanup();
	}

	void BaseGame::Initialize()
	{
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/Fonts", "FileSystem", "Essential");
		ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");

		// Create scene manager
		m_sceneMgr = m_root->createSceneManager(ST_GENERIC, "SceneManager");
		m_sceneMgr->setAmbientLight(ColourValue::Black);

		// Light
		m_light = m_sceneMgr->createLight();
		m_light->setType(Light::LT_DIRECTIONAL);
		m_light->setDirection(1.0f, 0.0f, 0.0f);

		// Space camera
		m_camera = new SpaceCamera(*this, Vector3(0, 0, -5));
		assert(m_camera);
		GetCamera().setFOVy(FieldOfView);
		GetCamera().setNearClipDistance(NearPlane);
		GetCamera().setFarClipDistance(FarPlane);

		m_viewport = m_renderWnd->addViewport(&GetCamera());
		m_viewport->setBackgroundColour(m_backgroundColour);

		GetCamera().setAspectRatio(Real(m_viewport->getActualWidth()) / Real(m_viewport->getActualHeight()));	
		m_viewport->setCamera(&GetCamera());

		// Ogre overlay
		m_textRenderer = new TextRenderer();

		Game::Initialize();

		// Player
		m_player = new Player(*this);

		// Sprite
		m_sceneMgr->getRenderQueue()->getQueueGroup(RENDER_QUEUE_MAIN + 1)->setShadowsEnabled(false);
		m_sprite = new Sprite();
		m_sprite->Init(m_sceneMgr, m_viewport, RENDER_QUEUE_MAIN + 1, true);
		m_sprite->SetSpriteLocation(Directories::GetContentDirectory() + "/Sprites");

		// Lens flare
		m_lensFlare = new LensFlare(*this, LensFlare::DefaultSunPos);

		//
		// Compositors
		//
		CompositorManager &compMgr = CompositorManager::getSingleton();

		// Bloom (crash on d3d if device lost occur)
		compMgr.addCompositor(m_viewport, "Bloom");
		if (m_root->getRenderSystem()->getName().find("Direct3D") == std::string::npos)
		{
			compMgr.setCompositorEnabled(m_viewport, "Bloom", true);
		}

		// Radial blur
		CompositorInstance *radialBlur = compMgr.addCompositor(m_viewport, "Radial Blur");
		radialBlur->addListener(new RadialBlurListener(*m_player));
		compMgr.setCompositorEnabled(m_viewport, "Radial Blur", true);

		//
		// CEGUI
		//
		m_ceguiLogger = new CEGUI::DefaultLogger();
		assert(m_ceguiLogger);
		CEGUI::Logger::getSingleton().setLogFilename(Directories::GetWritablePath("cegui.log"));

		m_renderer = &CEGUI::OgreRenderer::bootstrapSystem(*m_renderWnd);

		// Disable calls to beginFrame and endFrame, which also disables
		// default rendering: we will use a custom rendering method.
		m_renderer->setFrameControlExecutionEnabled(true);
		
		// Hook back into the rendering process
		//m_sceneMgr->addRenderQueueListener(new CEGUIRQListener(m_renderer, RENDER_QUEUE_OVERLAY, false));

		CEGUI::Imageset::setDefaultResourceGroup("GUI");
		CEGUI::Font::setDefaultResourceGroup("GUI");
		CEGUI::Scheme::setDefaultResourceGroup("GUI");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("GUI");
		CEGUI::WindowManager::setDefaultResourceGroup("GUI");
		CEGUI::ScriptModule::setDefaultResourceGroup("GUI");

		CEGUI::SchemeManager::getSingleton().create("VanillaSkin.scheme");
		CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
		CEGUI::SchemeManager::getSingleton().create("InGame.scheme");

		// Set default font
		CEGUI::Font &font = CEGUI::FontManager::getSingleton().create("GameFont.font", "GUI");
		CEGUI::System::getSingleton().setDefaultFont(&font);

		// Set mouse cursor
		CEGUI::System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");
		CEGUI::MouseCursor::getSingleton().setImage(CEGUI::System::getSingleton().getDefaultMouseCursor());

		// Set initial cursor position		
		CEGUI::System::getSingleton().injectMousePosition((float)m_renderWnd->getWidth() / 2.0f + 1, (float)m_renderWnd->getHeight() / 2.0f);

		//
		// Sound
		//
		m_sound = new Sound();

		// FPS
		m_textRenderer->AddTextBox("txtFps", "Fps: ", 
			(Real)m_viewport->getActualLeft(), (Real)m_viewport->getActualTop(), 
			(Real)m_viewport->getActualWidth(), (Real)m_viewport->getActualHeight() + m_fpsOverlayHeight);

	}

	void BaseGame::Cleanup()
	{
		if (m_sound)
		{
			delete m_sound;
			m_sound = NULL;
		}

		if (m_renderer)
		{
			CEGUI::OgreRenderer::destroySystem();
			m_renderer = NULL;
		}

		if (m_ceguiLogger != NULL)
		{
			delete m_ceguiLogger;
			m_ceguiLogger = NULL;
		}

		CompositorManager *compMgr = CompositorManager::getSingletonPtr();
		if (compMgr && compMgr->hasCompositorChain(m_viewport))
		{
			compMgr->removeCompositorChain(m_viewport);
		}

		if (m_lensFlare)
		{
			delete m_lensFlare;
			m_lensFlare = NULL;
		}

		if (m_sprite)
		{
			m_sprite->Shutdown();
			delete m_sprite;
			m_sprite = NULL;
		}

		if (m_textRenderer)
		{
			m_textRenderer->RemoveTextBox("txtFps");
			delete m_textRenderer;
			m_textRenderer = NULL;
		}

		if (m_player)
		{
			delete m_player;
			m_player = NULL;
		}

		if (m_graphics)
		{
			delete m_graphics;
			m_graphics = NULL;
		}

		if (m_viewport)
		{
			m_renderWnd->removeViewport(m_viewport->getZOrder());
			m_viewport = NULL;
		}

		if (m_camera)
		{
			delete m_camera;
			m_camera = NULL;
		}

		if (m_light)
		{
			m_sceneMgr->destroyLight(m_light);
			m_light = NULL;
		}

		if (m_sceneMgr)
		{
			m_root->destroySceneManager(m_sceneMgr);
			m_sceneMgr = NULL;
		}

		Game::Cleanup();
	}

	void BaseGame::LoadContent()
	{
		ShowLoadingMessage();
	
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory(), "FileSystem", "General");

		// GUI
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/Shaders", "FileSystem", "General");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/configs", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/fonts", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/imagesets", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/layouts", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/looknfeel", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/lua_scripts", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/schemes", "FileSystem", "GUI");
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetContentDirectory() + "/GUI/xml_schemas", "FileSystem", "GUI");

		// Sound
		ResourceGroupManager::getSingleton().addResourceLocation(Directories::GetMusicsDirectory(), "FileSystem", "General");

		Game::LoadContent();

		HideLoadingMessage();
	}

	SceneManager& BaseGame::GetSceneManager()
	{
		assert(m_sceneMgr);
		return *m_sceneMgr;
	}

	const SceneManager& BaseGame::GetSceneManager() const
	{
		assert(m_sceneMgr);
		return *m_sceneMgr;
	}

	SpaceCamera& BaseGame::GetSpaceCamera()
	{
		assert(m_camera);
		return *m_camera;
	}

	const SpaceCamera& BaseGame::GetSpaceCamera() const
	{
		assert(m_camera);
		return *m_camera;
	}

	Camera& BaseGame::GetCamera()
	{
		assert(m_camera);
		return m_camera->GetCamera();
	}

	const Camera& BaseGame::GetCamera() const
	{
		assert(m_camera);
		return m_camera->GetCamera();
	}

	Sprite& BaseGame::GetSprite()
	{
		assert(m_sprite);
		return *m_sprite;
	}

	const Sprite& BaseGame::GetSprite() const
	{
		assert(m_sprite);
		return *m_sprite;
	}

	Vector2 BaseGame::Convert3DPointTo2D(const Vector3 &point) const
	{
		const Vector3 worldView = GetCamera().getViewMatrix() * point;

		// Homogeneous clip space, between -1, 1 is in frusttrum
		const Vector3 hcsPos = GetCamera().getProjectionMatrix() * worldView;

		const float halfWidth = (float)GetWidth() / 2;
		const float halfHeight = (float)GetHeight() / 2;
		return Vector2(halfWidth + halfWidth * hcsPos.x, 
			halfHeight + halfHeight * (-hcsPos.y));
	}

	bool BaseGame::IsInFrontOfCamera(const Vector3 &point) const
	{
		// Not work, why?
		//const Matrix4 viewProjMatrix = GetCamera().getViewMatrix() * GetCamera().getProjectionMatrix();
		//Vector4 result = Vector4(point.x, point.y, point.z, 1) * viewProjMatrix;
		//
		//// Is result in front?
		//return result.z > result.w - GetCamera().getNearClipDistance();

		const Vector3 eyeSpacePos = GetCamera().getViewMatrix() * point;
		if (eyeSpacePos.z >= 0)
			return false;

		const Vector3 hcsPos = GetCamera().getProjectionMatrix() * eyeSpacePos;
		if ((hcsPos.x < -1.0f) || (hcsPos.x > 1.0f) || (hcsPos.y < -1.0f) || (hcsPos.y > 1.0f))
			return false;

		return true;
	}

	void BaseGame::Update(double timeSinceLastFrame)
	{
		m_elapsedTimeThisFrameInMs = (float)timeSinceLastFrame;
		m_totalTimeMs += m_elapsedTimeThisFrameInMs;

		// Make sure m_elapsedTimeThisFrameInMs is never 0
		if (m_elapsedTimeThisFrameInMs <= 0)
			m_elapsedTimeThisFrameInMs = 0.001f;

		// Increase frame counter
		m_totalFrameCount++;

		// Show FPS
		if (m_showFps)
		{
			const RenderTarget::FrameStats &frameStats = m_renderWnd->getStatistics();
			assert(m_textRenderer);
			m_textRenderer->SetText("txtFps", "Fps:%f Tri:%u", frameStats.avgFPS, frameStats.triangleCount);
		}

		// Update sound engine
		m_sound->Update();

		// Make a screenshot
		if (m_keyboard->isKeyDown(OIS::KC_SYSRQ))
		{
			m_renderWnd->writeContentsToTimestampedFile("screenshot", ".png");
		}

		Game::Update(timeSinceLastFrame);
	}
	
	float BaseGame::GetMoveFactorPerSecond() const
	{
		return m_elapsedTimeThisFrameInMs / 1000.0f;
	}

	void BaseGame::EnableSkyBox(bool enable)
	{
		assert(m_sceneMgr);
		m_sceneMgr->setSkyBox(enable, 
			"RocketCommander/SpaceSkyBox", 
			GetCamera().getFarClipDistance() * 0.5f, 
			true);
	}

	void BaseGame::SetLightDirection(const Ogre::Vector3 &lightDir)
	{
		assert(m_light);
		m_light->setDirection(lightDir.normalisedCopy());
	}

	void BaseGame::SetLightColour(const Ogre::ColourValue &lightColour)
	{
		assert(m_light);
		m_light->setDiffuseColour(lightColour);
		m_light->setSpecularColour(lightColour);
	}

	Player& BaseGame::GetPlayer()
	{
		assert(m_player);
		return *m_player;
	}

	const Player& BaseGame::GetPlayer() const
	{
		assert(m_player);
		return *m_player;
	}

	void BaseGame::windowResized(Ogre::RenderWindow* rw)
	{
		assert(rw);
		if (m_camera)
			GetCamera().setAspectRatio(Real(m_viewport->getActualWidth()) / Real(m_viewport->getActualHeight()));	
		Game::windowResized(rw);
	}

	Sound& BaseGame::GetSound()
	{
		assert(m_sound);
		return *m_sound;
	}

	const Sound& BaseGame::GetSound() const
	{
		assert(m_sound);
		return *m_sound;
	}

	void BaseGame::ShowLoadingMessage()
	{
		assert(m_textRenderer);
		m_textRenderer->AddTextBox(m_loadingTxtName, "Start loading...", 
			(Real)m_viewport->getActualLeft() + m_viewport->getActualWidth() / 2.0f, 
			(Real)m_viewport->getActualTop() + m_viewport->getActualHeight() / 2.0f , 
			(Real)m_viewport->getActualWidth() / 2, (Real)m_viewport->getActualHeight() / 2.0f);

		ResourceGroupManager::getSingleton().addResourceGroupListener(this);
	}
	
	void BaseGame::HideLoadingMessage()
	{
		Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener(this);

		assert(m_textRenderer);
		m_textRenderer->RemoveTextBox(m_loadingTxtName);
	}

	void BaseGame::resourceGroupScriptingStarted(const String& groupName, size_t scriptCount)
	{
		assert(m_textRenderer);
		m_textRenderer->SetText(m_loadingTxtName, "Parsing...");
		m_renderWnd->update();
	}

	void BaseGame::scriptParseStarted(const String& scriptName, bool& skipThisScript)
	{
		assert(m_textRenderer);
		m_textRenderer->SetText(m_loadingTxtName, scriptName);
		m_renderWnd->update();
	}

	void BaseGame::scriptParseEnded(const String& scriptName, bool skipped)
	{
	}

	void BaseGame::resourceGroupScriptingEnded(const String& groupName)
	{
	}

	void BaseGame::resourceGroupLoadStarted(const String& groupName, size_t resourceCount)
	{
		assert(m_textRenderer);
		m_textRenderer->SetText(m_loadingTxtName, "Loading...");
		m_renderWnd->update();
	}
	
	void BaseGame::resourceLoadStarted(const ResourcePtr& resource)
	{
		assert(m_textRenderer);
		m_textRenderer->SetText(m_loadingTxtName, resource->getName());
		m_renderWnd->update();
	}
	
	void BaseGame::resourceLoadEnded()
	{
	}

	void BaseGame::worldGeometryStageStarted(const String& description)
	{
		assert(m_textRenderer);
		m_textRenderer->SetText(m_loadingTxtName, description);
		m_renderWnd->update();
	}

	void BaseGame::worldGeometryStageEnded()
	{
	}
		
	void BaseGame::resourceGroupLoadEnded(const String& groupName)
	{
	}
}