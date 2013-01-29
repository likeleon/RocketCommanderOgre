#define WIN32_LEAN_AND_MEAN

#include "windows.h"

#include <Ogre.h>
#include <OIS.h>
#include <CEGUI.h>
#include <RendererModules/Ogre/CEGUIOgreRenderer.h>

#include "RocketCommanderGame.h"

INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	try
	{
		RocketCommander::RocketCommanderGame game;
		game.Run();
	}
	catch (Ogre::Exception &e)
	{
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_ICONERROR | MB_TASKMODAL);
	}
	return 0;
}