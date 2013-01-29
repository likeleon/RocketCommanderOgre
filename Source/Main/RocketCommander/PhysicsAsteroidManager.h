#pragma once

#include "BaseAsteroidManager.h"
#include "Player.h"

namespace RocketCommander
{
	class PhysicsAsteroidManager : public BaseAsteroidManager
	{
	public:
		PhysicsAsteroidManager(BaseGame &m_game, const Level &level)
			: BaseAsteroidManager(m_game, level) {}

		void KillAllInnerSectorAsteroids();
		float PlayerAsteroidCollision(Player &player);

	protected:
		// Overrides BaseAsteroidManager
		virtual void HandleSectorPhysics(int checkX, int checkZ, int cameraSectorPosX, int cameraSectorPosZ);

		// Implement in derived class
		virtual void SetHitDirectionEffect(const Ogre::Radian &direction) {}

	private:
		static Ogre::Vector3 ReflectVector(const Ogre::Vector3 &vec, const Ogre::Vector3 &normal);
		void HandleAsteroidCollision(Asteroid &asteroid1, Asteroid &asteroid2);
	};
}