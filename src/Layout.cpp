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
		DebugLogF("Added named layout object: \"{}\"", id);
	}
}

std::weak_ptr<ILayoutObject> CLayout::FindObject(const std::string& id)
{
	if (auto it = mNamedObjects.find(id); it != mNamedObjects.end())
	{
		return it->second;
	}
	return std::weak_ptr<ILayoutObject>();
}

void CLayout::Update(int32_t forceFlags)
{
	CLayoutObject::Update(forceFlags);
}

void CLayout::SetParent(ILayoutObject* parent)
{
	assert(false && "Layouts should not have a parent!");
}

bool CLayout::ParseAttributes(const pugi::xml_node& attributes)
{
	mColor.a = 0; // Layout background should be invisible by default
	return CLayoutObject::ParseAttributes(attributes);
}
