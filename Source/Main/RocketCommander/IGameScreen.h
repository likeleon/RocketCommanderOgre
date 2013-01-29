#pragma once

namespace RocketCommander
{
	class RocketCommanderGame;

	class IGameScreen : public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		virtual ~IGameScreen() {};

		// Name of this game screen
		virtual Ogre::String GetName() const = 0;

		// Returns true if we want to quit this screen and return to the previous screen.
		// If no more screens are left the game is exited.
		virtual bool GetQuit() const = 0;

		// Enter game screen.
		virtual void Enter() = 0;

		// Exit game screen.
		virtual void Exit() = 0;

		// Run game screen. Called each frame.
		virtual void Run() = 0;

		virtual bool IsInGame() const = 0;
	};
}