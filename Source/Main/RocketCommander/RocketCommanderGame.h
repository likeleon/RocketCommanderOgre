#pragma once

#include <stack>
#include "BaseGame.h"
#include "Level.h"

namespace RocketCommander
{
	class IGameScreen;
	class Rocket;
	class GameAsteroidManager;

	class RocketCommanderGame : public BaseGame
	{
	public:
		RocketCommanderGame();
		virtual ~RocketCommanderGame();
		
		void AddGameScreen(IGameScreen *gameScreen);
		void ChangeGameScreen(IGameScreen *gameScreen);
		void ExitCurrentGameScreen();

		GameAsteroidManager& GetAsteroidManager();
		const GameAsteroidManager& GetAsteroidManager() const;
		Rocket& GetRocket();
		const Rocket& GetRocket() const;

		const Level& GetLevel(int index) const { return m_levels[index]; }

	protected:
		// Overrides BaseGame
		virtual void Initialize();
		virtual void Update(double timeSinceLastFrame);
		virtual void Cleanup();

		std::stack<IGameScreen *>	m_gameScreens;

		GameAsteroidManager			*m_asteroidManager;
		Rocket						*m_rocketModel;
		std::vector<Level>			m_levels;

	private:
		void EnterGameScreen(IGameScreen *gameScreen, bool push);
		void RemoveCurrentGameScreen();

		bool m_inGame;
	};
}