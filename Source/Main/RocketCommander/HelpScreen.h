#pragma once

#include "GameScreen.h"

namespace RocketCommander
{
	class HelpScreen : public GameScreen
	{
	public:
		HelpScreen(RocketCommanderGame &game);

		// Overrides GameScreen
		virtual Ogre::String GetName() const { return "Help"; }
		virtual void Enter();
	};
}