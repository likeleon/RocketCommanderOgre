#pragma once

#include "Game.h"

namespace RocketCommander
{
	class Sprite;
	class LensFlare;
	class TextRenderer;
	class SpaceCamera;
	class Player;
	class Sound;

	class BaseGame : public Game, public Ogre::ResourceGroupListener
	{
	public:
		BaseGame();
		virtual ~BaseGame();

		// Get methods
		Ogre::SceneManager& GetSceneManager();
		const Ogre::SceneManager& GetSceneManager() const;
		SpaceCamera& GetSpaceCamera();
		const SpaceCamera& GetSpaceCamera() const;
		Ogre::Camera& GetCamera();
		const Ogre::Camera& GetCamera() const;
		Sprite& GetSprite();
		const Sprite& GetSprite() const;
		float GetMoveFactorPerSecond() const;
		float GetTotalTimeMs() const { return m_totalTimeMs; }
		Player& GetPlayer();
		const Player& GetPlayer() const;
		float GetElapsedTimeThisFrameInMs() const { return m_elapsedTimeThisFrameInMs; }
		int   GetTotalFrames() const { return m_totalFrameCount; }
		Sound& GetSound();
		const Sound& GetSound() const;

		// Set methods
		void SetLensFlareColour(const Ogre::ColourValue &colour) { m_remLensFlareColor = colour; }
		void SetLightDirection(const Ogre::Vector3 &lightDir);
		void SetLightColour(const Ogre::ColourValue &lightColour);

		// Viewport size
		int GetWidth() const { return m_viewport->getActualWidth(); }
		int GetHeight() const { return m_viewport->getActualHeight(); }

		Ogre::Vector2 Convert3DPointTo2D(const Ogre::Vector3 &point) const;
		bool IsInFrontOfCamera(const Ogre::Vector3 &point) const;

		static const Ogre::Radian FieldOfView;
		static const Ogre::Real NearPlane;
		static const Ogre::Real FarPlane;

	protected:
		// Overrides Game
		virtual void Initialize();
		virtual void LoadContent();
		virtual void Update(double timeSinceLastFrame);
		virtual void Cleanup();
		virtual void windowResized(Ogre::RenderWindow* rw);

		void EnableSkyBox(bool enable = true);

		static const float m_fpsOverlayHeight;

		GraphicsDeviceManager	*m_graphics;
		Ogre::SceneManager		*m_sceneMgr;
		Ogre::Viewport			*m_viewport;
		Sprite					*m_sprite;
		LensFlare				*m_lensFlare;
		Ogre::ColourValue		m_remLensFlareColor;
		bool					m_showFps;
		Ogre::Light				*m_light;
		Sound					*m_sound;

	private:
		void ShowLoadingMessage();
		void HideLoadingMessage();

		// Implements Ogre::ResourceGroupListener
		void resourceGroupScriptingStarted(const Ogre::String& groupName, size_t scriptCount);
		void scriptParseStarted(const Ogre::String& scriptName, bool& skipThisScript);
		void scriptParseEnded(const Ogre::String& scriptName, bool skipped);
		void resourceGroupScriptingEnded(const Ogre::String& groupName);
		void resourceGroupLoadStarted(const Ogre::String& groupName, size_t resourceCount);
		void resourceLoadStarted(const Ogre::ResourcePtr& resource);
		void resourceLoadEnded();
		void worldGeometryStageStarted(const Ogre::String& description);
		void worldGeometryStageEnded();
		void resourceGroupLoadEnded(const Ogre::String& groupName);

		const Ogre::ColourValue m_backgroundColour;
		CEGUI::OgreRenderer	*m_renderer;
		TextRenderer		*m_textRenderer;
		SpaceCamera			*m_camera;
		float				m_elapsedTimeThisFrameInMs;
		float				m_totalTimeMs;
		Player				*m_player;
		int					m_totalFrameCount;
		const Ogre::String  m_loadingTxtName;
		CEGUI::Logger		*m_ceguiLogger;
	};
}