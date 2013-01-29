// MeshXToOgre.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DxMesh.h"
#include "OgreExporter.h"

using namespace MeshXToOgre;

static std::string stringw2a(const std::wstring &w)
{
	std::string a;
	a.assign(w.begin(), w.end());
	return a;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		std::wcerr << _T("usage: ") << argv[0] << _T(" DxMesh OgreMesh") << std::endl;
		return -1;
	}

	const std::wstring mesh = argv[1];

	Ogre::LogManager& logMgr = Ogre::LogManager::getSingleton();
	logMgr.createLog("MeshXToOgre.log", true);

	DxMesh dxMesh(mesh);
	const LPD3DXMESH d3dxMesh = dxMesh.GetMesh();
	if (!d3dxMesh)
	{
		logMgr.logMessage("Failed to get .x mesh information.");
		return -1;
	}

	std::string name = boost::filesystem::path(stringw2a(mesh)).replace_extension("").filename();
	std::string outFile = stringw2a(argv[2]);

	if (!OgreExporter::ExportMesh(name, outFile.c_str(), d3dxMesh, true, true, Ogre::VES_TANGENT))
	{
		logMgr.logMessage("Failed to export mesh.");
		return -1;
	}

	return 0;
}
