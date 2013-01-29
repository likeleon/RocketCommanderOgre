#include "stdafx.h"
#include "Goal.h"

using namespace Ogre;

namespace RocketCommander
{
	const float Goal::DownScaling = 0.00358866737f;

	Goal::Goal(SceneManager &sceneMgr, const Ogre::Vector3 &position)
	: Model(sceneMgr, "Goal", DownScaling)
	{
		SetPosition(position);
		SetOrientation(Quaternion(-Radian(Math::PI / 2.0f), Vector3::UNIT_X));
	}
}