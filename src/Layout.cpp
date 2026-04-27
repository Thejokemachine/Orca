#include "Layout.h"

#include <pugixml.hpp>

#include <cassert>
#include <algorithm>

#include "Logger.h"
#include "LayoutParser.h"

using namespace orca;

CLayout::CLayout()
{
}

void CLayout::AddObject(const std::shared_ptr<ILayoutObject> layoutObject)
{
	mObjects.insert(layoutObject);
	if (const auto& id = layoutObject->GetId(); !id.empty())
	{
		mNamedObjects[id] = layoutObject;
		DebugLog(std::format("Added named layout object: \"{}\"", id));
	}
}

void CLayout::Update()
{
	CLayoutObject::Update();
}

void CLayout::SetParent(ILayoutObject* parent)
{
	assert(false && "Layouts should not have a parent!");
}

bool CLayout::ParseAttributes(const pugi::xml_node& attributes)
{
	// Read any layout attributes
	return true;
}

void CLayout::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const auto& layoutObj : mChildren)
	{
		if (auto ptr = layoutObj.lock(); ptr && ptr->GetVisible())
		{
			target.draw(*ptr, states);
		}
	}
}
