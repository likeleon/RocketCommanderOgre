#include "stdafx.h"
#include "TestGame.h"
#include "Line2D.h"
#include "RandomHelper.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(Line2DTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGame
	{
		virtual bool keyPressed(const OIS::KeyEvent &evt)
		{
			if (evt.key == OIS::KC_1)
			{
				boost::shared_ptr<Line2D> line2D(new Line2D(*m_sceneMgr, RandomHelper::GetRandomVector2(-1, 1), 
					RandomHelper::GetRandomVector2(-1, 1), RandomHelper::GetRandomColour()));
				m_lines.push_back(line2D);
			}
			else if (evt.key == OIS::KC_2 && !m_lines.empty())
			{
				m_lines.pop_front();
			}

			m_leftTopDebug->setText("lines: " + StringConverter::toString(m_lines.size()));
			return TestGame::keyPressed(evt);
		}

		std::deque<boost::shared_ptr<Line2D> > m_lines;
	};
	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()