#include "stdafx.h"
#include "PhysicsAsteroidManager.h"
#include "MathHelper.h"
#include "Asteroid.h"
#include "SmallAsteroid.h"
#include "Sound.h"

using namespace Ogre;

namespace RocketCommander
{
	void PhysicsAsteroidManager::HandleSectorPhysics(int checkX, int checkZ, int cameraSectorPosX, int cameraSectorPosZ)
	{
		Asteroids &thisSectorAsteroids = m_sectorAsteroids[checkZ][checkX];

		if (thisSectorAsteroids.empty())
			return;

		for (Asteroids::iterator astIter = thisSectorAsteroids.begin(); astIter != thisSectorAsteroids.end();)
		{
			Asteroid * const asteroid = *astIter;

			// Get position in percentage
			const float xp = asteroid->GetPosition().x / SectorWidth;
			const float zp = asteroid->GetPosition().z / SectorWidth;

			// Get sector position
			int ix = (int)MathHelper::Round(xp);
			int iz = (int)MathHelper::Round(zp);

			// -0.5: left border, +0.5: right border
			// -0.4 ~ +0.4: inside the sector
			const float borderX = xp - ix;
			const float borderZ = zp - iz;

			// Subtract current position from sector position
			ix = ix - cameraSectorPosX + MiddleSector;
			iz = iz - cameraSectorPosZ + MiddleSector;

			if (ix < 0)
				ix = 0;
			if (iz < 0)
				iz = 0;
			if (ix >= NumberOfSectors)
				ix = NumberOfSectors - 1;
			if (iz >= NumberOfSectors)
				iz = NumberOfSectors - 1;

			// Change asteroid sector if it is not longer in the correct sector.
			if (ix != checkX || iz != checkZ)
			{
				thisSectorAsteroids.erase(astIter++);
				m_sectorAsteroids[iz][ix].push_back(asteroid);
			}
			else
			{
				astIter++;
			}

			if (borderX > -0.4f && borderX < 0.4f &&
				borderZ > -0.4f && borderZ < 0.4f)
			{
				// Only check this ssecctor
				for (Asteroids::iterator otherAstIter = thisSectorAsteroids.begin(); otherAstIter != thisSectorAsteroids.end(); ++otherAstIter)
				{
					Asteroid * const otherAsteroid = *otherAstIter;
					if (asteroid != otherAsteroid)
					{
						const float maxAllowedDistance = otherAsteroid->GetCollisionRadius() + 
							asteroid->GetCollisionRadius();

						if ((otherAsteroid->GetPosition() - asteroid->GetPosition()).length() < maxAllowedDistance)
						{
							HandleAsteroidCollision(*asteroid, *otherAsteroid);
						}
					}
				}
			}
			else
			{
				// Check neighbor sectors too
				for (int z = checkZ - 1; z <= checkZ + 1; ++z)
				{
					if (z < 0 || z >= NumberOfSectors)
						continue;

					for (int x = checkX - 1; x <= checkX + 1; ++x)
					{
						if (x < 0 || x >= NumberOfSectors || !m_sectorIsVisible[z][x])
							continue;
						
						for (Asteroids::iterator otherAstIter = thisSectorAsteroids.begin(); otherAstIter != thisSectorAsteroids.end(); ++otherAstIter)
						{
							Asteroid * const otherAsteroid = *otherAstIter;
							if (asteroid != otherAsteroid)
							{
								const float maxAllowedDistance = otherAsteroid->GetCollisionRadius() + 
									asteroid->GetCollisionRadius();

								if ((otherAsteroid->GetPosition() - asteroid->GetPosition()).length() < maxAllowedDistance)
								{
									HandleAsteroidCollision(*asteroid, *otherAsteroid);
								}
							}
						}
					}
				}
			}
		}
	}

