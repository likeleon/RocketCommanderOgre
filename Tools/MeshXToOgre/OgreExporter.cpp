#include "stdafx.h"
#include "OgreExporter.h"
#include "DxVertex.h"
#include "OgreDefaultHardwareBufferManager.h"

Ogre::LogManager logMgr;
Ogre::ResourceGroupManager resGrpMgr;
Ogre::MeshManager meshMgr;
Ogre::LodStrategyManager lodManager;
Ogre::DefaultHardwareBufferManager defHWBufMgr;

namespace MeshXToOgre
{
	bool OgreExporter::ExportMesh(const std::string &name, const Ogre::String &outFile, const LPD3DXMESH d3dxMesh, 
		bool generateEdgeLists, bool generateTangents, Ogre::VertexElementSemantic tangentSemantic)
	{
		Ogre::LogManager& logMgr = Ogre::LogManager::getSingleton();
		Ogre::MeshPtr ogreMesh;

		Ogre::Vector3 min, max, currpos;
		Ogre::Real maxSquaredRadius;
		bool first = true;

		try
		{
			logMgr.logMessage("Creating mesh object...");

			ogreMesh = Ogre::MeshManager::getSingleton().create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			logMgr.logMessage("Mesh object created.");

			logMgr.logMessage("Creating submesh object...");
			Ogre::SubMesh *ogreSubMesh = ogreMesh->createSubMesh();
			logMgr.logMessage("Submesh object created.");

			ogreSubMesh->setMaterialName(name);
			logMgr.logMessage("Submesh material done.");

			logMgr.logMessage("Setting up geometry...");

			// Set up mesh geometry
			ogreSubMesh->vertexData = new Ogre::VertexData();
			ogreSubMesh->vertexData->vertexCount = d3dxMesh->GetNumVertices();
			ogreSubMesh->vertexData->vertexStart = 0;
			Ogre::VertexBufferBinding *bind = ogreSubMesh->vertexData->vertexBufferBinding;
			Ogre::VertexDeclaration *decl = ogreSubMesh->vertexData->vertexDeclaration;
			// Always 1 texture layer, 2D coords
			enum {POSITION_BINDING = 0, NORMAL_BINDING = 1, TEXCOORD_BINDING = 2};
			decl->addElement(POSITION_BINDING, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
			decl->addElement(NORMAL_BINDING, 0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
			decl->addElement(TEXCOORD_BINDING, 0, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
			// Create buffers
			Ogre::HardwareVertexBufferSharedPtr pbuf = Ogre::HardwareBufferManager::getSingleton().
            createVertexBuffer(decl->getVertexSize(POSITION_BINDING), ogreSubMesh->vertexData->vertexCount,
                Ogre::HardwareBuffer::HBU_DYNAMIC, false);
			Ogre::HardwareVertexBufferSharedPtr nbuf = Ogre::HardwareBufferManager::getSingleton().
				createVertexBuffer(decl->getVertexSize(NORMAL_BINDING), ogreSubMesh->vertexData->vertexCount,
					Ogre::HardwareBuffer::HBU_DYNAMIC, false);
			Ogre::HardwareVertexBufferSharedPtr tbuf = Ogre::HardwareBufferManager::getSingleton().
				createVertexBuffer(decl->getVertexSize(TEXCOORD_BINDING), ogreSubMesh->vertexData->vertexCount,
					Ogre::HardwareBuffer::HBU_DYNAMIC, false);
			bind->setBinding(POSITION_BINDING, pbuf);
			bind->setBinding(NORMAL_BINDING, nbuf);
			bind->setBinding(TEXCOORD_BINDING, tbuf);

			ogreSubMesh->useSharedVertices = false;

			logMgr.logMessage("Doing positions and texture coords...");

			float *pPos = static_cast<float *>(pbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			DxVertex *vertices = NULL;
			if (FAILED(d3dxMesh->LockVertexBuffer(D3DLOCK_READONLY, (void **)&vertices)))
				throw std::exception("Failed to get .x vertices");
			for (int v = 0; v < ogreSubMesh->vertexData->vertexCount; ++v)
			{
				//logMgr.logMessage("Doing vertex " + Ogre::StringConverter::toString(v));
				const DxVertex &vertex = vertices[v];
				*pPos++ = vertex.position.x;
				*pPos++ = vertex.position.y;
				*pPos++ = vertex.position.z;

				// Deal with bounds
				currpos = Ogre::Vector3(vertex.position.x, vertex.position.y, vertex.position.z);
				if (first)
				{
					min = max = currpos;
					maxSquaredRadius = currpos.squaredLength();
					first = false;
				}
				else
				{
					min.makeFloor(currpos);
					max.makeCeil(currpos);
					maxSquaredRadius = std::max(maxSquaredRadius, currpos.squaredLength());
				}
			}
			pbuf->unlock();

			float *pTex = static_cast<float *>(tbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			logMgr.logMessage("Doing uvs, normals and indexes (v2)...");
			float *pNorm = static_cast<float *>(nbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			ogreSubMesh->indexData->indexCount = d3dxMesh->GetNumFaces() * 3;
			// Always use 16-bit buffers
			Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
				createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, ogreSubMesh->indexData->indexCount, Ogre::HardwareIndexBuffer::HBU_STATIC_WRITE_ONLY);
			ogreSubMesh->indexData->indexBuffer = ibuf;
			unsigned short *pIdx = static_cast<unsigned short *>(ibuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
			// Lock the index buffer of the dx mesh
			short *indices = NULL;
			if (FAILED(d3dxMesh->LockIndexBuffer(D3DLOCK_READONLY, (void **)&indices)))
				throw std::exception("Failed to get .x indices");
			for (int i = 0; i < ogreSubMesh->indexData->indexCount; i += 3)
			{
				for (int j = 0; j < 3; ++j)
				{
					int vertIdx = indices[i + j];

					// Face index
					pIdx[i + j] = vertIdx;

					// Vertex normals
					const DxVertex &vertex = vertices[vertIdx];
					pTex[(vertIdx * 2)] = vertex.tex.x;
					pTex[(vertIdx * 2) + 1] = vertex.tex.y;
					pNorm[(vertIdx * 3)] = vertex.normal.x;
					pNorm[(vertIdx * 3) + 1] = vertex.normal.y;
					pNorm[(vertIdx * 3) + 2] = vertex.normal.z;
				}
			}

			nbuf->unlock();
			ibuf->unlock();
			tbuf->unlock();

			d3dxMesh->UnlockIndexBuffer();
			d3dxMesh->UnlockVertexBuffer();

			// TODO: Reorganise the vertex buffers?
			//// Now use Ogre's ability to reorganise the vertex buffers the best way
			//Ogre::VertexDeclaration* newDecl = 
			//	ogreSubMesh->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(
			//		false, false);
			//Ogre::BufferUsageList bufferUsages;
			//for (size_t u = 0; u <= newDecl->getMaxSource(); ++u)
			//	bufferUsages.push_back(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			//ogreSubMesh->vertexData->reorganiseBuffers(newDecl, bufferUsages);

			logMgr.logMessage("Geometry done.");

			// Set bounds
			ogreMesh->_setBoundingSphereRadius(Ogre::Math::Sqrt(maxSquaredRadius));
			ogreMesh->_setBounds(Ogre::AxisAlignedBox(min, max), false);

			// Export
			logMgr.logMessage("Creating MeshSerializer...");
			Ogre::MeshSerializer serializer;
			logMgr.logMessage("MeshSerializer created.");

			// Generate LODs?

			if (generateEdgeLists)
			{
				ogreMesh->buildEdgeList();
			}

			if (generateTangents)
			{
				unsigned short src, dst;
				ogreMesh->suggestTangentVectorBuildParams(tangentSemantic, src, dst);
				ogreMesh->buildTangentVectors(tangentSemantic, src, dst);
			}

			// Export
			Ogre::String msg;
			msg = "Exporting mesh data to file '" + outFile + "'";
			logMgr.logMessage(msg);
			serializer.exportMesh(ogreMesh.getPointer(), outFile);
			logMgr.logMessage("Export successful");
		}
		catch (...)
		{
			logMgr.logMessage("Failed to export to ogre mesh");

			Cleanup(ogreMesh);
			return false;
		}

		Cleanup(ogreMesh);
		return true;
	}

	void OgreExporter::Cleanup(Ogre::MeshPtr ogreMesh)
	{
		Ogre::MeshManager::getSingleton().remove(ogreMesh->getHandle());
	}
}