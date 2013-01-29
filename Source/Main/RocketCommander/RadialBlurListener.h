#pragma once

#include "OgreCompositorLogic.h"
#include "Player.h"

namespace RocketCommander
{
	class RadialBlurListener : public Ogre::CompositorInstance::Listener
	{
	public:
		RadialBlurListener(const Player &player);

		virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	    virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

	private:
		Ogre::GpuProgramParametersSharedPtr fpParams;
		const Player &m_player;
	};
}