	void PhysicsAsteroidManager::HandleAsteroidCollision(Asteroid &asteroid1, Asteroid &asteroid2)
	{
		const float maxAllowedDistance = asteroid1.GetCollisionRadius() + asteroid2.GetCollisionRadius();
		//const float distance = (asteroid2.GetPosition() - asteroid1.GetPosition()).length();

		// Collision point
		const Vector3 middle = asteroid1.GetPosition() * (asteroid2.GetCollisionRadius() / maxAllowedDistance) +
			asteroid2.GetPosition() * (asteroid1.GetCollisionRadius() / maxAllowedDistance);

		// Relative positions to middle
		Vector3 relPos1 = asteroid1.GetPosition() - middle;
		relPos1.normalise();
		Vector3 relPos2 = asteroid2.GetPosition() - middle;
		relPos2.normalise();

		asteroid1.SetPosition(middle + relPos1 * asteroid1.GetCollisionRadius() * 1.015f);
		asteroid2.SetPosition(middle + relPos2 * asteroid2.GetCollisionRadius() * 1.015f);

		// Move speed
		const float speed1 = asteroid1.GetMovement().length();
		const float speed2 = asteroid2.GetMovement().length();

		// Mass = size * size
		const float mass1 = asteroid1.GetSize() * asteroid1.GetSize();
		const float mass2 = asteroid2.GetSize() * asteroid2.GetSize();
		const float bothMasses = mass1 + mass2;

		// Force = speed * mass
		const float force1 = speed1 * mass1;
		const float force2 = speed2 * mass2;
		const float bothForces = force1 + force2;

		// Copy over normals
		const Vector3 normal1 = relPos1;
		const Vector3 normal2 = relPos2;

		// Normalize movement
		Vector3 direction1 = asteroid1.GetMovement();
		if (direction1 != Vector3::ZERO)
			direction1.normalise();
		Vector3 direction2 = asteroid2.GetMovement();
		if (direction2 != Vector3::ZERO)
			direction2.normalise();

		// Collision strength (1 = same dir, 0 = 90 degree)
		float collisionStrength1 = std::abs(direction1.dotProduct(normal1));
		float collisionStrength2 = std::abs(direction2.dotProduct(normal2));

		Vector3 reflection1 = ReflectVector(direction1, normal1);
		Vector3 reflection2 = ReflectVector(direction2, normal2);
		if (direction1.length() <= 0.01f)
		{
			collisionStrength1 = collisionStrength2;
			reflection1 = normal1;
		}
		if (direction2.length() <= 0.01f)
		{
			collisionStrength2 = collisionStrength1;
			reflection2 = normal2;
		}

		float movement1 = (1 - collisionStrength1) * speed1;
		movement1 += collisionStrength1 * (std::abs(mass1 - mass2) * speed1 + (2 * mass2 * speed2)) / bothMasses;
		asteroid1.SetMovement(reflection1 * movement1);
	
		float movement2 = (1 - collisionStrength2) * speed2;
		movement2 += collisionStrength2 * (std::abs(mass2 - mass1) * speed2 + (2 * mass1 * speed1)) / bothMasses;
		asteroid2.SetMovement(reflection2 * movement2);

		asteroid1.SetRotationSpeed(Vector2(asteroid1.GetRotationSpeed().x, 
			(asteroid1.GetRotationSpeed().y + Math::RangeRandom(-0.75f, +1.0f)) / 2.0f));
		asteroid2.SetRotationSpeed(Vector2(asteroid2.GetRotationSpeed().x, 
			(asteroid2.GetRotationSpeed().y + Math::RangeRandom(-0.75f, +1.0f)) / 2.0f));
	}

	Vector3 PhysicsAsteroidManager::ReflectVector(const Ogre::Vector3 &vec, const Ogre::Vector3 &normal)
	{
		return (vec - normal * 2.0f * vec.dotProduct(normal));
	}

	void PhysicsAsteroidManager::KillAllInnerSectorAsteroids()
	{
		for (int z = -1; z <= +1; ++z)
		{
			for (int x = -1; x <= +1; ++x)
			{
				// Kill asteroids
				Asteroids &asteroids = m_sectorAsteroids[MiddleSector + z][MiddleSector + x];
				for (Asteroids::iterator asteroid = asteroids.begin(); asteroid != asteroids.end(); ++asteroid)
				{
					delete (*asteroid);
				}
				asteroids.clear();

				// Kill small asteroids
				SmallAsteroids &smallAsteroids = m_sectorSmallerAsteroids[SmallMiddleSector + z][SmallMiddleSector + x];
				for (SmallAsteroids::iterator smallAsteroid = smallAsteroids.begin(); smallAsteroid != smallAsteroids.end(); ++smallAsteroid)
				{
					delete (*smallAsteroid);
				}
				smallAsteroids.clear();
			}
		}
	}

