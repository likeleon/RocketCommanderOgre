#include "stdafx.h"
#include "TestGameWithLight.h"
#include "TextRenderer.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(TestGameTest)

BOOST_AUTO_TEST_CASE(RunTest)
{
	TestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(InitUpdateTest)
{
	class InitUpdateTestGame : public TestGame
	{
	public:
		InitUpdateTestGame() : m_initTestFired(false), m_updateTestFired(false) {}

		bool m_initTestFired;
		bool m_updateTestFired;

	private:
		virtual void InitTest() { m_initTestFired = true; }
		virtual void UpdateTest(double timeSinceLastFrame) { m_updateTestFired = true; }
	};

	InitUpdateTestGame game;
	BOOST_CHECK_EQUAL(false, game.m_initTestFired);
	BOOST_CHECK_EQUAL(false, game.m_updateTestFired);
	
	game.Run();
	BOOST_CHECK_EQUAL(true, game.m_initTestFired);
	BOOST_CHECK_EQUAL(true, game.m_updateTestFired);
}

BOOST_AUTO_TEST_CASE(RenderMeshTest)
{
	class RenderMeshTestGame : public TestGameWithLight
	{
	private:
		virtual void InitTest()
		{
			TestGameWithLight::InitTest();
			
			m_meshes.push_back("rocket.mesh");
			m_meshes.push_back("asteroid1.mesh");
			m_meshes.push_back("asteroid2.mesh");
			m_meshes.push_back("asteroid3.mesh");
			m_meshes.push_back("asteroid4.mesh");
			m_meshes.push_back("asteroid5.mesh");
			//m_meshes.push_back("asteroidLow.mesh");

			BOOST_FOREACH(String mesh, m_meshes)
			{
				m_sceneMgr->createEntity(mesh, mesh);
			}

			m_meshNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
			m_nowMeshIndex = 0;
			m_meshNode->attachObject(m_sceneMgr->getEntity(m_meshes[m_nowMeshIndex]));

			GetCamera().setPosition(Vector3(0, 0, -280));
			GetCamera().lookAt(Vector3(0, 0, 0));
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			TextRenderer::getSingleton().SetText("txtDebug", m_meshes[m_nowMeshIndex]);
			TestGameWithLight::UpdateTest(timeSinceLastFrame);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key >= OIS::KC_1 && evt.key < OIS::KC_1 + (int)m_meshes.size())
			{
				m_meshNode->detachAllObjects();
				m_nowMeshIndex = evt.key - OIS::KC_1;
				m_meshNode->attachObject(m_sceneMgr->getEntity(m_meshes[m_nowMeshIndex]));
			}
			return TestGameWithLight::keyPressed(evt);
		}

		StringVector m_meshes;
		Entity *m_asteroid;
		Entity *m_rocket;
		SceneNode *m_meshNode;
		int m_nowMeshIndex;
	};

	RenderMeshTestGame game;
	game.Run();
}


BOOST_AUTO_TEST_CASE(SkyBoxTest)
{
	class SkyBoxTestGame : public TestGame
	{
		virtual void InitTest()
		{
			EnableSkyBox();
		}
	};
	SkyBoxTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()