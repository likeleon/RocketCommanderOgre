#include "stdafx.h"
#include "TestGameWithLight.h"
#include "GameAsteroidManager.h"
#include "LensFlare.h"

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
			m_meshes.push_back("Fuel.mesh");
			m_meshes.push_back("Health.mesh");
			m_meshes.push_back("Bomb.mesh");
			m_meshes.push_back("Speed.mesh");
			m_meshes.push_back("ExtraLife.mesh");
			m_meshes.push_back("asteroid1Low.mesh");
			m_meshes.push_back("asteroid2Low.mesh");
			m_meshes.push_back("asteroid3Low.mesh");

			BOOST_FOREACH(String mesh, m_meshes)
			{
				m_sceneMgr->createEntity(mesh, mesh);
			}

			m_meshNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();
			m_nowMeshIndex = 0;
			SetMesh(m_meshes[m_nowMeshIndex]);

			GetCamera().setPosition(Vector3(0, 0, -280));
			GetCamera().lookAt(Vector3(0, 0, 0));
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1)
				m_nowMeshIndex++;
			else if (evt.key == OIS::KC_2)
				m_nowMeshIndex--;
			m_nowMeshIndex = std::max(m_nowMeshIndex, 0);
			m_nowMeshIndex = std::min(m_nowMeshIndex, (int)m_meshes.size() - 1);
			
			SetMesh(m_meshes[m_nowMeshIndex]);
			
			return TestGameWithLight::keyPressed(evt);
		}

		void SetMesh(const String &meshName)
		{
			m_meshNode->detachAllObjects();
			m_meshNode->attachObject(m_sceneMgr->getEntity(meshName));

			m_leftTopDebug->setText(meshName);
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

BOOST_AUTO_TEST_CASE(ViewAngleTest)
{
	class ViewAngleTestGame : public TestGame
	{
		virtual void InitTest()
		{
			ShowCameraInfo();
			m_targetPosition = Vector3(0, 0, -5000);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{			
			Vector3 leftRotation = GetCamera().getRight() * -Vector3::UNIT_SCALE;
			const float leftAngle = leftRotation.angleBetween(m_targetPosition).valueDegrees();
			
			Vector3 rightRotation = GetCamera().getRight();
			const float rightAngle = rightRotation.angleBetween(m_targetPosition).valueDegrees();
			
			Vector3 upRotation = GetCamera().getUp();
			const float upAngle = upRotation.angleBetween(m_targetPosition).valueDegrees();

			Vector3 downRotation = GetCamera().getUp() * -Vector3::UNIT_SCALE;
			const float downAngle = downRotation.angleBetween(m_targetPosition).valueDegrees();

			const String rotateInfo = "Rotates\nleft  :" + 
				StringConverter::toString(leftRotation) +
				"\nright: " + StringConverter::toString(rightRotation) +
				"\nup   : " + StringConverter::toString(upRotation) +
				"\ndown : " + StringConverter::toString(downRotation);
			m_rightTopDebug->setText(rotateInfo);

			const String angleInfo = "Angles\nleft  :" + 
				StringConverter::toString(leftAngle) +
				"\nright: " + StringConverter::toString(rightAngle) +
				"\nup   : " + StringConverter::toString(upAngle) +
				"\ndown : " + StringConverter::toString(downAngle);
			m_rightBottomDebug->setText(angleInfo);
		}

		Vector3 m_targetPosition;
	};

	ViewAngleTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(CompositorTest)
{
	class CompositorTestGame : public TestGame
	{
		virtual void InitTest()
		{
			EnableSkyBox();
			m_asteroidManager.reset(new GameAsteroidManager(*this, Level::LoadAllLevels()[0]));
			m_asteroidManager->SetLevel(m_asteroidManager->GetLevel());

			m_enableBloom = true;
			CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", m_enableBloom);

			m_enableRadialBlur = true;
			CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Radial Blur", m_enableRadialBlur);
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			m_asteroidManager->Update();
			m_lensFlare->Render(m_remLensFlareColor);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1)
			{
				m_enableBloom = !m_enableBloom;
				CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Bloom", m_enableBloom);
			}
			else if (evt.key == OIS::KC_2)
			{
				m_enableRadialBlur = !m_enableRadialBlur;
				CompositorManager::getSingleton().setCompositorEnabled(m_viewport, "Radial Blur", m_enableRadialBlur);
			}
			return TestGame::keyPressed(evt);
		}

		boost::shared_ptr<GameAsteroidManager> m_asteroidManager;
		bool m_enableBloom;
		bool m_enableRadialBlur;
	};
	CompositorTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_CASE(VideoOptionTest)
{
	class VideoOptionTestGame : public TestGame
	{
		virtual void InitTest()
		{
			// Help information
			m_leftBottomDebug->setText("0: Next renderer\n"
				"1: Next video mode\n"
				"2: Prev video mode\n"
				"3: Next FSAA\n"
				"4: Toggle full screen\n"
				"5: Next rendering device\n"
				"6: Toggle VSync\n");

			std::string currentSettingInfo = "Renderer: " + m_root->getRenderSystem()->getName() + "\n";

			// Get renderer config options
			ConfigOptionMap &options = m_root->getRenderSystem()->getConfigOptions();

			for (ConfigOptionMap::const_iterator iter = options.begin(); iter != options.end(); ++iter)
			{
				const ConfigOption &option = iter->second;

				if (option.name == "Video Mode")
				{
					// Video mode
					m_nowVideoMode = std::distance(option.possibleValues.begin(),
						find(option.possibleValues.begin(), option.possibleValues.end(), option.currentValue));
				}
				else if (option.name == "FSAA")
				{
					// FSAA
					m_nowFsaa = std::distance(option.possibleValues.begin(),
						find(option.possibleValues.begin(), option.possibleValues.end(), option.currentValue));
				}
				else if (option.name == "Full Screen")
				{
					// Full screen
					m_nowFullScreen = std::distance(option.possibleValues.begin(),
						find(option.possibleValues.begin(), option.possibleValues.end(), option.currentValue));
				}
				else if (option.name == "Rendering Device")
				{
					// Rendering device
					m_nowRenderingDevice = std::distance(option.possibleValues.begin(),
						find(option.possibleValues.begin(), option.possibleValues.end(), option.currentValue));
				}
				else if (option.name == "VSync")
				{
					// VSync
					m_nowVSync = std::distance(option.possibleValues.begin(),
						find(option.possibleValues.begin(), option.possibleValues.end(), option.currentValue));
				}

				currentSettingInfo += option.name + ": " + option.currentValue + "\n";
			}

			m_leftTopDebug->setText(currentSettingInfo);
		}

		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			ConfigOptionMap &options = m_root->getRenderSystem()->getConfigOptions();
			String configName = "";
			String configValue = "";

			if (evt.key == OIS::KC_0)
			{
				const RenderSystemList &renderers = m_root->getAvailableRenderers();
				const RenderSystemList::const_iterator nowRenderer = std::find(renderers.begin(), renderers.end(), m_root->getRenderSystem());
				assert(nowRenderer != renderers.end());

				RenderSystemList::const_iterator nextRenderer = nowRenderer + 1;

				if (nextRenderer == renderers.end())
					nextRenderer = renderers.begin();
				Ogre::NameValuePairList emptyOptions;
				Reconfigure((*nextRenderer)->getName(), emptyOptions);
				return true;
			}

			for (ConfigOptionMap::const_iterator iter = options.begin(); iter != options.end(); ++iter)
			{
				const ConfigOption &option = iter->second;
				const StringVector &possibleValues = option.possibleValues;

				if ((evt.key == OIS::KC_1 || evt.key == OIS::KC_2) && option.name == "Video Mode")
				{
					// Next/prev video mode
					if (evt.key == OIS::KC_1)
						m_nowVideoMode = std::min(possibleValues.size() - 1, m_nowVideoMode + 1);
					else if (evt.key == OIS::KC_2)
						m_nowVideoMode = std::max(0u, m_nowVideoMode - 1);
					
					configValue = possibleValues[m_nowVideoMode];
				}
				else if (evt.key == OIS::KC_3 && option.name == "FSAA")
				{
					// Next FSAA
					m_nowFsaa = (m_nowFsaa + 1) % possibleValues.size();
					configValue = possibleValues[m_nowFsaa];
				}
				else if (evt.key == OIS::KC_4 && option.name == "Full Screen")
				{
					// Toggle full screen
					m_nowFullScreen = (m_nowFullScreen + 1) % possibleValues.size();
					configValue = possibleValues[m_nowFullScreen];

					/*Ogre::StringVector vmopts = Ogre::StringUtil::split(options["Video Mode"].currentValue, " x");
					unsigned int w = Ogre::StringConverter::parseUnsignedInt(vmopts[0]);
					unsigned int h = Ogre::StringConverter::parseUnsignedInt(vmopts[1]);
					m_renderWnd->setFullscreen(!m_renderWnd->isFullScreen(), w, h);*/
				}
				else if (evt.key == OIS::KC_5 && option.name == "Rendering Device")
				{
					// Next rendering device
					m_nowRenderingDevice = (m_nowRenderingDevice + 1) % possibleValues.size();
					configValue = possibleValues[m_nowRenderingDevice];
				}
				else if (evt.key == OIS::KC_6 && option.name == "VSync")
				{
					// Toggle vsync
					m_nowVSync = (m_nowVSync + 1) % possibleValues.size();
					configValue = possibleValues[m_nowVSync];
				}

				if (!configValue.empty())
				{
					configName = option.name;
					break;
				}
			}

			if (!configName.empty() && !configValue.empty())
			{
				NameValuePairList newOptions;
				newOptions[configName] = configValue;
				Reconfigure(m_root->getRenderSystem()->getName(), newOptions);
			}

			return TestGame::keyPressed(evt);
		}

		unsigned int m_nowVideoMode;
		unsigned int m_nowFsaa;
		unsigned int m_nowFullScreen;
		unsigned int m_nowRenderingDevice;
		unsigned int m_nowVSync;
	};

	VideoOptionTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()