#pragma once

namespace RocketCommander
{
	class Line2D
	{
	public:
		Line2D(Ogre::SceneManager &sceneMgr, const Ogre::Vector2 &startPoint, const Ogre::Vector2 &endPoint, const Ogre::ColourValue &colour);
		~Line2D();

		void SetColour(const Ogre::ColourValue &colour);

	private:
		static int instanceId;

		Ogre::SceneManager	&m_sceneMgr;
		Ogre::ManualObject	*m_manObj;
		Ogre::SceneNode		*m_manObjNode;
		Ogre::MaterialPtr	m_manObjMaterial;
	};
}