#include "stdafx.h"
#include "TestGameWithLight.h"
#include "Item.h"

using namespace RocketCommander;
using namespace Ogre;

BOOST_AUTO_TEST_SUITE(ItemTest)

BOOST_AUTO_TEST_CASE(RenderTest)
{
	class RenderTestGame : public TestGameWithLight
	{
		typedef std::vector<boost::shared_ptr<Item> > Items;

		virtual void InitTest()
		{
			TestGameWithLight::InitTest();
			ShowCameraInfo();

			GetCamera().setPosition(Vector3(0, 0, -5));
			GetCamera().lookAt(Vector3(0, 0, 0));

			m_items.push_back(boost::shared_ptr<Item>(new Item(*m_sceneMgr, Item::ItemType_Fuel, Vector3::ZERO)));
			m_items.push_back(boost::shared_ptr<Item>(new Item(*m_sceneMgr, Item::ItemType_Health, Vector3(0, +3.0f, 0))));
			m_items.push_back(boost::shared_ptr<Item>(new Item(*m_sceneMgr, Item::ItemType_ExtraLife, Vector3(+3.0f, 0, 0))));
			m_items.push_back(boost::shared_ptr<Item>(new Item(*m_sceneMgr, Item::ItemType_Speed, Vector3(0, -3.0f, 0))));
			m_items.push_back(boost::shared_ptr<Item>(new Item(*m_sceneMgr, Item::ItemType_Bomb, Vector3(-3.0f, 0, 0))));
		}

		Items m_items;
	};
	
	RenderTestGame game;
	game.Run();
}

BOOST_AUTO_TEST_SUITE_END()

