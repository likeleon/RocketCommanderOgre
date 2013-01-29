#pragma once

namespace RocketCommander
{
	class BaseGame;

	class SpaceCamera
	{
	public:
		enum CameraModes
		{
			CameraMode_MenuMode,	// default mode for menu, just rotating around.
			CameraMode_InGame,		// in game using the rocket controls (always flying forwards).
			CameraMode_FreeCamera,	// Free camera mode, allows to freely rotate and move around.
		};

		SpaceCamera(BaseGame &game, const Ogre::Vector3 &cameraPos);
		~SpaceCamera();

		void Update(double timeSinceLastFrame);

		// Set methods
		void SetInGame(bool inGame);

		// Get methods
		Ogre::Camera& GetCamera();
		const Ogre::Camera& GetCamera() const;
		bool IsInGame() const;

	private:
		enum RotationAxis
		{
			RotationAxis_Pitch,
			RotationAxis_Yaw,
		};

		enum MoveDirections
		{
			MoveDirection_X,
			MoveDirection_Y,
			MoveDirection_Z,
		};

		void HandleFreeCamera() {}
		void HandlePlayerInput();
		void RandomlyRotateAround();
		void Rotate(RotationAxis axis, float angle);
		void Translate(float amount, MoveDirections direction);
		void UpdateCamera();

		Ogre::Camera *m_camera;
		CameraModes m_cameraMode;
		BaseGame &m_game;
		float m_yawRotation;
		float m_pitchRotation;
		Ogre::Vector3 m_pos;
		float m_mouseSensibility;
	};
}