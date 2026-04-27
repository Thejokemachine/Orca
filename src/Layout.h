#ifndef __ORCA_LAYOUT__
#define __ORCA_LAYOUT__

#include <expected>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <pugixml.hpp>

#include "LayoutObject.h"

namespace orca
{
	class CLayout : public CLayoutObject
	{
	public:
		CLayout();

		void AddObject(const std::shared_ptr<ILayoutObject> layoutObject);

		/*
		* ILayoutObject
		*/
		virtual void Update() override;
		virtual void SetParent(ILayoutObject* parent) override;
		virtual bool ParseAttributes(const pugi::xml_node& attributes) override;

		/*
		* sf::Drawable
		*/
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:

		std::unordered_set<std::shared_ptr<ILayoutObject>> mObjects;
		std::unordered_map<std::string, std::weak_ptr<ILayoutObject>> mNamedObjects;

	};
}

#endif