	// 0 if no collision happened, 1 for a frontal collsion.
	float PhysicsAsteroidManager::PlayerAsteroidCollision(Player &player)
	{
		const Vector3 &camPos = m_game.GetCamera().getPosition();
		Vector3 camDir = Vector3(0, 0, -1);
		//camDir = m_game.GetCamera().getViewMatrix().transformAffine(camDir);
		Matrix3 viewMat3;
		m_game.GetCamera().getViewMatrix().extract3x3Matrix(viewMat3);
		camDir = viewMat3 * camDir;
		const Vector3 nextCamPos1 = camPos + 
			camDir * 10.0f;
		const Vector3 nextCamPos2 = camPos +
			camDir * (10.0f + player.GetSpeed() * m_game.GetMoveFactorPerSecond() * player.GetMovementSpeedPerSecond());
		float damageFactor = 0.0f;
		float remToPlayWhoshVolume = 0.0f;

		for (int z = -1; z <= +1; ++z)
		{
			for (int x = -1; x <= +1; ++x)
			{
				Asteroids &thisSector = m_sectorAsteroids[MiddleSector + z][MiddleSector + x];
				for (Asteroids::iterator astIter = thisSector.begin(); astIter != thisSector.end();)
				{
					Asteroid &asteroid = *(*astIter);
					bool removeThisAsteroid = false;

					// Check distance to camera
					const float distance = (asteroid.GetPosition() - camPos).length();

					// If distance is smaller than collision radius, we could have
					// a hit. Now do polygon based collision testing.
					if (distance <= asteroid.GetSize() * 0.825157789f)
					{
						// Add 25% than just flying by very closely
						float thisDamageFactor = 0.175f + 
							0.25f * std::max(0.0f, 1.0f - (distance / (asteroid.GetCollisionRadius() * 1.025f)));

						// Big asteroids to more damage
						thisDamageFactor *= 0.5f + 0.5f * (asteroid.GetSize() / MaxAsteroidSize);

						// Check highest damange factor
						if (thisDamageFactor > damageFactor)
							damageFactor = thisDamageFactor;

						// Play side hit sound
						// Play only once (asteroid is killed after that)
						m_game.GetSound().Play(Sound::Sounds_SideHit);

						// Remeber to show hit direction effect
						Vector3 distVector = asteroid.GetPosition() - camPos;
						distVector = m_game.GetCamera().getViewMatrix().transformAffine(distVector);
						SetHitDirectionEffect(Math::ATan2(distVector.x, distVector.y));

						if (player.GetNumberOfBombItems() > 0)
						{
							player.SetNumberOfBombItems(player.GetNumberOfBombItems() - 1);
							damageFactor = 0.0f;
						}

						// Remove asteroid we hit
						removeThisAsteroid = true;	
					} // if (distance)

					const float distanceSoon = (asteroid.GetPosition() - nextCamPos1).length();
					// Check if some asteroids are getting really close
					if (distanceSoon < MaxAsteroidSize * 3.5f)
					{
						// Find out if the asteroid is not longer moving closer
						if (distanceSoon < (asteroid.GetPosition() - nextCamPos2).length())
						{
							// Play one of the whosh sounds
							float loudness = 1.0f - 
								((distanceSoon - asteroid.GetCollisionRadius() * 1.5f) / 
								(MaxAsteroidSize * 3.5f - asteroid.GetCollisionRadius() * 1.5f));
							loudness *= 1.25f;
							if (loudness > 1.0f)
								loudness = 1.0f;

							if (loudness > 0.1f)
							{
								// Play whosh sound
								const float newVolume = 0.5f + 0.5f * loudness;
								if (newVolume > remToPlayWhoshVolume)
									remToPlayWhoshVolume = newVolume;

								if (distance <= asteroid.GetCollisionRadius() * 2.5f)
								{
									player.SetCameraWobbel(0.33f * (1.0f - (distance / (asteroid.GetCollisionRadius() * 2.0f))));
								}

								// Add a little score for flying so close
								if (m_game.GetTotalFrames() % 20 == 0)
									player.SetScore(player.GetScore() + (int)m_game.GetElapsedTimeThisFrameInMs());
							}
						}
					} // if (distanceSoon)

					if (removeThisAsteroid)
					{
						thisSector.erase(astIter++);
						delete &asteroid;
					}
					else
					{
						astIter++;
					}
				}
			} // for (int x)
		} // for (int z)

		// Max. check every 10 frames, else we have to many whosh sounds
		if (remToPlayWhoshVolume > 0 &&
			m_game.GetTotalFrames() % 5 == 0)
			m_game.GetSound().PlayWhosh(remToPlayWhoshVolume);
		return damageFactor;
	}
}