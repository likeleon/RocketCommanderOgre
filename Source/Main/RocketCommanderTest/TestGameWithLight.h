#pragma once

#include "TestGame.h"

class TestGameWithLight : public RocketCommander::TestGame
{
public:
	TestGameWithLight()
	: m_moveLight(true), m_lightPivot(NULL)
	{
	}

	virtual void InitTest()
	{
		m_sceneMgr->destroyLight(m_light);
		m_light = NULL;

		// Create white light with pivot node and billboard
		Ogre::Light* light = m_sceneMgr->createLight();
		light->setPosition(0, 0, -150);
		light->setDiffuseColour(1, 1, 1);
		light->setSpecularColour(1, 1, 1);

		Ogre::BillboardSet *bbs = m_sceneMgr->createBillboardSet();
		bbs->setMaterialName("RocketCommander/Flare");

		m_lightPivot = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
		bbs->createBillboard(light->getPosition())->setColour(Ogre::ColourValue::White);
		m_lightPivot->attachObject(light);
		m_lightPivot->attachObject(bbs);
	}

	virtual void UpdateTest(double timeSinceLastFrame)
	{
		if (m_moveLight)
			m_lightPivot->yaw(Ogre::Degree((Ogre::Real)timeSinceLastFrame / 30));
		TestGame::UpdateTest(timeSinceLastFrame);
	}

	virtual bool keyPressed(const OIS::KeyEvent &evt)
	{
		if (evt.key == OIS::KC_M)
			m_moveLight = !m_moveLight;
		return TestGame::keyPressed(evt);
	}

protected:
	bool m_moveLight;
	Ogre::SceneNode *m_lightPivot;
};