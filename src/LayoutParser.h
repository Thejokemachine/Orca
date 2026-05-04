#ifndef __ORCA_LAYOUTPARSER__
#define __ORCA_LAYOUTPARSER__

#include <filesystem>
#include <string>
#include <expected>
#include <memory>
#include <pugixml.hpp>
#include <functional>

namespace orca
{
	class CLayout;
	class ILayoutObject;

	class CLayoutParser
	{
	public:
		std::expected<CLayout*, std::string> ParseLayoutFromFile(const std::filesystem::path& filepath);
		std::expected<CLayout*, std::string> ParseLayoutFromString(std::string_view buffer);
		std::expected<CLayout*, std::string> ParseLayoutFromXml(const pugi::xml_document& doc);

	private:
		void AddChildren(ILayoutObject& layoutObject, const pugi::xml_node& node, std::vector<std::shared_ptr<ILayoutObject>>& createdObjects);

		struct SPrefab
		{
			pugi::xml_node node;
			std::function<std::shared_ptr<ILayoutObject>()> factoryFunc;
		};
		std::unordered_map<std::string, std::unique_ptr<SPrefab>> mPrefabs;
	};
}

#endif