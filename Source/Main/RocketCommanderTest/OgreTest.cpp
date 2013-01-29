#include "stdafx.h"
#include "Ogre.h"

BOOST_AUTO_TEST_SUITE(OgreTest)

BOOST_AUTO_TEST_CASE(SimpleRunTest)
{
	Ogre::Root *ogre = new Ogre::Root("", "");

	try
	{
		// Load render system
		ogre->loadPlugin("RenderSystem_Direct3D9_d");

		const Ogre::RenderSystemList &renderSystem = ogre->getAvailableRenderers();
		ogre->setRenderSystem(*(renderSystem.begin()));
		ogre->initialise(false);

		// Load common plugins
		ogre->loadPlugin("Plugin_OctreeSceneManager_d");

		// Setup main window
		Ogre::NameValuePairList opts;
		opts["resolution"] = "1024x768";
		opts["fullscreen"] = "false";
		opts["vsync"] = "false";

		// Create a rendering window
		Ogre::RenderWindow *window = ogre->createRenderWindow("OgreTest", 1024, 768, false, &opts);

		// Use the ST_GENERIC scene manager
		Ogre::SceneManager *sceneMgr = ogre->createSceneManager(Ogre::ST_GENERIC);
		Ogre::Camera *camera = sceneMgr->createCamera("camera");
		camera->setNearClipDistance(5);
		Ogre::Viewport *vp = window->addViewport(camera);
		vp->setBackgroundColour(Ogre::ColourValue(0, 0, 1));

		// For now, we'll just set it to 4:3 the easy way
		camera->setAspectRatio((Ogre::Real)1.333333);

		while (1)
		{
			ogre->renderOneFrame();
		}
	}
	catch (Ogre::Exception &e)
	{
		std::cerr << e.getFullDescription() << std::endl;
	}

	delete ogre;
}

BOOST_AUTO_TEST_SUITE_END()