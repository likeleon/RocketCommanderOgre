#include "stdafx.h"
#include "TestGame.h"
#include "BaseAsteroidManager.h"
#include "LensFlare.h"

using namespace RocketCommander;

BOOST_AUTO_TEST_SUITE(BaseAsteroidManagerTest)

BOOST_AUTO_TEST_CASE(RenderAsteroidsTest)
{
	class RenderAsteroidsTestGame : public TestGame
	{
		virtual void InitTest()
		{
			ShowCameraInfo();
			EnableSkyBox();
			m_asteroidManager.reset(new BaseAsteroidManager(*this, Level::LoadAllLevels()[0]));
		}

		virtual void UpdateTest(double timeSinceLastFrame)
		{
			m_asteroidManager->Update();
			m_lensFlare->Render(m_remLensFlareColor);

			const std::vector<std::vector<bool> > &visible = m_asteroidManager->GetSectorIsVisible();
			std::string msg = "";

			for (int z = 0; z < BaseAsteroidManager::NumberOfSectors; ++z)
			{
				for (int x = 0; x < BaseAsteroidManager::NumberOfSectors; ++x)
				{
					msg += visible[z][x] ? "*" : "-";
				}
				msg += "\r\n";
			}
			m_rightTopDebug->setText(msg);
		}

		boost::shared_ptr<BaseAsteroidManager> m_asteroidManager;
	};

	RenderAsteroidsTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()
