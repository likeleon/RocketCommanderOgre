#pragma once

#include "GameScreen.h"

namespace RocketCommander
{
	class OptionsScreen : public GameScreen
	{
	public:
		OptionsScreen(RocketCommanderGame &game);
		virtual ~OptionsScreen();

		// Overrides GameScreen
		virtual Ogre::String GetName() const { return "Options"; }
		virtual void Enter();

	private:
		class RendererOption
		{
		public:
			RendererOption(const std::string &name, const CEGUI::String &windowNamePrefix);
			~RendererOption();

			void UpdateValue(const Ogre::RenderSystem &renderer);
			const std::string& GetName() const;
			std::string GetValue() const;
			bool IsAvailable() const;

			bool OnMouseClick(const CEGUI::EventArgs &e);
			bool OnMouseEnters(const CEGUI::EventArgs &e);
			bool OnMouseLeaves(const CEGUI::EventArgs &e);

		private:
			const std::string	m_name;
			CEGUI::Window		*m_txtCaption;
			CEGUI::Window		*m_txtValue;
			Ogre::StringVector  m_possibleValues;
			bool				m_isAvailable;
		};

		void UpdateOptionValues(const Ogre::RenderSystem &renderer);

		// UI event handlers
		bool OnApplyChangesMouseClick(const CEGUI::EventArgs &e);
		bool OnApplyChangesMouseEnters(const CEGUI::EventArgs &e);
		bool OnApplyChangesMouseLeaves(const CEGUI::EventArgs &e);
		bool OnRendererMouseClick(const CEGUI::EventArgs &e);
		bool OnRendererMouseEnters(const CEGUI::EventArgs &e);
		bool OnRendererMouseLeaves(const CEGUI::EventArgs &e);

		CEGUI::Window	*m_txtRenderer;
		typedef std::list<RendererOption *> RendererOptions;
		RendererOptions m_rendererOptions;
	};
}