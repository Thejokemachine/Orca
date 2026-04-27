#include "LayoutParser.h"

#include "Logger.h"
#include "Layout.h"

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

std::expected<CLayout*, std::string> CLayoutParser::ParseLayoutFromString(const std::string& buffer)
{
	pugi::xml_document doc;

	auto result = doc.load_buffer(buffer.c_str(), buffer.size());

	if (result.status != pugi::xml_parse_status::status_ok)
	{
		return std::unexpected(result.description());
	}

	return ParseLayoutFromXml(doc);
}

std::expected<CLayout*, std::string> CLayoutParser::ParseLayoutFromXml(const pugi::xml_document& doc)
{
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
	DebugLog(std::format("Found root object with id: {}", layoutId));

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
		auto child = std::make_shared<CLayoutObject>();
		layoutObject.AddChild(child);
		if (child->ParseAttributes(childNode))
		{
			AddChildren(*child, childNode, createdObjects);
		}
		createdObjects.emplace_back(child);
	}
}