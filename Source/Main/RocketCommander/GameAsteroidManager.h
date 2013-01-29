#pragma once

#include "PhysicsAsteroidManager.h"

namespace RocketCommander
{
	class Item;
	class Goal;

	class GameAsteroidManager : public PhysicsAsteroidManager
	{
	public:
		GameAsteroidManager(BaseGame &game, const Level &level);
		virtual ~GameAsteroidManager();

		// Overrides PhysicsAsteroidManager
		virtual void SetLevel(const Level &level);
		virtual void ShowAllItems();

	private:
		static const int MaxHitDirectionTimeoutMs = 2000;

		// Overrides PhysicsAsteroidManager
		virtual void SetHitDirectionEffect(const Ogre::Radian &direction); 

		void ClearAllItems();
		void ClearGoal();

		std::list<Item *> m_items;
		int m_hitDirectionEffectTimeoutMs;
		Ogre::Radian m_hitDirection;
		Goal *m_goal;
	};
}