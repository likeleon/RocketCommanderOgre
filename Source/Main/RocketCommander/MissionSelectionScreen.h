#pragma once

#include "GameScreen.h"
#include "Sound.h"

namespace RocketCommander
{
	class Line2D;

	class MissionSelector
	{
	public:
		class IEventListener
		{
		public:
			virtual ~IEventListener() {}
			virtual bool OnSelected(MissionSelector *selector) = 0;
		};

		MissionSelector(const Ogre::String &name, 
			const Level &level, 
			Ogre::SceneManager &sceneMgr, 
			Sound &sound,
			IEventListener &eventListener);
		~MissionSelector();

		const Level& GetLevel() const { return m_level; }

		bool OnButtonMouseEnters(const CEGUI::EventArgs &e);
		bool OnButtonMouseLeaves(const CEGUI::EventArgs &e);
		bool OnButtonMouseClick(const CEGUI::EventArgs &e);

	private:
		const Ogre::String				m_name;
		Sound							&m_sound;
		IEventListener					&m_eventListener;
		CEGUI::DefaultWindow			*m_txtName;
		std::vector<CEGUI::PushButton*> m_btnImages;
		std::vector<Line2D *>			m_lines;
		const Level						&m_level;
	};

	class MissionSelectionScreen : public GameScreen, public MissionSelector::IEventListener
	{
	public:
		MissionSelectionScreen(RocketCommanderGame &game);

		// Overrides GameScreen
		virtual Ogre::String GetName() const { return "MissionSelection"; }
		virtual void Enter();
		virtual void Run();
		virtual void Exit();

	private:
		// Implements MissionSelector::IEventListener
		virtual bool OnSelected(MissionSelector *selector);

		std::vector<MissionSelector *> m_missionSelectors;
		const Level *m_selectedLevel;
	};
}