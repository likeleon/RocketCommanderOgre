#pragma once

#include "Model.h"

namespace RocketCommander
{
	class Rocket : public Model
	{
	public:
		Rocket(Ogre::SceneManager &sceneMgr)
		 : Model(sceneMgr, "rocket", 0.00753780361f)
		{
		}
	};
}