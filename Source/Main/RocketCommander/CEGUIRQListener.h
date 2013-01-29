#pragma once

#include <RendererModules/Ogre/CEGUIOgreRenderTarget.h>

namespace RocketCommander
{
	// RenderQueueListener based class used to hook into the ogre rendering system
	class CEGUIRQListener : public Ogre::RenderQueueListener
	{
	public:
		CEGUIRQListener(CEGUI::OgreRenderer *renderer, Ogre::uint8 queueId, bool postQueue)
			: m_renderer(renderer),
			m_queueId(queueId),
			m_postQueue(postQueue)
		{
		}

		virtual void renderQueueStarted(Ogre::uint8 id, const Ogre::String &invocation, bool &skipThisQueue)
		{
			if (!m_postQueue && m_queueId == id)
				CEGUI::System::getSingleton().renderGUI();
		}

		virtual void renderQueueEnded(Ogre::uint8 id, const Ogre::String &invocation, bool &repeatThisQueue)
		{
			if (!m_postQueue || m_queueId != id)
				return;

			// Need to reset ogre renderer viewport here for whichever vp is being drawn.
			Ogre::Viewport *vp = Ogre::Root::getSingleton().getRenderSystem()->_getViewport();

			int left, top, width, height;
			vp->getActualDimensions(left, top, width, height);

			const CEGUI::Rect vpArea((float)left, (float)top, (float)(left + width), (float)(top + height));
			dynamic_cast<CEGUI::OgreRenderTarget&>(m_renderer->getDefaultRenderingRoot().getRenderTarget()).setArea(vpArea);

			CEGUI::System::getSingleton().renderGUI();
		}

	private:
		CEGUI::OgreRenderer *m_renderer;
		Ogre::uint8			m_queueId;
		bool				m_postQueue;
	};
}