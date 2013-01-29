#pragma once

#include "GameScreen.h"

namespace RocketCommander
{
	class CreditsScreen : public GameScreen
	{
	public:
		CreditsScreen(RocketCommanderGame &game);

		// Overrides GameScreen
		virtual Ogre::String GetName() const { return "Credits"; }
		virtual void Enter();
	};
}