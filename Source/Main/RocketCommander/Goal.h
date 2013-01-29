#pragma once

#include "Model.h"

namespace RocketCommander
{
	class Goal : public Model
	{
	public:
		Goal(Ogre::SceneManager &sceneMgr, const Ogre::Vector3 &position);

	private:
		static const float DownScaling;
	};
};