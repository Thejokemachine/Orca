#include "LayoutParser.h"

#include "Logger.h"
#include "Layout.h"
#include "ILayoutObject.h"
#include "StringUtilities.h"

#include "LayoutSequence.h"

using namespace orca;

std::expected<CLayout*, std::string> CLayoutParser::ParseLayoutFromFile(const std::filesystem::path& filepath)
{
	pugi::xml_document doc;

	auto result = doc.load_file(filepath.c_str());

	if (result.status != pugi::xml_parse_status::status_ok)
	{
		return std::unexpected(result.description());
	}

	return ParseLayoutFromXml(doc);
}

std::expected<CLayout*, std::string> CLayoutParser::ParseLayoutFromString(std::string_view buffer)
{
	pugi::xml_document doc;

	auto result = doc.load_buffer(buffer.data(), buffer.size());

	if (result.status != pugi::xml_parse_status::status_ok)
	{
		return std::unexpected(result.description());
	}

	return ParseLayoutFromXml(doc);
}

std::expected<CLayout*, std::string> CLayoutParser::ParseLayoutFromXml(const pugi::xml_document& doc)
{
	// Read prefabs
	std::string prefabId;
	const auto& docPrefabs = doc.children("prefab");
	for (auto& docPrefab : docPrefabs)
	{
		auto attrId = docPrefab.attribute("id");
		if (attrId.empty())
		{
			return std::unexpected("Prefab element does not contain an Id attribute!");
		}
		prefabId = attrId.as_string();
		DebugLogF("Found prefab object with id: {}", prefabId);

		auto prefab = new SPrefab();
		prefab->node = docPrefab;
		prefab->factoryFunc = []() {
			return std::make_shared<CLayoutObject>();
		};
		mPrefabs[prefabId].reset(prefab);
	}

	// Read layout
	const auto& docLayouts = doc.children("layout");
	auto count = std::count_if(docLayouts.begin(), docLayouts.end(), [](const auto&) { return true; });
	if (count == 0)
	{
		return std::unexpected("No root \"layout\" element found!");
	}
	if (count > 1)
	{
		return std::unexpected("More than one \"layout\" element found!");
	}

	const auto& docLayout = *docLayouts.begin();
	std::string layoutId;

	auto attrId = docLayout.attribute("id");
	if (attrId.empty())
	{
		return std::unexpected("Layout element does not contain an Id attribute!");
	}
	layoutId = attrId.as_string();
	DebugLogF("Found root object with id: {}", layoutId);

	CLayout* layout = new CLayout();
	layout->SetId(layoutId);
	if (!layout->ParseAttributes(docLayout))
	{
		delete layout;
		return std::unexpected("Layout ParseAttributes failed!");
	}

	std::vector<std::shared_ptr<ILayoutObject>> createdObjects;
	AddChildren(*layout, docLayout, createdObjects);

	for (auto& obj : createdObjects)
	{
		layout->AddObject(obj);
	}

	return layout;
}

void CLayoutParser::AddChildren(ILayoutObject& layoutObject, const pugi::xml_node& node, std::vector<std::shared_ptr<ILayoutObject>>& createdObjects)
{
	for (const auto& childNode : node)
	{
		std::shared_ptr<ILayoutObject> child = nullptr;
		auto type = childNode.name();
		if (!strcmp(type, "panel"))
		{
			child = std::make_shared<CLayoutObject>();
		}
		else if (!strcmp(type, "sequence"))
		{
			child = std::make_shared<CLayoutSequence>();
		}
		else if (auto it = mPrefabs.find(type); it != mPrefabs.end())
		{
			const SPrefab& prefab = *it->second;
			child = prefab.factoryFunc();
			if (child->ParseAttributes(prefab.node))
			{
				AddChildren(*child, prefab.node, createdObjects);
			}
			child->SetId(utils::emptyString);
		}
		else
		{
			DebugLogF("\"{}\" is not a registered layout type!", type);
		}

		if (child)
		{
			if (child->ParseAttributes(childNode))
			{
				AddChildren(*child, childNode, createdObjects);
			}
			createdObjects.emplace_back(child);
			layoutObject.AddChild(child);
		}
	}
}