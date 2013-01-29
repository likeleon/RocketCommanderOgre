#include "stdafx.h"
#include "Line2D.h"

using namespace Ogre;

namespace RocketCommander
{
	int Line2D::instanceId = 0;

	Line2D::Line2D(Ogre::SceneManager &sceneMgr, const Vector2 &startPoint, const Vector2 &endPoint, const ColourValue &colour)
	: m_sceneMgr(sceneMgr)
	{
		m_manObj = m_sceneMgr.createManualObject();
		m_manObjNode = m_sceneMgr.getRootSceneNode()->createChildSceneNode();

		const String materialName = "line2d_material" + StringConverter::toString(instanceId);
		m_manObjMaterial = MaterialManager::getSingleton().create(materialName,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		m_manObjMaterial->setReceiveShadows(false);
		m_manObjMaterial->getTechnique(0)->setLightingEnabled(true);
		SetColour(colour);

		m_manObj->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
		m_manObj->setUseIdentityProjection(true);
		m_manObj->setUseIdentityView(true);
		m_manObj->setQueryFlags(0);
		AxisAlignedBox box;
		box.setInfinite();
		m_manObj->setBoundingBox(box);

		m_manObj->begin(materialName, Ogre::RenderOperation::OT_LINE_LIST); 
		m_manObj->position(Vector3(startPoint.x, startPoint.y, 0.0f)); 
		m_manObj->position(Vector3(endPoint.x, endPoint.y, 0.0f)); 
		m_manObj->end(); 

		m_manObjNode->attachObject(m_manObj);

		instanceId++;
	}

	Line2D::~Line2D()
	{
		m_manObjNode->getParentSceneNode()->removeChild(m_manObjNode);
		m_sceneMgr.destroyManualObject(m_manObj);
		MaterialManager::getSingleton().remove(m_manObjMaterial->getHandle());
		m_manObjMaterial.setNull();
	}

	void Line2D::SetColour(const Ogre::ColourValue &colour)
	{
		assert(!m_manObjMaterial.isNull());
		m_manObjMaterial->getTechnique(0)->getPass(0)->setDiffuse(colour); 
		m_manObjMaterial->getTechnique(0)->getPass(0)->setAmbient(colour); 
		m_manObjMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(colour); 
	}
}