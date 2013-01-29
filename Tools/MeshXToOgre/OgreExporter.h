#pragma once

namespace MeshXToOgre
{
	class OgreExporter
	{
	public:
		static bool ExportMesh(const std::string &name, const Ogre::String &outFile, const LPD3DXMESH d3dxMesh,
			bool generateEdgeLists, bool generateTangents, Ogre::VertexElementSemantic tangentSemantic);

	private:
		static void Cleanup(Ogre::MeshPtr mesh);
	};
}