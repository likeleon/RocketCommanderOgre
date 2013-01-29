#include "stdafx.h"
#include "GameAsteroidManager.h"
#include "RandomHelper.h"
#include "Item.h"
#include "Goal.h"

using namespace Ogre;

namespace RocketCommander
{
	GameAsteroidManager::GameAsteroidManager(BaseGame &game, const Level &level)
	 : PhysicsAsteroidManager(game, level),
	   m_hitDirectionEffectTimeoutMs(0),
	   m_hitDirection(0.0f),
	   m_goal(NULL)
	{
	}

	GameAsteroidManager::~GameAsteroidManager()
	{
		ClearAllItems();
		ClearGoal();
	}

	void GameAsteroidManager::SetLevel(const Level &level)
	{
		PhysicsAsteroidManager::SetLevel(level);

		ClearAllItems();
		ClearGoal();

		// Copy over all items
		const Level::Items &items = level.GetItems();
		for (Level::Items::const_iterator itemIter = items.begin(); itemIter != items.end(); ++itemIter)
		{
			const Item::ItemTypes itemType = itemIter->first;
			const Level::ItemPositions &itemPositions = itemIter->second;

			for (Level::ItemPositions::const_iterator itemPosIter = itemPositions.begin(); itemPosIter != itemPositions.end(); ++itemPosIter)
			{
				const Vector3 &itemPos = (*itemPosIter);
				m_items.push_back(new Item(m_game.GetSceneManager(), itemType, itemPos)); 
			}
		}

		// Goal at target position
		m_goal = new Goal(m_game.GetSceneManager(), m_targetPos);
	}

	void GameAsteroidManager::SetHitDirectionEffect(const Ogre::Radian &direction)
	{
		m_hitDirectionEffectTimeoutMs = MaxHitDirectionTimeoutMs;
		m_hitDirection = direction;
	}

	void GameAsteroidManager::ShowAllItems()
	{
		for (std::list<Item *>::iterator itemIter = m_items.begin(); itemIter != m_items.end();)
		{
			Item &item = *(*itemIter);

			const Vector3 &itemPos = item.GetPosition();

			// Distance to viewer
			const float distance = (itemPos - m_game.GetCamera().getPosition()).length();

			bool itemRemoved = false;

			// Skip if out of visible range
			if (distance > GetMaxViewDepth() * 6)
				goto proceed_to_next_item;

			// If very close to item, collect it
			if (distance <= Item::ItemSize * 2.15f)
			{
				m_game.GetPlayer().HandleItem(item.GetType());

				// Remove item
				delete &item;
				m_items.erase(itemIter++);
				itemRemoved = true;
			}

proceed_to_next_item:
			if (!itemRemoved)
				++itemIter;
		}
	}
	
	void GameAsteroidManager::ClearAllItems()
	{
		for (std::list<Item *>::iterator item = m_items.begin(); item != m_items.end(); ++item)
		{
			delete *item;
		}
		m_items.clear();
	}

	void GameAsteroidManager::ClearGoal()
	{
		if (m_goal)
			delete m_goal;
		m_goal = NULL;
	}
}
