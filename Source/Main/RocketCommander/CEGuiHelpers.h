#pragma once

namespace RocketCommander
{
	class CEGuiHelpers
	{
	public:
		static const CEGUI::colour WhiteColour;
		static const CEGUI::colour LightGrayColour;
		static const CEGUI::colour GrayColour;
		static const CEGUI::colour RedColour;
		static const CEGUI::colour DarkGreenColour;

		static void SubscribeEvent(const CEGUI::String &widget, const CEGUI::String &event, const CEGUI::Event::Subscriber &method)
		{
			CEGUI::WindowManager &winMgr = CEGUI::WindowManager::getSingleton();
			if (winMgr.isWindowPresent(widget))
			{
				CEGUI::Window *window = winMgr.getWindow(widget);
				window->subscribeEvent(event, method);
			}
		}

		static CEGUI::MouseButton ConvertButton(OIS::MouseButtonID buttonId)
		{
			switch (buttonId)
			{
			case OIS::MB_Left:
				return CEGUI::LeftButton;
			case OIS::MB_Right:
				return CEGUI::RightButton;
			case OIS::MB_Middle:
				return CEGUI::MiddleButton;
			default:
				return CEGUI::LeftButton;
			}
		}

		static bool InjectMouseMove(const OIS::MouseEvent &evt)
		{
			CEGUI::System &sys = CEGUI::System::getSingleton();
			sys.injectMouseMove((float)evt.state.X.rel, (float)evt.state.Y.rel);
			if (evt.state.Z.rel)
				sys.injectMouseWheelChange(evt.state.Z.rel / 120.0f);
			return true;
		}

		static bool InjectMouseDown(OIS::MouseButtonID id)
		{
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGuiHelpers::ConvertButton(id));
			return true;
		}

		static bool InjectMouseReleased(OIS::MouseButtonID id)
		{
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGuiHelpers::ConvertButton(id));
			return true;
		}

		static CEGUI::String GetSolidColourString(const CEGUI::colour &colour)
		{
			CEGUI::String colourString = CEGUI::PropertyHelper::colourToString(colour);
			return "tl:" + colourString + 
				"tr:" + colourString + 
				"bl:" + colourString +
				"br:" + colourString;
		}
	};
}